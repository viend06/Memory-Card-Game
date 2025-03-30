//game.h
#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cmath>
using namespace std;
// Các hằng số
const int CHIEURONGMANHINH = 1100;
const int CHIEUDAIMANHINH = 600;
//menu
extern bool showMenu;// kiểm tra xem có đang hiển thị menu chính không
extern SDL_Texture* ANHNENMENU;// texture của hình nền menu
extern SDL_Texture* anhnutstart; // texture của nút bắt đầu
extern SDL_Rect startButtonRect;// vị trí và kích thước của nút bắt đầu
extern SDL_Window* window; // cửa sổ SDL
extern SDL_Renderer* renderer;// Bộ vẽ SDL
extern SDL_Texture* backgroundTexture;// texture nền game
extern SDL_Texture*HINHNENNHO;
const int HINHNENNHO_X= -20;
const int HINHNENNHO_Y= -85;
const int CHIEURONGHINHNENNHO = 750;
const int CHIEUDAIHINHNENNHO = 800;
extern int chieurongluoithe;
extern int chieudailuoithe;
extern int LUOITHE_X;
extern int LUOITHE_Y;
extern vector<SDL_Texture*> cardTextures; // danh sách texture của các thẻ
extern SDL_Texture* backTexture;// texture mặt sau của thẻ
extern int  hangthe; 
extern int cotthe;
const int CHIEURONGTHE = 90;
const int CHIEUDAITHE= 90;
const int KHOANGCACHTHE = 5 ; 
const int THOIGIANLATTHE= 210; // thoi gian hien thi the khi lat
const int banggame_x= 745; // Vị trí x của bảng điểm
const int banggame_y= 200;// Vị trí bảng điểm theo trục Y
const int chieurongbang= 300;// Chiều rộng bảng điểm
const int chieudaibang= 200;// Chiều cao bảng điểm
const int thanhTG_X = 720;
const int thanhTG_Y= 530;
const int chieurongthanhTG = 350; 
const int chieudaithanhTG = 20;
extern Uint32 TIME_LIMIT;// giới hạn thời gian của mỗi màn chơichơi
extern int currentLevel;// cấp độ hiện tại của người chơi
extern Uint32 timeRemaining;// thời gian còn lại của màn chơi
extern Uint32 startTime;// thời điểm bắt đầu màn chơi
extern bool timeUp;// kiểm tra xem thời gian đã hết chưa
extern int diem;
extern Uint32 levelCompleteTime;
extern bool showLevelCompleteSlide; 
extern Uint32 slideStartTime;
extern int slideOffset;
const int SLIDE_DURATION=300;
const int SLIDE_HEIGHT = 80;
extern bool flippingAllCards;
extern Uint32 flipAllStartTime;
//cấu trúc thẻ 
struct Card {
    int x, y;// vị trí thẻ 
    int value;// giá trị thẻ (để so khớp)
    bool flipped;// trạng thái lật
    bool matched;// đã khớp
    bool hieuung; // đang có hiệu ứng 
    bool flippingBack; // đang lập ngược lại 
    Uint32 flipStartTime;// thời gian bắt đầu lật
};
// Biến toàn cục
extern vector<Card> cards;// danh sách các thẻ
extern Card* the1; // thẻ thứ nhất đang chọn
extern Card* the2;// thẻ thứ hai đang chọn
extern bool waiting;// trạng thái chờ
extern Uint32 lastFlipTime;// thời gian lật thẻ gần nhất
// Thêm vào game.h
extern bool showLevelComplete;// Biến kiểm tra xem có hiển thị thông báo hoàn thành level không 
extern Uint32 levelCompleteTime;// thời gian hoàn thành màn chơi
extern int countdown;// thời gian đếm ngược trước màn chơi mới 
extern bool showCountdown; // kiểm tra xem có hiển thị đếm ngược không 
extern SDL_Texture* gameOverTexture; // Thêm dòng này
// Thêm các biến âm thanh
extern Mix_Music* nhacnen; 
extern Mix_Chunk* cardFlipSound;
extern Mix_Chunk* gameOverSound;
// Khai báo hàm
void shuffleCards(const vector<string>& imagePaths);
void handleMouseClick(int x, int y);
void updateGame();
void renderGame();
void renderMenu();

#endif // GAME_H