#include "AISetupState.h"
#include "Game.h"
#include "AISolvingState.h" 
#include "AIDrawMapState.h" 
#include "MainMenuState.h"
#include <iostream> 
#include <cmath> 

AISetupState::AISetupState(Game* game)
    : GameState(game),
    mAiGridDim(10),
    mAiNumColors(6),
    mShowCustomInput(false)
{
    mUI.init(mGame->getResources());
    initButtons();
    initNetwork(); // <--- Khởi tạo hiệu ứng
}

// === 1. KHỞI TẠO CÁC ĐIỂM ===
void AISetupState::initNetwork() {
    for (int i = 0; i < 40; ++i) { // 40 điểm
        Node n;
        n.x = (float)(rand() % SCREEN_WIDTH);
        n.y = (float)(rand() % SCREEN_HEIGHT);
        // Tốc độ ngẫu nhiên chậm
        n.vx = ((rand() % 100) - 50) / 50.0f; // -1.0 đến 1.0
        n.vy = ((rand() % 100) - 50) / 50.0f;
        mNodes.push_back(n);
    }
}

// === 2. CẬP NHẬT VỊ TRÍ ===
void AISetupState::updateNetwork() {
    for (auto& n : mNodes) {
        n.x += n.vx;
        n.y += n.vy;

        // Nảy lại khi chạm cạnh màn hình
        if (n.x < 0 || n.x > SCREEN_WIDTH) n.vx *= -1;
        if (n.y < 0 || n.y > SCREEN_HEIGHT) n.vy *= -1;
    }
}

// === 3. VẼ MẠNG LƯỚI ===
void AISetupState::drawNetwork(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Vẽ các đường nối trước
    for (size_t i = 0; i < mNodes.size(); ++i) {
        for (size_t j = i + 1; j < mNodes.size(); ++j) {
            // Tính khoảng cách giữa 2 điểm
            float dx = mNodes[i].x - mNodes[j].x;
            float dy = mNodes[i].y - mNodes[j].y;
            float distSq = dx * dx + dy * dy;

            // Nếu gần nhau (< 150px) thì vẽ dây
            if (distSq < 150 * 150) {
                // Độ mờ dựa trên khoảng cách (càng gần càng rõ)
                int alpha = (int)(255 - (distSq * 255 / (150 * 150)));
                if (alpha < 0) alpha = 0;

                SDL_SetRenderDrawColor(renderer, 100, 255, 100, alpha / 2); // Màu xanh lá nhạt
                SDL_RenderDrawLine(renderer, (int)mNodes[i].x, (int)mNodes[i].y, (int)mNodes[j].x, (int)mNodes[j].y);
            }
        }
    }

    // Vẽ các điểm tròn (vuông nhỏ)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200); // Màu trắng
    for (const auto& n : mNodes) {
        SDL_Rect r = { (int)n.x - 2, (int)n.y - 2, 4, 4 };
        SDL_RenderFillRect(renderer, &r);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void AISetupState::initButtons() {
    auto res = mGame->getResources();

    int sBtnW = 100, sBtnH = 50, sPad = 20;
    int gridSizes[] = { 10, 14, 20, 30, 40 };
    int numAiButtons = 5;
    int totalW = (int)numAiButtons * sBtnW + (int)(numAiButtons - 1) * sPad;
    int startX = (SCREEN_WIDTH - totalW) / 2;

    for (int i = 0; i < 5; ++i) {
        std::string key = "size_" + std::to_string(gridSizes[i]);
        std::string text = std::to_string(gridSizes[i]);
        mUI.addButton(key, { startX + i * (sBtnW + sPad), 250, sBtnW, sBtnH }, text, PALETTE[i], BUTTON_HIGHLIGHT, TEXT_WHITE);
    }

    mUI.addButton("custom", { startX, 320, sBtnW, sBtnH }, "ai_setup_custom", BUTTON_GRAY, BUTTON_HIGHLIGHT);
    mUI.addButton("draw", { startX + sBtnW + sPad, 320, sBtnW * 2 + sPad, sBtnH }, "ai_setup_draw_map", BUTTON_BLUE, BUTTON_HIGHLIGHT);

    mUI.addButton("start_random", { (SCREEN_WIDTH - 250) / 2, 500, 250, 60 }, "ai_setup_start", BUTTON_GREEN, BUTTON_HIGHLIGHT);
    mUI.addButton("back", { (SCREEN_WIDTH - 200) / 2, 600, 200, 60 }, "ai_setup_back", BUTTON_GRAY, BUTTON_HIGHLIGHT);

    mUI.updateButtonTexts();
}

void AISetupState::handleEvents(SDL_Event& e) {
    if (mShowCustomInput) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_BACKSPACE && !mInputText.empty()) {
                mInputText.pop_back();
            }
            else if (e.key.keysym.sym == SDLK_RETURN && !mInputText.empty()) {
                confirmCustomInput();
            }
            else if (e.key.keysym.sym == SDLK_ESCAPE) {
                mShowCustomInput = false;
                SDL_StopTextInput();
            }
        }
        else if (e.type == SDL_TEXTINPUT) {
            if (isdigit(e.text.text[0]) && mInputText.length() < 3) {
                mInputText += e.text.text;
            }
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Rect box = { SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 100, 500, 200 };
            SDL_Point mousePoint = { e.button.x, e.button.y };
            if (!SDL_PointInRect(&mousePoint, &box)) {
                mShowCustomInput = false;
                SDL_StopTextInput();
            }
        }
        return;
    }

    std::string action = mUI.handleEvents(e);

    if (action == "back") {
        mGame->changeState(new MainMenuState(mGame));
    }
    else if (action == "start_random") {
        mGame->changeState(new AISolvingState(mGame, mAiGridDim, mGame->getNumColors(), false, {}));
    }
    else if (action == "draw") {
        mGame->pushState(new AIDrawMapState(mGame, mAiGridDim, mGame->getNumColors()));
    }
    else if (action == "custom") {
        mShowCustomInput = true;
        mInputText = "";
        SDL_StartTextInput();
    }
    else if (action == "size_10") mAiGridDim = 10;
    else if (action == "size_14") mAiGridDim = 14;
    else if (action == "size_20") mAiGridDim = 20;
    else if (action == "size_30") mAiGridDim = 30;
    else if (action == "size_40") mAiGridDim = 40;
}

void AISetupState::confirmCustomInput() {
    try {
        int val = std::stoi(mInputText);
        if (val >= 10 && val <= 100) {
            mAiGridDim = val;
            mShowCustomInput = false;
            SDL_StopTextInput();
        }
        else mInputText = "";
    }
    catch (...) {
        mInputText = "";
    }
}

void AISetupState::update() {
    updateNetwork(); // <--- Cập nhật hiệu ứng
}

void AISetupState::draw(SDL_Renderer* renderer) {
    auto res = mGame->getResources();
    auto fontUI = res->getFontUI();
    auto fontTitle = res->getFontTitle();
    auto fontSmall = res->getFontSmall();

    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, 255);
    SDL_RenderClear(renderer);

    // === VẼ NỀN TRANG TRÍ ===
    drawNetwork(renderer);

    drawText(renderer, res->getText("ai_setup_title"), SCREEN_WIDTH / 2, 80, fontTitle, PALETTE[2]);
    drawText(renderer, res->getText("ai_setup_grid_size"), SCREEN_WIDTH / 2, 200, fontUI, TEXT_WHITE);

    mUI.getButton("size_10")->draw(renderer, fontUI, mAiGridDim == 10);
    mUI.getButton("size_14")->draw(renderer, fontUI, mAiGridDim == 14);
    mUI.getButton("size_20")->draw(renderer, fontUI, mAiGridDim == 20);
    mUI.getButton("size_30")->draw(renderer, fontUI, mAiGridDim == 30);
    mUI.getButton("size_40")->draw(renderer, fontUI, mAiGridDim == 40);

    mUI.getButton("custom")->draw(renderer, fontSmall, mShowCustomInput);
    mUI.getButton("draw")->draw(renderer, fontUI, false);
    mUI.getButton("start_random")->draw(renderer, fontSmall, false);
    mUI.getButton("back")->draw(renderer, fontUI, false);

    if (mShowCustomInput) {
        drawCustomInputOverlay(renderer);
    }
}

void AISetupState::drawCustomInputOverlay(SDL_Renderer* renderer) {
    auto res = mGame->getResources();

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlay = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &overlay);

    SDL_Rect box = { SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 100, 500, 200 };
    SDL_SetRenderDrawColor(renderer, UI_BG_COLOR.r, UI_BG_COLOR.g, UI_BG_COLOR.b, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &box);

    drawText(renderer, res->getText("ai_custom_prompt"), box.x + box.w / 2, box.y + 40, res->getFontUI(), TEXT_WHITE);

    SDL_Rect inputBox = { box.x + 50, box.y + 90, box.w - 100, 50 };
    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, 255);
    SDL_RenderFillRect(renderer, &inputBox);

    std::string displayText = mInputText.empty() ? " " : mInputText;
    if (SDL_GetTicks() % 1000 < 500) displayText += "_";

    drawText(renderer, displayText, inputBox.x + inputBox.w / 2, inputBox.y + inputBox.h / 2, res->getFontUI(), TEXT_WHITE);
    drawText(renderer, res->getText("ai_custom_confirm"), box.x + box.w / 2, box.y + box.h - 25, res->getFontSmall(), { 200, 200, 200, 255 });
}