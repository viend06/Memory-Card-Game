//game.cpp
#include "include/game.h"
#include "include/utils.h"
#include <algorithm>
#include <iostream> 
#include <random>
#include <SDL2/SDL_ttf.h>
using namespace std ;
int hangthe = 4;
int cotthe = 5;
int chieurongluoithe = cotthe * (CHIEURONGTHE + KHOANGCACHTHE) - KHOANGCACHTHE;
int chieudailuoithe = hangthe * (CHIEUDAITHE + KHOANGCACHTHE) - KHOANGCACHTHE;
int LUOITHE_X = (CHIEURONGMANHINH - chieurongluoithe) / 2 - 180;
int LUOITHE_Y = (CHIEUDAIMANHINH - chieudailuoithe) / 2 + 21 ;
Uint32 TIME_LIMIT = 10000;
int currentLevel = 1;
Uint32 startTime;
int diem = 0;//
Uint32 timeRemaining = 10000; 
bool timeUp = false;
bool showLevelComplete = false;
Uint32 levelCompleteTime = 0;
int countdown = 5;
bool showCountdown = false;
vector<Card> cards;
Card* the1 = nullptr;
Card* the2 = nullptr;
bool waiting = false;
Uint32 lastFlipTime = 0;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
vector<SDL_Texture*> cardTextures;
SDL_Texture* backTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* HINHNENNHO = nullptr;
Mix_Music* nhacnen = nullptr;
Mix_Chunk* cardFlipSound = nullptr;
Mix_Chunk* gameOverSound = nullptr;
SDL_Texture* ANHNENMENU = nullptr;
bool showMenu = true;
SDL_Texture* anhnutstart = nullptr;
SDL_Rect startButtonRect = {CHIEURONGMANHINH/2 - 100, CHIEUDAIMANHINH/2 - 50, 200, 100};// khởi tạo 1 hình chữ nhật với vị trí và kích thước 
vector<string> imagePaths = {"assets/iamge.png", "assets/iamge1.jpg", "assets/iamge2.jpg", "assets/iamge3.jpg", "assets/iamge4.jpg", "assets/iamge5.jpg", "assets/iamge6.jpg", "assets/iamge7.jpg","assets/iamge8.jpg","assets/iamge9.png"};//
struct CardSwapanimation {
    Card* card1;
    Card* card2;
    Uint32 startTime;
    bool active;
    int startX1, startY1, startX2, startY2;
    int targetX1, targetY1, targetX2, targetY2; // Thêm dòng này
};
SDL_Texture* gameOverTexture = nullptr; // Thêm dòng này cùng với các biến texture khác
CardSwapanimation swapAnimation={nullptr , nullptr , 0 , false,0,0,0,0};
const Uint32 SWAP_DURATION = 500;
bool showLevelCompleteSlide = false;
Uint32 slideStartTime = 0 ; 
int slideOffset = - SLIDE_HEIGHT ;
bool flippingAllCards = false;
Uint32 flipAllStartTime = 0 ;
bool shouldResetTime = false;
SDL_Point getGridPosition(int row, int col) {
    return {LUOITHE_X + col * (CHIEURONGTHE + KHOANGCACHTHE),LUOITHE_Y + row * (CHIEUDAITHE + KHOANGCACHTHE)};
}
void renderMenu() {
    SDL_RenderCopy(renderer, ANHNENMENU, NULL, NULL); // hàm vẽ texture lên renderer, texture chứa hình nền menu,nullnull vẽ phủ toàn bộ kh cắt xénxén 
    SDL_RenderCopy(renderer, anhnutstart, NULL, &startButtonRect); // vẽ nút start lên renderer với vị trí và kích thước cho trướctrước
    TTF_Font* font = TTF_OpenFont("OpenSans-VariableFont_wdth,wght.ttf", 36);
    if (font) { // nếu load được fontfont
        SDL_Color textColor = {255, 255, 255, 255}; // Trắng
        SDL_Color outlineColor = {0, 0, 0, 255};    // Đen
        int textWidth, textHeight;
        TTF_SizeText(font, "START", &textWidth, &textHeight);// tính toán kích thước của đoạn văn bản dựa trên font chữ và nội dungdung
        int x = startButtonRect.x + (startButtonRect.w - textWidth)/2;
        int y = startButtonRect.y + (startButtonRect.h - textHeight)/2;
        renderTextWithOutline(renderer, font, "START", textColor, outlineColor, 2, x, y); // Viền dày 2px
        TTF_CloseFont(font);
    }
    SDL_RenderPresent(renderer); // cập nhật renderer để hiển thị tất cả những gì đã vẽ lên màn hình
}
void shuffleCards(const vector<string>& imagePaths) {
    vector<int> values;
    for (int i = 0; i < 10; i++) {
        values.push_back(i);
        values.push_back(i);
    }
    std::random_device rd; // lấy seed ngẫu nhiên từ phần cứng
    std::default_random_engine rng(rd()); // Bộ sinh số ngẫu nhiên
    std::shuffle(values.begin(), values.end(), rng); // xáo trộn các giá trị
    cards.clear();// xóa danh sách bài cũ 
    int index = 0;// chỉ số để lấy giá trị từ vector đã xáo trộntrộn
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 5; col++) {
            Card card = {
                LUOITHE_X + col * (CHIEURONGTHE + KHOANGCACHTHE),// tính toán vị trí của lá bài trên màn hình
                LUOITHE_Y + row * (CHIEUDAITHE + KHOANGCACHTHE),
                values[index++], // giá trị bài 
                false, // chưa lật
                false, // chưa khớp
                false,// hiệu ứng
                 false,// không lật ngược
                  0};// chưa bắt đầu lật
            cards.push_back(card);// thêm lá bài vào danh sách
        }
    }
    cardTextures.clear();// xóa danh sách texture cũ 
    for (const auto& path : imagePaths) {
        SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
        cardTextures.push_back(texture);
    }
}
void handleMouseClick(int x, int y) {
    if (waiting || timeUp) return;
    for (auto &card : cards) {
        if (!card.flipped && !card.matched && x > card.x && x < card.x + CHIEURONGTHE && y > card.y && y < card.y + CHIEUDAITHE) {
            Mix_PlayChannel(-1, cardFlipSound, 0);
            card.hieuung = true;card.flippingBack = false;
            card.flipStartTime = SDL_GetTicks();
            break;
        }
    }
}
void updateGame() {
    Uint32 currentTime = SDL_GetTicks();
    if(!showLevelComplete && !showCountdown){
        if(currentTime - startTime >= TIME_LIMIT){
            timeUp = true ; 
            timeRemaining = 0 ;
            return ;
        }
        timeRemaining = TIME_LIMIT -(currentTime - startTime);
        if(timeRemaining <= 0 ){
            timeRemaining = 0 ; 
            timeUp = true ; 
        }
    }
    for (auto &card : cards) {
        if (card.hieuung) {
            if (currentTime - card.flipStartTime >= THOIGIANLATTHE) {
                if (!card.flippingBack) {
                    card.flipped = !card.flipped;
                    if (!the1) {
                        the1 = &card;
                    } else if (!the2) {
                        the2 = &card;
                        waiting = true;
                        lastFlipTime = SDL_GetTicks();
                    }
                } else {
                    card.flipped = false;
                }
                card.hieuung = false;
            }
        }
    }
    if (waiting && SDL_GetTicks() - lastFlipTime > 500) {
        if (the1->value == the2->value) {
            the1->matched = true;
            the2->matched = true;
            diem+=10;
        } else {
        // Thay thế toàn bộ phần này
            swapAnimation.card1 = the1;
            swapAnimation.card2 = the2;
            swapAnimation.startX1 = the1->x;
            swapAnimation.startY1 = the1->y;
            swapAnimation.startX2 = the2->x;
            swapAnimation.startY2 = the2->y;
    // Tính toán vị trí đích dựa trên lưới
            int row1 = (the1->y - LUOITHE_Y) / (CHIEUDAITHE + KHOANGCACHTHE);
            int col1 = (the1->x - LUOITHE_X) / (CHIEURONGTHE + KHOANGCACHTHE);
            int row2 = (the2->y - LUOITHE_Y) / (CHIEUDAITHE + KHOANGCACHTHE);
            int col2 = (the2->x - LUOITHE_X) / (CHIEURONGTHE + KHOANGCACHTHE);
            auto pos1 = getGridPosition(row2, col2);
            auto pos2 = getGridPosition(row1, col1);
            swapAnimation.targetX1 = pos1.x;
            swapAnimation.targetY1 = pos1.y;
            swapAnimation.targetX2 = pos2.x;
            swapAnimation.targetY2 = pos2.y;
            swapAnimation.startTime = SDL_GetTicks();
            swapAnimation.active = true;
        }
        the1 = the2 = nullptr;
        waiting = false;
    }
    // Xử lý hiệu ứng đổi chỗ
if (swapAnimation.active) {
    float progress = min(1.0f, (SDL_GetTicks() - swapAnimation.startTime) / (float)SWAP_DURATION);
    if (progress >= 1.0f) {
        swapAnimation.active = false;
        swap(swapAnimation.card1->value, swapAnimation.card2->value);
        swapAnimation.card1->x = swapAnimation.targetX1;
        swapAnimation.card1->y = swapAnimation.targetY1;
        swapAnimation.card2->x = swapAnimation.targetX2;
        swapAnimation.card2->y = swapAnimation.targetY2;
        swap(swapAnimation.card1->value, swapAnimation.card2->value);
        swapAnimation.card1->flippingBack = true;
        swapAnimation.card2->flippingBack = true;
        swapAnimation.card1->hieuung = true;
        swapAnimation.card2->hieuung = true;
        swapAnimation.card1->flipStartTime = SDL_GetTicks();
        swapAnimation.card2->flipStartTime = SDL_GetTicks();
    } else {
        // Tính toán vị trí trung gian
        float t = 0.5f - 0.5f * cos(progress * M_PI); // Làm mượt
        swapAnimation.card1->x = swapAnimation.startX1 + t * (swapAnimation.startX2 - swapAnimation.startX1);
        swapAnimation.card1->y = swapAnimation.startY1 + t * (swapAnimation.startY2 - swapAnimation.startY1);
        swapAnimation.card2->x = swapAnimation.startX2 + t * (swapAnimation.startX1 - swapAnimation.startX2);
        swapAnimation.card2->y = swapAnimation.startY2 + t * (swapAnimation.startY1 - swapAnimation.startY2);
    }
}
    if (timeRemaining <= 0) {
        timeRemaining = 0;
        timeUp = true;
    }
    bool allMatched = true ; 
    for(auto &card : cards){
        if(!card.matched){
            allMatched = false;
            break;
        }
    }
    if(allMatched&& !showLevelComplete &&!showCountdown&&!flippingAllCards){
        flippingAllCards = true ;
        flipAllStartTime = currentTime;
        showLevelComplete = true;
        showLevelCompleteSlide = true;
        slideStartTime = currentTime;
        levelCompleteTime = currentTime;
        shouldResetTime = true;
        TIME_LIMIT = (TIME_LIMIT - 30000 > 30000) ? TIME_LIMIT - 30000 : 30000;
        timeRemaining = TIME_LIMIT-(levelCompleteTime- startTime);

    }
    if(showLevelCompleteSlide){
        float progress = min(1.0f,(currentTime-slideStartTime)/(float)SLIDE_DURATION);
        slideOffset = -SLIDE_HEIGHT +(int)(progress*SLIDE_HEIGHT);
        if(progress >= 1.0f){
            showLevelCompleteSlide = false;
        }
    }
    if(flippingAllCards){
        float progress = min(1.0f,(currentTime - flipAllStartTime)/500.0f);
        if(progress >= 1.0f){
            flippingAllCards = false;
            if(shouldResetTime){
                startTime = SDL_GetTicks();
                shouldResetTime = false;
                timeRemaining = TIME_LIMIT;
            }
            for(auto& card : cards){
                card.flipped = false;
                card.matched = false;
            }
        }
    }
    if (showLevelComplete) {
        if (currentTime - levelCompleteTime > 5000) { // Hiển thị trong 5 giây
            showLevelComplete = false;
            showCountdown = true;
            levelCompleteTime = currentTime;
            currentLevel++;
        }
    }
    if (showCountdown) {
        if (currentTime - levelCompleteTime > 1000) { // Mỗi giây giảm 1
            countdown--;
            levelCompleteTime = currentTime;
            if (countdown <= 0) {
                showCountdown = false;
                countdown = 5;
                timeRemaining = TIME_LIMIT; 
                shuffleCards(imagePaths);
                startTime = SDL_GetTicks();
                timeUp = false;
            }
        }
    }
}
void renderGame() {
    SDL_SetRenderDrawColor(renderer, 255, 192, 203, 255);
    SDL_RenderClear(renderer);
    SDL_Rect bgRect = {0, 0, CHIEURONGMANHINH, CHIEUDAIMANHINH};
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, &bgRect);
    SDL_Rect smallBgRect = {HINHNENNHO_X, HINHNENNHO_Y, CHIEURONGHINHNENNHO, CHIEUDAIHINHNENNHO};
    SDL_RenderCopy(renderer, HINHNENNHO, nullptr, &smallBgRect);
    Uint32 currentTime = SDL_GetTicks();
    for (auto &card : cards) {
        if(swapAnimation.active&&(&card== swapAnimation.card1||&card == swapAnimation.card2)){
            continue;
        }
        SDL_Rect rect = {card.x, card.y, CHIEURONGTHE, CHIEUDAITHE};
        if(flippingAllCards){
            float progress = min(1.0f,(currentTime-flipAllStartTime)/500.0f);
            float flipProgress = progress *2.0f;
            if(flipProgress < 1.0f){
                int width =(int)(CHIEURONGTHE*(1.0f-flipProgress));
                int xOffset= (CHIEURONGTHE-width)/2;
                SDL_Rect flipRect = {card.x + xOffset, card.y, width, CHIEUDAITHE};
                SDL_Texture* texture = flipProgress < 0.5f ? cardTextures[card.value] : backTexture;
                SDL_RenderCopy(renderer, texture, nullptr, &flipRect);
            } else {
                // Giữ trạng thái úp
                SDL_RenderCopy(renderer, backTexture, nullptr, &rect);
            }
        }else{
            if (card.hieuung) {
                float progress = (currentTime - card.flipStartTime) / (float)THOIGIANLATTHE;
                if (progress > 1.0f) progress = 1.0f;
                progress = 0.5 * (1 - cos(progress * M_PI));
                int width = (int)(CHIEURONGTHE * (1.0f - progress));
                int xOffset = (CHIEURONGTHE - width) / 2;
                SDL_Rect flipRect = {card.x + xOffset, card.y, width, CHIEUDAITHE};
                SDL_Texture* texture = (progress < 0.5) ? backTexture : cardTextures[card.value];
                SDL_RenderCopy(renderer, texture, nullptr, &flipRect);
            } 
            else if (card.flipped || card.matched) {
                SDL_RenderCopy(renderer, cardTextures[card.value], nullptr, &rect);
            } 
            else {
                SDL_RenderCopy(renderer, backTexture, nullptr, &rect);
            }
        }
    }
    if (swapAnimation.active) {
        for (auto* card : {swapAnimation.card1, swapAnimation.card2}) {
            if (card) {
                SDL_Rect rect = {card->x, card->y, CHIEURONGTHE, CHIEUDAITHE};
                if (card->flipped || card->matched) {
                    SDL_RenderCopy(renderer, cardTextures[card->value], nullptr, &rect);
                } else {
                    SDL_RenderCopy(renderer, backTexture, nullptr, &rect);
                }
            }
        }
    }
    const int BORDER_WIDTH = 5;
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    SDL_Rect borderRect = {banggame_x - BORDER_WIDTH,banggame_y - BORDER_WIDTH,chieurongbang + 2 * BORDER_WIDTH,chieudaibang + 2 * BORDER_WIDTH};
    SDL_RenderFillRect(renderer, &borderRect);
    SDL_SetRenderDrawColor(renderer, 255, 192, 203, 255);
    SDL_Rect scoreboardRect = {banggame_x, banggame_y, chieurongbang, chieudaibang};
    SDL_RenderFillRect(renderer, &scoreboardRect);
    TTF_Font* font = TTF_OpenFont("BubblegumSans-Regular.ttf", 28);
    if (font) {
        SDL_Color textColor = {255, 127, 80, 255};
        string titleText = "BOARD GAME";
        SDL_Surface* titleSurface = TTF_RenderText_Solid(font, titleText.c_str(), textColor);
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        SDL_Rect titleRect = {banggame_x + (chieurongbang - titleSurface->w)/2,banggame_y + 20, titleSurface->w, titleSurface->h};
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
        string levelText = "Level: " + to_string(currentLevel);
        SDL_Surface* levelSurface = TTF_RenderText_Solid(font, levelText.c_str(), textColor);
        SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);
        SDL_Rect levelRect = {banggame_x + 20, banggame_y + 80, levelSurface->w, levelSurface->h};
        SDL_RenderCopy(renderer, levelTexture, nullptr, &levelRect);
        string scoreText = "Score: " + to_string(diem);
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        SDL_Rect scoreRect = {banggame_x + 20, banggame_y + 130, scoreSurface->w, scoreSurface->h};
        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
        SDL_FreeSurface(titleSurface);
        SDL_FreeSurface(levelSurface);
        SDL_FreeSurface(scoreSurface);
        SDL_DestroyTexture(titleTexture);
        SDL_DestroyTexture(levelTexture);
        SDL_DestroyTexture(scoreTexture);
    }
    float timeRemainingPercent;
    if (showLevelComplete || showCountdown) {
        timeRemainingPercent = (float)timeRemaining / (float)TIME_LIMIT;
    } else {
        Uint32 elapsedTime = currentTime - startTime;
        timeRemainingPercent = (float)(TIME_LIMIT - elapsedTime) / (float)TIME_LIMIT;
    }
    int timerWidth =(int)(chieurongthanhTG*timeRemainingPercent);
    timerWidth = min(timerWidth, chieurongthanhTG);
    const int TIMER_BORDER_WIDTH = 5;
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    SDL_Rect timerBorderRect = {thanhTG_X - TIMER_BORDER_WIDTH,thanhTG_Y - TIMER_BORDER_WIDTH,chieurongthanhTG + 2 * TIMER_BORDER_WIDTH,chieudaithanhTG + 2 * TIMER_BORDER_WIDTH};
    SDL_RenderFillRect(renderer, &timerBorderRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect timerBar = {thanhTG_X, thanhTG_Y, timerWidth, chieudaithanhTG};
    SDL_RenderFillRect(renderer, &timerBar);
    if (showLevelComplete||showLevelCompleteSlide) {
        SDL_SetRenderDrawColor(renderer,255,192,203,255);
        SDL_Rect slideRect={0,slideOffset,CHIEURONGMANHINH,SLIDE_HEIGHT};
        SDL_RenderFillRect(renderer,&slideRect);
        TTF_Font* slideFont = TTF_OpenFont("BubblegumSans-Regular.ttf", 36);
    if (slideFont) {
        string slideText = "Level " + to_string(currentLevel) + " Completed!";
        SDL_Color textColor = {0, 33, 71, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(slideFont, slideText.c_str(), textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = {(CHIEURONGMANHINH - surface->w)/2,slideOffset + (SLIDE_HEIGHT - surface->h)/2,surface->w,surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        TTF_CloseFont(slideFont);
        }
    }
    if (showCountdown) {
        TTF_Font* countFont = TTF_OpenFont("BubblegumSans-Regular.ttf", 90);
        if (countFont) {
            string countText = to_string(countdown);
            SDL_Color countColor = {138, 43, 226, 255};
            SDL_Surface* surface = TTF_RenderText_Solid(countFont, countText.c_str(), countColor);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = {(CHIEURONGMANHINH - surface->w)/2,(CHIEUDAIMANHINH - surface->h)/2,surface->w,surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
            TTF_CloseFont(countFont);
        }
    }
    SDL_RenderPresent(renderer);
}
