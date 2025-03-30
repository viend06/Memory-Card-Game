//utils.cpp
#include "include/utils.h"
#include "include/game.h"
SDL_Texture* loadTexture(const string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) cout << "Lỗi tải ảnh: " << path << endl;
    return texture;
}
void renderTextWithOutline(SDL_Renderer* renderer, TTF_Font* font, 
    const char* text, SDL_Color textColor,
    SDL_Color outlineColor, int outlineSize,
    int x, int y) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Surface* outlineSurface = TTF_RenderText_Solid(font, text, outlineColor);
    SDL_Texture* outlineTexture = SDL_CreateTextureFromSurface(renderer, outlineSurface);
    for (int dy = -outlineSize; dy <= outlineSize; dy++) {
        for (int dx = -outlineSize; dx <= outlineSize; dx++) {
            if (dx != 0 || dy != 0) { // Bỏ qua vị trí gốc
                SDL_Rect outlineRect = {x + dx, y + dy, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, outlineTexture, NULL, &outlineRect);
            }
        }
    }
    // Vẽ chữ chính
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(outlineSurface);
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(outlineTexture);
}