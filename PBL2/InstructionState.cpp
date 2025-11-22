#include "InstructionState.h"
#include "Game.h"
#include <iostream>
#include <cmath> 

InstructionState::InstructionState(Game* game)
    : GameState(game), mCurrentPageIndex(0)
{
    mUI.init(mGame->getResources());
    initPages();
    initButtons();
}

InstructionState::~InstructionState() {}

void InstructionState::initPages() {
    // (Giữ nguyên danh sách 8 trang như cũ)
    mPages.push_back({ "tut_p1_title", { "tut_p1_1", "tut_p1_2", "tut_p1_3" }, ILLUST_GOAL });
    mPages.push_back({ "tut_p2_title", { "tut_p2_1", "tut_p2_2", "tut_p2_3" }, ILLUST_START });
    mPages.push_back({ "tut_p3_title", { "tut_p3_1", "tut_p3_2", "tut_p3_3" }, ILLUST_MECHANICS });
    mPages.push_back({ "tut_p4_title", { "tut_p4_1", "tut_p4_2", "tut_p4_3" }, ILLUST_STRATEGY });
    mPages.push_back({ "tut_p5_title", { "tut_p5_1", "tut_p5_2", "tut_p5_3" }, ILLUST_MOVES });
    mPages.push_back({ "tut_p6_title", { "tut_p6_1", "tut_p6_2", "tut_p6_3" }, ILLUST_FEATURES });
    mPages.push_back({ "tut_p7_title", { "tut_p7_1", "tut_p7_2", "tut_p7_3" }, ILLUST_EVENT });
    mPages.push_back({ "tut_p8_title", { "tut_p8_1", "tut_p8_2", "tut_p8_3" }, ILLUST_AI });
}

void InstructionState::initButtons() {
    int btnY = 640;
    mUI.addButton("back", { SCREEN_WIDTH / 2 - 80, btnY, 160, 50 }, "settings_back", BUTTON_GRAY, BUTTON_HIGHLIGHT);

    // Nút Prev/Next làm to hơn chút cho dễ bấm
    mUI.addButton("prev", { 40, 300, 60, 100 }, "<", BUTTON_BLUE, BUTTON_HIGHLIGHT);
    mUI.addButton("next", { SCREEN_WIDTH - 100, 300, 60, 100 }, ">", BUTTON_BLUE, BUTTON_HIGHLIGHT);
    mUI.updateButtonTexts();
}

void InstructionState::handleEvents(SDL_Event& e) {
    std::string action = mUI.handleEvents(e);
    if (action == "back") mGame->popState();
    else if (action == "prev") { if (mCurrentPageIndex > 0) mCurrentPageIndex--; }
    else if (action == "next") { if (mCurrentPageIndex < mPages.size() - 1) mCurrentPageIndex++; }
}

void InstructionState::update() {}

// === VẼ THANH CHỈ DẪN TRANG (CÁC CHẤM TRÒN) ===
void InstructionState::drawPageIndicator(SDL_Renderer* renderer) {
    int numPages = (int)mPages.size();
    int dotSize = 12;
    int gap = 10;
    int totalW = numPages * dotSize + (numPages - 1) * gap;
    int startX = (SCREEN_WIDTH - totalW) / 2;
    int y = 580; // Vị trí Y của các chấm

    for (int i = 0; i < numPages; ++i) {
        SDL_Rect dot = { startX + i * (dotSize + gap), y, dotSize, dotSize };

        if (i == mCurrentPageIndex) {
            // Trang hiện tại: Chấm màu sáng, to hơn chút
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
        else {
            // Trang khác: Chấm màu tối
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        }
        SDL_RenderFillRect(renderer, &dot);
    }
}

void InstructionState::draw(SDL_Renderer* renderer) {
    auto res = mGame->getResources();

    // 1. Nền tối
    SDL_SetRenderDrawColor(renderer, 30, 30, 35, 255);
    SDL_RenderClear(renderer);

    const auto& currentPage = mPages[mCurrentPageIndex];

    // 2. VẼ THẺ BÀI (CARD BACKGROUND)
    int cardW = 800;
    int cardH = 500;
    int cardX = (SCREEN_WIDTH - cardW) / 2;
    int cardY = 60;

    // Bóng đổ của thẻ
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_Rect shadow = { cardX + 10, cardY + 10, cardW, cardH };
    SDL_RenderFillRect(renderer, &shadow);

    // Thân thẻ (Màu xám đậm hơn nền chút)
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_Rect card = { cardX, cardY, cardW, cardH };
    SDL_RenderFillRect(renderer, &card);

    // Viền thẻ
    SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
    SDL_RenderDrawRect(renderer, &card);

    // 3. Tiêu đề
    drawText(renderer, res->getText(currentPage.titleKey), SCREEN_WIDTH / 2, cardY + 40, res->getFontTitle(), PALETTE[2]);

    // 4. Khung Minh họa
    int illustW = 400;
    int illustH = 250;
    int illustX = (SCREEN_WIDTH - illustW) / 2;
    int illustY = cardY + 80;

    // Nền đen cho minh họa
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_Rect bg = { illustX, illustY, illustW, illustH };
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &bg);

    // Gọi hàm vẽ tương ứng (Giữ nguyên code cũ)
    switch (currentPage.illustType) {
    case ILLUST_GOAL: drawIllustGoal(renderer, illustX, illustY, illustW, illustH); break;
    case ILLUST_START: drawIllustStart(renderer, illustX, illustY, illustW, illustH); break;
    case ILLUST_MECHANICS: drawIllustMechanics(renderer, illustX, illustY, illustW, illustH); break;
    case ILLUST_STRATEGY: drawIllustStrategy(renderer, illustX, illustY, illustW, illustH); break;
    case ILLUST_MOVES: drawIllustMoves(renderer, illustX, illustY, illustW, illustH); break;
    case ILLUST_FEATURES: drawIllustFeatures(renderer, illustX, illustY, illustW, illustH); break;
    case ILLUST_EVENT: drawIllustEvent(renderer, illustX, illustY, illustW, illustH); break;
    case ILLUST_AI: drawIllustAI(renderer, illustX, illustY, illustW, illustH); break;
    }

    // 5. Text nội dung
    int textY = cardY + 360;
    for (const auto& lineKey : currentPage.contentKeys) {
        drawText(renderer, res->getText(lineKey), SCREEN_WIDTH / 2, textY, res->getFontUI(), TEXT_WHITE);
        textY += 35;
    }

    // 6. Vẽ chỉ dẫn trang
    drawPageIndicator(renderer);

    // 7. Vẽ Nút
    mUI.getButton("back")->draw(renderer, res->getFontUI());
    if (mCurrentPageIndex > 0) mUI.getButton("prev")->draw(renderer, res->getFontTitle());
    if (mCurrentPageIndex < mPages.size() - 1) mUI.getButton("next")->draw(renderer, res->getFontTitle());
}

// === CÁC HÀM VẼ MINH HỌA (COPY LẠI TỪ CODE CŨ CỦA BẠN VÀO ĐÂY) ===
// ... (Hãy dán lại 8 hàm drawIllust... từ file cũ vào đây để file hoàn chỉnh) ...
// (Để ngắn gọn, tôi không paste lại 8 hàm đó vì chúng không thay đổi)

// 1. Goal
void InstructionState::drawIllustGoal(SDL_Renderer* r, int x, int y, int w, int h) {
    int boxSize = 100;
    int startX = x + (w - 250) / 2;
    int startY = y + (h - boxSize) / 2;
    for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) {
        SDL_SetRenderDrawColor(r, PALETTE[(i + j) % 6].r, PALETTE[(i + j) % 6].g, PALETTE[(i + j) % 6].b, 255);
        SDL_Rect cell = { startX + j * 33, startY + i * 33, 33, 33 };
        SDL_RenderFillRect(r, &cell);
    }
    drawText(r, "->", startX + 125, startY + 30, mGame->getResources()->getFontTitle(), TEXT_WHITE);
    SDL_SetRenderDrawColor(r, PALETTE[0].r, PALETTE[0].g, PALETTE[0].b, 255);
    SDL_Rect right = { startX + 150, startY, 100, 100 };
    SDL_RenderFillRect(r, &right);
}

void InstructionState::drawIllustStart(SDL_Renderer* r, int x, int y, int w, int h) {
    int s = 40;
    int sx = x + (w - s * 4) / 2;
    int sy = y + (h - s * 4) / 2;
    for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) {
        SDL_SetRenderDrawColor(r, PALETTE[(i * j + 2) % 6].r, PALETTE[(i * j + 2) % 6].g, PALETTE[(i * j + 2) % 6].b, 255);
        SDL_Rect cell = { sx + j * s, sy + i * s, s - 1, s - 1 };
        SDL_RenderFillRect(r, &cell);
    }
    SDL_Rect start = { sx - 2, sy - 2, s + 4, s + 4 };
    SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
    SDL_RenderDrawRect(r, &start);
    drawText(r, mGame->getResources()->getText("illust_start"), sx + s * 2, sy - 30, mGame->getResources()->getFontSmall(), { 255, 255, 0, 255 });
}

void InstructionState::drawIllustMechanics(SDL_Renderer* r, int x, int y, int w, int h) {
    int s = 40;
    int sx = x + (w - s * 4) / 2;
    int sy = y + (h - s * 4) / 2;
    for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) {
        if (i < 2 && j < 2) SDL_SetRenderDrawColor(r, PALETTE[0].r, PALETTE[0].g, PALETTE[0].b, 255);
        else SDL_SetRenderDrawColor(r, PALETTE[2].r, PALETTE[2].g, PALETTE[2].b, 255);
        SDL_Rect cell = { sx + j * s, sy + i * s, s - 1, s - 1 };
        SDL_RenderFillRect(r, &cell);
    }
    drawText(r, mGame->getResources()->getText("illust_click"), sx + s * 2, sy + s * 4 + 20, mGame->getResources()->getFontSmall(), TEXT_WHITE);
}

void InstructionState::drawIllustStrategy(SDL_Renderer* r, int x, int y, int w, int h) {
    int s = 50;
    int cx = x + w / 2;
    int cy = y + h / 2;
    SDL_Rect center = { cx - s / 2, cy - s / 2, s, s };
    SDL_SetRenderDrawColor(r, PALETTE[0].r, PALETTE[0].g, PALETTE[0].b, 255);
    SDL_RenderFillRect(r, &center);
    SDL_SetRenderDrawColor(r, PALETTE[1].r, PALETTE[1].g, PALETTE[1].b, 255);
    SDL_Rect r1 = { cx + s / 2, cy - s / 2, s, s }; SDL_RenderFillRect(r, &r1);
    SDL_Rect r2 = { cx - s / 2, cy - s * 1.5, s, s }; SDL_RenderFillRect(r, &r2);
    SDL_Rect r3 = { cx - s * 1.5, cy - s / 2, s, s }; SDL_RenderFillRect(r, &r3);
    SDL_SetRenderDrawColor(r, PALETTE[3].r, PALETTE[3].g, PALETTE[3].b, 255);
    SDL_Rect r4 = { cx - s / 2, cy + s / 2, s, s }; SDL_RenderFillRect(r, &r4);
    drawText(r, mGame->getResources()->getText("illust_good"), cx, cy - s * 2, mGame->getResources()->getFontSmall(), PALETTE[1]);
}

void InstructionState::drawIllustMoves(SDL_Renderer* r, int x, int y, int w, int h) {
    std::string moveText = mGame->getResources()->getText("game_moves") + ": 5 / 20";
    drawText(r, moveText, x + w / 2, y + h / 2, mGame->getResources()->getFontTitle(), TEXT_WHITE);
    SDL_Rect barBg = { x + 50, y + h / 2 + 40, w - 100, 20 };
    SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
    SDL_RenderFillRect(r, &barBg);
    SDL_Rect barFg = { x + 50, y + h / 2 + 40, (w - 100) * 5 / 20, 20 };
    SDL_SetRenderDrawColor(r, PALETTE[0].r, PALETTE[0].g, PALETTE[0].b, 255);
    SDL_RenderFillRect(r, &barFg);
}

void InstructionState::drawIllustFeatures(SDL_Renderer* r, int x, int y, int w, int h) {
    int btnW = 100, btnH = 40;
    int startX = x + (w - btnW) / 2;
    int startY = y + 60;
    SDL_Rect undo = { startX, startY, btnW, btnH };
    SDL_SetRenderDrawColor(r, BUTTON_BLUE.r, BUTTON_BLUE.g, BUTTON_BLUE.b, 255);
    SDL_RenderFillRect(r, &undo);
    drawText(r, mGame->getResources()->getText("game_undo"), startX + btnW / 2, startY + btnH / 2, mGame->getResources()->getFontSmall(), TEXT_BLACK);
    startY += 60;
    SDL_Rect hint = { startX, startY, btnW, btnH };
    SDL_SetRenderDrawColor(r, BUTTON_YELLOW.r, BUTTON_YELLOW.g, BUTTON_YELLOW.b, 255);
    SDL_RenderFillRect(r, &hint);
    drawText(r, mGame->getResources()->getText("game_hint"), startX + btnW / 2, startY + btnH / 2, mGame->getResources()->getFontSmall(), TEXT_BLACK);
}

void InstructionState::drawIllustEvent(SDL_Renderer* r, int x, int y, int w, int h) {
    int centerX = x + w / 2;
    int centerY = y + h / 2;
    bool blink = (SDL_GetTicks() / 500) % 2 == 0;
    SDL_Color c = blink ? PALETTE[0] : PALETTE[3];
    SDL_Rect warnBox = { centerX - 40, centerY - 40, 80, 80 };
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, 255);
    SDL_RenderFillRect(r, &warnBox);
    drawText(r, "!", centerX, centerY, mGame->getResources()->getFontTitle(), TEXT_WHITE);
}

void InstructionState::drawIllustAI(SDL_Renderer* r, int x, int y, int w, int h) {
    int miniW = 80, miniH = 60, gap = 20;
    int startX = x + (w - (miniW * 2 + gap)) / 2;
    int startY = y + (h - (miniH * 2 + gap)) / 2;
    SDL_Rect rects[4] = {
        { startX, startY, miniW, miniH }, { startX + miniW + gap, startY, miniW, miniH },
        { startX, startY + miniH + gap, miniW, miniH }, { startX + miniW + gap, startY + miniH + gap, miniW, miniH }
    };
    std::string names[4] = { "BFS", "DFS", "Scan", "DSU" };
    for (int i = 0; i < 4; ++i) {
        SDL_SetRenderDrawColor(r, PALETTE[i + 1].r, PALETTE[i + 1].g, PALETTE[i + 1].b, 255);
        SDL_RenderFillRect(r, &rects[i]);
        drawText(r, names[i], rects[i].x + miniW / 2, rects[i].y + miniH / 2, mGame->getResources()->getFontSmall(), TEXT_BLACK);
    }
}