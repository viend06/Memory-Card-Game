#include "include/game.h"
#include "include/utils.h"
#include <SDL2/SDL_ttf.h>  // Add this line
int main(int argc, char* argv[]) {
    srand(time(0));
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();  // Initialize SDL_ttf    
    window = SDL_CreateWindow("Memory Card Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CHIEURONGMANHINH, CHIEUDAIMANHINH, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
     ANHNENMENU = loadTexture("assets/image.png"); // Chuẩn bị file ảnh nền
    backgroundTexture = loadTexture("assets/background.jpg");
    backTexture = loadTexture("assets/catback.png");
    HINHNENNHO = loadTexture("assets/small_background.png");
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Không thể khởi tạo SDL_mixer: %s", Mix_GetError());
        return -1;
    }
    // Load các file âm thanh
    nhacnen = Mix_LoadMUS("assets/soundbackground.mp3");
    cardFlipSound = Mix_LoadWAV("assets/cardflip.ogg");
    gameOverSound = Mix_LoadWAV("assets/gameover.wav");
    Mix_VolumeMusic(30);  // Âm lượng nhạc nền (0-128)
    Mix_Volume(-1, 64);   // Âm lượng hiệu ứng (0-128)
    vector<string> imagePaths = {"assets/iamge.png", "assets/iamge1.jpg", "assets/iamge2.jpg", "assets/iamge3.jpg", "assets/iamge4.jpg", "assets/iamge5.jpg", "assets/iamge6.jpg", "assets/iamge7.jpg","assets/iamge8.jpg","assets/iamge9.png"};
    gameOverTexture = loadTexture("assets/game_over.png"); // Chuẩn bị file ảnh game over
    shuffleCards(imagePaths);
    startTime = SDL_GetTicks();
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                // Xử lý click menu
                if (showMenu) {
                    if (x >= startButtonRect.x && x <= startButtonRect.x + startButtonRect.w &&
                        y >= startButtonRect.y && y <= startButtonRect.y + startButtonRect.h) {
                        showMenu = false;
                        startTime = SDL_GetTicks();
                        timeUp = false;
                        shuffleCards(imagePaths);
                        if (nhacnen) {
                            Mix_PlayMusic(nhacnen, -1); // -1 để lặp vô hạn
                            Mix_VolumeMusic(MIX_MAX_VOLUME / 2); // Chỉnh âm lượng
                        }
                    }
                }
                // Xử lý click game (giữ nguyên)
                else if (!timeUp) {
                    handleMouseClick(event.button.x, event.button.y);
                }
            }
        }
        // Phần render (giữ nguyên cấu trúc)
        if (showMenu) {
            renderMenu();
        }
        else {
            updateGame();
            renderGame();
            // Giữ nguyên phần xử lý time up
            if (timeUp && !showMenu) {
                Mix_HaltMusic(); // Dừng nhạc nền khi game over
                Mix_PlayChannel(-1, gameOverSound, 0);
                
                // Vẽ background game over full screen
                SDL_RenderCopy(renderer, gameOverTexture, NULL, NULL);
                
                // Thêm text thông báo
                TTF_Font* font = TTF_OpenFont("BubblegumSans-Regular.ttf", 48);
                if (font) {
                    // Vẽ điểm số
                    string scoreText = "Score: " + to_string(diem);
                    SDL_Color textColor = {255, 255, 255, 255};
                    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_Rect rect = {(CHIEURONGMANHINH - surface->w)/2, 
                                     (CHIEUDAIMANHINH - surface->h)/2 + 100, 
                                     surface->w, surface->h};
                    SDL_RenderCopy(renderer, texture, nullptr, &rect);
                    
                    // Vẽ hướng dẫn
                    string instruction = "Click to return to menu";
                    SDL_Surface* instrSurface = TTF_RenderText_Solid(font, instruction.c_str(), textColor);
                    SDL_Texture* instrTexture = SDL_CreateTextureFromSurface(renderer, instrSurface);
                    SDL_Rect instrRect = {(CHIEURONGMANHINH - instrSurface->w)/2,
                                        (CHIEUDAIMANHINH - instrSurface->h)/2 + 20,
                                        instrSurface->w, instrSurface->h};
                    SDL_RenderCopy(renderer, instrTexture, nullptr, &instrRect);
                    
                    SDL_RenderPresent(renderer);
                    
                    // Chờ click chuột
                    bool waiting = true;
                    while (waiting) {
                        SDL_Event e;
                        while (SDL_PollEvent(&e)) {
                            if (e.type == SDL_QUIT) {
                                waiting = false;
                                running = false;
                            }
                            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                                waiting = false;
                            }
                        }
                        SDL_Delay(16);
                    }
                    
                    // Giải phóng bộ nhớ
                    SDL_FreeSurface(surface);
                    SDL_FreeSurface(instrSurface);
                    SDL_DestroyTexture(texture);
                    SDL_DestroyTexture(instrTexture);
                    TTF_CloseFont(font);
                }
                
                showMenu = true; // Quay lại menu
                // Reset game
                currentLevel = 1;
                diem = 0;
                TIME_LIMIT = 60000;
                timeRemaining = TIME_LIMIT;
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    TTF_Quit();  // Quit SDL_ttf
    SDL_Quit();
    return 0;
}