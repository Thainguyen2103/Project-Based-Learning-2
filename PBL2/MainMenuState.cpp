#include "MainMenuState.h"
#include "SettingsState.h"
#include "AISetupState.h" 
#include "PlayingState.h" 
#include "Game.h"
#include "InstructionState.h" 
#include "CreditsState.h"     
#include <fstream>
#include <iostream>
#include <limits> 
#include <SDL_mixer.h> 
#include <cstdlib> // Cho rand()

MainMenuState::MainMenuState(Game* game) : GameState(game) {
    mUI.init(mGame->getResources());

    initButtons();
    initParticles(); // Khởi tạo nền
    mGame->checkForSaveFile();

    Mix_VolumeMusic(MIX_MAX_VOLUME);

    // Khởi tạo biến Glitch
    mGlitchTimer = 0;
    mIsGlitching = false;
    mGlitchOffsetX = 0;
    mGlitchOffsetY = 0;
}

// === 1. KHỞI TẠO NÚT ===
void MainMenuState::initButtons() {
    int btnW = 280, btnH = 60, pad = 20, startX = (SCREEN_WIDTH - btnW) / 2;
    int total_menu_height = 5 * btnH + 4 * pad;
    int startY = 140 + (SCREEN_HEIGHT - 140 - total_menu_height) / 2;

    mUI.addButton("main_continue", { startX, startY, btnW, btnH }, "main_continue", BUTTON_GREEN, BUTTON_HIGHLIGHT);
    mUI.addButton("main_new_game", { startX, startY + (btnH + pad), btnW, btnH }, "main_new_game", BUTTON_BLUE, BUTTON_HIGHLIGHT);
    mUI.addButton("main_ai_solver", { startX, startY + 2 * (btnH + pad), btnW, btnH }, "main_ai_solver", BUTTON_YELLOW, BUTTON_HIGHLIGHT);
    mUI.addButton("main_settings", { startX, startY + 3 * (btnH + pad), btnW, btnH }, "main_settings", PALETTE[5], BUTTON_HIGHLIGHT);
    mUI.addButton("main_exit", { startX, startY + 4 * (btnH + pad), btnW, btnH }, "main_exit", BUTTON_RED, BUTTON_HIGHLIGHT);

    int smallBtnW = 150, smallBtnH = 50, smallPad = 10;
    int smallStartX = SCREEN_WIDTH - smallBtnW - smallPad;

    mUI.addButton("instructions", { smallStartX, smallPad, smallBtnW, smallBtnH }, "main_instructions", BUTTON_BLUE, BUTTON_HIGHLIGHT);
    mUI.addButton("credits", { smallStartX, smallPad + smallBtnH + smallPad, smallBtnW, smallBtnH }, "main_credits", BUTTON_YELLOW, BUTTON_HIGHLIGHT);

    mUI.updateButtonTexts();
}

// === 2. LOGIC NỀN (PARTICLES) ===
void MainMenuState::initParticles() {
    for (int i = 0; i < 50; ++i) {
        FloatingBlock b;
        b.x = (float)(rand() % SCREEN_WIDTH);
        b.y = (float)(rand() % SCREEN_HEIGHT);
        b.size = 20 + rand() % 40;
        b.speed = 0.5f + (rand() % 100) / 100.0f;
        b.color = PALETTE[rand() % PALETTE.size()];
        b.alpha = 50 + rand() % 100;
        mParticles.push_back(b);
    }
}

void MainMenuState::updateParticles() {
    for (auto& b : mParticles) {
        b.y -= b.speed;
        if (b.y + b.size < 0) {
            b.y = (float)SCREEN_HEIGHT;
            b.x = (float)(rand() % SCREEN_WIDTH);
            b.color = PALETTE[rand() % PALETTE.size()];
        }
    }
}

void MainMenuState::drawParticles(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (const auto& b : mParticles) {
        SDL_SetRenderDrawColor(renderer, b.color.r, b.color.g, b.color.b, b.alpha);
        SDL_Rect r = { (int)b.x, (int)b.y, b.size, b.size };
        SDL_RenderFillRect(renderer, &r);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// === 3. LOGIC UPDATE CHÍNH ===
void MainMenuState::update() {
    updateParticles(); // Cập nhật nền

    // CẬP NHẬT GLITCH
    if (mIsGlitching) {
        mGlitchTimer--;
        // Rung lắc ngẫu nhiên
        mGlitchOffsetX = (rand() % 10) - 5;
        mGlitchOffsetY = (rand() % 6) - 3;

        if (mGlitchTimer <= 0) {
            mIsGlitching = false; // Kết thúc nhiễu
            mGlitchTimer = rand() % 100 + 50; // Chờ một lúc
        }
    }
    else {
        mGlitchTimer--;
        if (mGlitchTimer <= 0) {
            mIsGlitching = true; // Bắt đầu nhiễu
            mGlitchTimer = rand() % 10 + 5;
        }
    }
}

// === 4. VẼ TIÊU ĐỀ GLITCH ===
void MainMenuState::drawGlitchTitle(SDL_Renderer* renderer) {
    auto res = mGame->getResources();
    TTF_Font* font = res->getFontTitle();
    std::string text = res->getText("main_title");

    int centerX = SCREEN_WIDTH / 2;
    int centerY = 120;

    if (mIsGlitching) {
        // Lớp Đỏ (Lệch trái)
        drawText(renderer, text, centerX - 5 + mGlitchOffsetX, centerY + mGlitchOffsetY, font, { 255, 0, 0, 200 });
        // Lớp Xanh (Lệch phải)
        drawText(renderer, text, centerX + 5 - mGlitchOffsetX, centerY - mGlitchOffsetY, font, { 0, 255, 255, 200 });
        // Lớp Trắng (Giữa)
        drawText(renderer, text, centerX + mGlitchOffsetX, centerY + mGlitchOffsetY, font, TEXT_WHITE);

        // Kẻ vài đường ngang (Scanlines)
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 200);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int i = 0; i < 3; ++i) {
            int ly = centerY - 30 + rand() % 60;
            SDL_RenderDrawLine(renderer, centerX - 200, ly, centerX + 200, ly);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    }
    else {
        // Bình thường: Bóng + Chữ
        drawText(renderer, text, centerX + 4, centerY + 4, font, { 50, 0, 50, 255 });
        drawText(renderer, text, centerX, centerY, font, PALETTE[4]);
    }
}

// === 5. HÀM DRAW CHÍNH ===
void MainMenuState::draw(SDL_Renderer* renderer) {
    auto res = mGame->getResources();
    auto fontUI = res->getFontUI();
    auto fontSmall = res->getFontSmall();

    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, 255);
    SDL_RenderClear(renderer);

    // 1. Vẽ nền (ô vuông bay)
    drawParticles(renderer);

    // 2. Vẽ tiêu đề Glitch
    drawGlitchTitle(renderer);

    // 3. Vẽ các nút
    if (!mGame->getSaveFileExists()) {
        Button* continueBtn = mUI.getButton("main_continue");
        if (continueBtn) {
            continueBtn->draw(renderer, fontUI);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150); // Làm tối nút Continue
            SDL_Rect r = continueBtn->getRect();
            SDL_RenderFillRect(renderer, &r);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        mUI.getButton("main_new_game")->draw(renderer, fontUI);
        mUI.getButton("main_ai_solver")->draw(renderer, fontUI);
        mUI.getButton("main_settings")->draw(renderer, fontUI);
        mUI.getButton("main_exit")->draw(renderer, fontUI);
    }
    else {
        mUI.getButton("main_continue")->draw(renderer, fontUI);
        mUI.getButton("main_new_game")->draw(renderer, fontUI);
        mUI.getButton("main_ai_solver")->draw(renderer, fontUI);
        mUI.getButton("main_settings")->draw(renderer, fontUI);
        mUI.getButton("main_exit")->draw(renderer, fontUI);
    }

    mUI.getButton("instructions")->draw(renderer, fontSmall);
    mUI.getButton("credits")->draw(renderer, fontSmall);
}

// === 6. XỬ LÝ SỰ KIỆN ===
void MainMenuState::handleEvents(SDL_Event& e) {
    std::string action = mUI.handleEvents(e);

    if (action == "main_continue") {
        if (mGame->getSaveFileExists()) {
            loadAndPushPlayingState(true);
        }
    }
    else if (action == "main_new_game") {
        mGame->deleteSaveFile();
        mGame->setHighestLevel(1);
        mGame->saveHighestLevel();
        loadAndPushPlayingState(false);
    }
    else if (action == "main_ai_solver") {
        mGame->changeState(new AISetupState(mGame));
    }
    else if (action == "main_settings") {
        mGame->changeState(new SettingsState(mGame));
    }
    else if (action == "main_exit") {
        mGame->requestQuit();
    }
    else if (action == "instructions") {
        mGame->pushState(new InstructionState(mGame));
    }
    else if (action == "credits") {
        mGame->pushState(new CreditsState(mGame));
    }
}

// === 7. LOAD GAME ===
void MainMenuState::loadAndPushPlayingState(bool isContinue) {
    if (isContinue) {
        std::ifstream saveFile("savegame.txt");
        if (!saveFile.is_open()) {
            mGame->setSaveFileExists(false);
            return;
        }
        int level, gridDim, numColors, moves, maxMoves;
        double savedTime;
        std::string label;

        saveFile >> label >> level;
        saveFile >> label >> gridDim;
        saveFile >> label >> numColors;
        saveFile >> label >> moves;
        saveFile >> label >> maxMoves;
        saveFile >> label >> savedTime;

        if (!saveFile) {
            saveFile.close(); mGame->deleteSaveFile(); mGame->setSaveFileExists(false); return;
        }
        std::vector<std::vector<SDL_Color>> initialGrid(gridDim, std::vector<SDL_Color>(gridDim));
        std::vector<std::vector<SDL_Color>> currentGrid(gridDim, std::vector<SDL_Color>(gridDim));

        saveFile >> label;
        if (label != "InitialGrid:") {
            saveFile.close(); mGame->deleteSaveFile(); mGame->setSaveFileExists(false); return;
        }
        for (int y = 0; y < gridDim; ++y) {
            for (int x = 0; x < gridDim; ++x) {
                int r, g, b; saveFile >> r >> g >> b;
                initialGrid[x][y] = { (Uint8)r, (Uint8)g, (Uint8)b, 255 };
            }
        }
        saveFile >> label;
        if (label != "CurrentGrid:") {
            saveFile.close(); mGame->deleteSaveFile(); mGame->setSaveFileExists(false); return;
        }
        for (int y = 0; y < gridDim; ++y) {
            for (int x = 0; x < gridDim; ++x) {
                int r, g, b; saveFile >> r >> g >> b;
                currentGrid[x][y] = { (Uint8)r, (Uint8)g, (Uint8)b, 255 };
            }
        }
        saveFile.close();

        mGame->pushState(new PlayingState(mGame, mGame->getResources(), level, gridDim, numColors, maxMoves,
            mGame->getAlgorithm(), mGame->getFillDirection(), moves, savedTime, initialGrid, currentGrid));
    }
    else {
        int level = mGame->getHighestLevel();
        int gridDim = 6 + (level - 1) * 2; gridDim = std::min(gridDim, 40);
        int numColors = mGame->getNumColors();
        float modifier = mGame->getDifficultyModifier();
        int maxMoves = static_cast<int>((gridDim * gridDim * numColors) / (gridDim * modifier) + 5);

        mGame->pushState(new PlayingState(mGame, mGame->getResources(), level, gridDim, numColors, maxMoves,
            mGame->getAlgorithm(), mGame->getFillDirection()));
    }
}