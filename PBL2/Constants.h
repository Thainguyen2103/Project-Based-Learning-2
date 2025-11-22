#pragma once

#include <SDL.h>
#include <SDL_ttf.h> 
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iomanip>
#include <algorithm> 
#include <limits>    

// =================================================================
// CÀI ĐẶT TOÀN CỤC
// =================================================================

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int MAP_AREA_SIZE = 680;
const int UI_PANEL_WIDTH = 300;

enum AppState {
    STATE_MAIN_MENU, STATE_SETTINGS, STATE_PLAYING, STATE_ANIMATING_FILL, STATE_GAME_OVER,
    STATE_AI_SETUP, STATE_AI_SOLVING, STATE_AI_CUSTOM_INPUT,
    STATE_AI_DRAW_MAP, STATE_INSTRUCTION, STATE_CREDITS
};

enum AlgorithmType { ALGO_BFS, ALGO_DFS, ALGO_SCANLINE, ALGO_UNION_FIND };
enum FillDirection { DIR_FOUR, DIR_EIGHT };
enum Language { LANG_VI, LANG_EN, LANG_CN, LANG_JP };
enum GameDifficulty { DIFF_EASY, DIFF_NORMAL, DIFF_HARD };

// === ENUM CHO HÌNH MINH HỌA ===
enum IllustrationType {
    ILLUST_GOAL,        // Trang 1: Mục tiêu
    ILLUST_START,       // Trang 2: Điểm bắt đầu
    ILLUST_MECHANICS,   // Trang 3: Cách chơi
    ILLUST_STRATEGY,    // Trang 4: Chiến thuật
    ILLUST_MOVES,       // Trang 5: Số lượt đi
    ILLUST_FEATURES,    // Trang 6: Tính năng
    ILLUST_EVENT,       // Trang 7: Sự kiện
    ILLUST_AI           // Trang 8: AI
};

const int STEPS_PER_FRAME = 10;

struct PredictionResult {
    bool possibleToWin = true;
    int remainingColors = 0;
    int predictedSteps = 0;
};

// =================================================================
// BẢNG MÀU VÀ MÀU GIAO DIỆN
// =================================================================

const std::vector<SDL_Color> PALETTE = {
    {231, 76,  60,  255}, {46,  204, 113, 255}, {52,  152, 219, 255},
    {241, 196, 15,  255}, {155, 89,  182, 255}, {26,  188, 156, 255},
    {230, 126, 34,  255}, {211, 84,  0,   255}
};
const SDL_Color BG_COLOR = { 23, 32, 42, 255 };
const SDL_Color UI_BG_COLOR = { 44, 62, 80, 255 };
const SDL_Color BUTTON_GREEN = { 46, 204, 113, 255 };
const SDL_Color BUTTON_BLUE = { 52, 152, 219, 255 };
const SDL_Color BUTTON_RED = { 231, 76, 60, 255 };
const SDL_Color BUTTON_YELLOW = { 241, 196, 15, 255 };
const SDL_Color BUTTON_GRAY = { 128, 128, 128, 255 };
const SDL_Color BUTTON_HIGHLIGHT = { 26, 188, 156, 255 };
const SDL_Color BUTTON_WHITE = { 236, 240, 241, 255 };
const SDL_Color TEXT_BLACK = { 44, 62, 80, 255 };
const SDL_Color TEXT_WHITE = { 255, 255, 255, 255 };

// =================================================================
// CÁC HÀM TIỆN ÍCH
// =================================================================

struct ColorComparator {
    bool operator()(const SDL_Color& a, const SDL_Color& b) const {
        if (a.r != b.r) return a.r < b.r;
        if (a.g != b.g) return a.g < b.g;
        return a.b < b.b;
    }
};

inline bool areColorsEqual(const SDL_Color& c1, const SDL_Color& c2) {
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

inline void drawText(SDL_Renderer* renderer, const std::string& text, int cX, int cY, TTF_Font* f, SDL_Color c) {
    if (!f || text.empty() || !renderer) return;
    SDL_Surface* s = TTF_RenderUTF8_Blended(f, text.c_str(), c); if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s); if (!t) { SDL_FreeSurface(s); return; }
    SDL_Rect dR = { cX - s->w / 2, cY - s->h / 2, s->w, s->h };
    SDL_RenderCopy(renderer, t, NULL, &dR);
    SDL_FreeSurface(s); SDL_DestroyTexture(t);
}

inline void drawTextLeft(SDL_Renderer* renderer, const std::string& text, int cX, int cY, TTF_Font* f, SDL_Color c) {
    if (!f || text.empty() || !renderer) return;
    SDL_Surface* s = TTF_RenderUTF8_Blended(f, text.c_str(), c); if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s); if (!t) { SDL_FreeSurface(s); return; }
    SDL_Rect dR = { cX, cY - s->h / 2, s->w, s->h };
    SDL_RenderCopy(renderer, t, NULL, &dR);
    SDL_FreeSurface(s); SDL_DestroyTexture(t);
}

inline void drawTextRight(SDL_Renderer* renderer, const std::string& text, int cX, int cY, TTF_Font* f, SDL_Color c) {
    if (!f || text.empty() || !renderer) return;
    SDL_Surface* s = TTF_RenderUTF8_Blended(f, text.c_str(), c); if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s); if (!t) { SDL_FreeSurface(s); return; }
    SDL_Rect dR = { cX - s->w, cY - s->h / 2, s->w, s->h };
    SDL_RenderCopy(renderer, t, NULL, &dR);
    SDL_FreeSurface(s); SDL_DestroyTexture(t);
}