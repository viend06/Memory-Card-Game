//utils.h
#ifndef UTILS_H
#define UTILS_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <SDL2/SDL_ttf.h>
using namespace std;
// Khai báo hàm
SDL_Texture* loadTexture(const string& path);
void renderTextWithOutline(SDL_Renderer* renderer, TTF_Font* font, 
    const char* text, SDL_Color textColor, 
    SDL_Color outlineColor, int outlineSize,
    int x, int y);
#endif // UTILS_H