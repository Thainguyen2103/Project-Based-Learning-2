#include "PlayingState.h"
#include "Game.h" 
#include "GameOverState.h" 
#include "MainMenuState.h" 
#include <fstream>
#include <cstdio> 
#include <iostream> 
#include <cstdlib> // Cho rand()
#include <sstream> // Cho stringstream
#include <SDL_mixer.h> 

const double PlayingState::EVENT_INTERVAL = 10.0;
const double PlayingState::EVENT_WARNING_DURATION = 3.0;

// Constructor 1 (Game Mới)
PlayingState::PlayingState(Game* game, ResourceManager* resManager, int level, int gridDim, int numColors, int maxMoves, AlgorithmType algo, FillDirection fillDir)
    : GameState(game), mResManager(resManager), mPlayerLevel(level), mGridDim(gridDim), mNumColors(numColors),
    mMaxMoves(maxMoves), mMoves(0), mGameWon(false), mGameLost(false), mIsHintActive(false), mHintColor({ 0,0,0,0 }),
    mSavedElapsedTime(0.0), mIsEventWarningActive(false), mEventCellChangeCount(2)
{
    Mix_VolumeMusic(MIX_MAX_VOLUME);
    mGrid.init(mGridDim, mNumColors, algo, fillDir);

    initUI();
    initParticles(); // <--- TRANG TRÍ

    resetTimer();
    mRandomEventTimer = std::chrono::steady_clock::now();
    predictProgress();
    saveGameState();
}

// Constructor 2 (Load Game)
PlayingState::PlayingState(Game* game, ResourceManager* resManager, int level, int gridDim, int numColors, int maxMoves, AlgorithmType algo, FillDirection fillDir,
    int moves, double savedTime, const std::vector<std::vector<SDL_Color>>& initialGrid, const std::vector<std::vector<SDL_Color>>& currentGrid)
    : GameState(game), mResManager(resManager), mPlayerLevel(level), mGridDim(gridDim), mNumColors(numColors),
    mMaxMoves(maxMoves), mMoves(moves), mGameWon(false), mGameLost(false), mIsHintActive(false), mHintColor({ 0,0,0,0 }),
    mSavedElapsedTime(savedTime), mIsEventWarningActive(false), mEventCellChangeCount(2)
{
    Mix_VolumeMusic(MIX_MAX_VOLUME);
    mGrid.init(mGridDim, mNumColors, algo, fillDir);
    mGrid.setCells(currentGrid);
    mGrid.setInitialGrid(initialGrid);

    initUI();
    initParticles(); // <--- TRANG TRÍ

    mLevelStartTime = std::chrono::steady_clock::now();
    mRandomEventTimer = std::chrono::steady_clock::now();
    predictProgress();
}

PlayingState::~PlayingState() {}

// === 1. HÀM KHỞI TẠO HẠT (TRANG TRÍ) ===
void PlayingState::initParticles() {
    int areaW = SCREEN_WIDTH - UI_PANEL_WIDTH; // Chỉ bay ở vùng bàn cờ
    for (int i = 0; i < 30; ++i) {
        GameParticle p;
        p.x = (float)(rand() % areaW);
        p.y = (float)(rand() % SCREEN_HEIGHT);
        p.size = 10 + rand() % 30;
        p.speed = 0.2f + (rand() % 50) / 100.0f;
        p.alpha = 20 + rand() % 40; // Mờ ảo
        p.color = PALETTE[rand() % mNumColors];
        mParticles.push_back(p);
    }
}

// === 2. HÀM CẬP NHẬT HẠT ===
void PlayingState::updateParticles() {
    int areaW = SCREEN_WIDTH - UI_PANEL_WIDTH;
    for (auto& p : mParticles) {
        p.y -= p.speed; // Bay lên
        if (p.y + p.size < 0) {
            p.y = (float)SCREEN_HEIGHT;
            p.x = (float)(rand() % areaW);
            p.color = PALETTE[rand() % mNumColors];
        }
    }
}

void PlayingState::initUI() {
    mUI.init(mResManager);
    int gameUIX = SCREEN_WIDTH - UI_PANEL_WIDTH;
    int gameUIW = UI_PANEL_WIDTH;
    int gameBtnW = gameUIW - 80;
    int gameBtnH = 50;
    int gameBtnMargin = 15;
    int gameBtnStartX = gameUIX + (gameUIW - gameBtnW) / 2;

    int currentY = SCREEN_HEIGHT - 20 - gameBtnH;
    mUI.addButton("game_back_to_menu", { gameBtnStartX, currentY, gameBtnW, gameBtnH }, "game_back_to_menu", BUTTON_GRAY, BUTTON_HIGHLIGHT);

    currentY -= (gameBtnH + gameBtnMargin);
    mUI.addButton("game_replay_level", { gameBtnStartX, currentY, gameBtnW, gameBtnH }, "game_replay_level", BUTTON_GREEN, BUTTON_HIGHLIGHT);

    currentY -= (gameBtnH + gameBtnMargin);
    mUI.addButton("game_hint", { gameBtnStartX, currentY, gameBtnW, gameBtnH }, "game_hint", BUTTON_YELLOW, BUTTON_HIGHLIGHT);

    currentY -= (gameBtnH + gameBtnMargin);
    mUI.addButton("game_undo", { gameBtnStartX, currentY, gameBtnW, gameBtnH }, "game_undo", BUTTON_BLUE, BUTTON_HIGHLIGHT);
}

void PlayingState::handleEvents(SDL_Event& e) {
    if (mGrid.isAnimating() || mGameWon || mGameLost) return;

    std::string action = mUI.handleEvents(e);
    if (!action.empty()) {
        if (action == "game_back_to_menu") { saveGameState(); mGame->changeState(new MainMenuState(mGame)); }
        else if (action == "game_replay_level") retryCurrentLevel();
        else if (action == "game_hint") { mHintColor = mGrid.findBestMove(); mIsHintActive = true; }
        else if (action == "game_undo") undoLastMove();
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (mGrid.isClickInBounds(e.button.x, e.button.y)) {
            // Âm thanh ngẫu nhiên
            Mix_Chunk* fillSfx = mResManager->getRandomFillSound();
            if (fillSfx) Mix_PlayChannel(-1, fillSfx, 0);

            startFloodFill(mGrid.getColorFromClick(e.button.x, e.button.y));
        }
        return;
    }
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_z && (e.key.keysym.mod & KMOD_CTRL)) undoLastMove();
    }
}

void PlayingState::update() {
    if (mGameWon || mGameLost) return;

    updateTimer();
    updateRandomEvent();
    updateParticles(); // <--- CẬP NHẬT TRANG TRÍ

    if (mGrid.isAnimating()) {
        bool finished = mGrid.animateFillStep();
        if (finished) {
            checkWinCondition();
            predictProgress();
            saveGameState();
        }
    }
    else {
        checkWinCondition();
    }
}

// === 3. HÀM VẼ TRANG TRÍ ===
void PlayingState::drawDecoration(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (const auto& p : mParticles) {
        SDL_SetRenderDrawColor(renderer, p.color.r, p.color.g, p.color.b, p.alpha);
        SDL_Rect r = { (int)p.x, (int)p.y, p.size, p.size };
        SDL_RenderFillRect(renderer, &r);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void PlayingState::draw(SDL_Renderer* renderer) {
    // 1. Vẽ nền đen
    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, 255);
    SDL_RenderClear(renderer);

    // 2. Vẽ hiệu ứng nền (Dưới Grid)
    drawDecoration(renderer);

    // 3. Vẽ Tấm Nền UI (Bên phải) - Đè lên hiệu ứng nền
    SDL_Rect uiArea = { SCREEN_WIDTH - UI_PANEL_WIDTH, 0, UI_PANEL_WIDTH, SCREEN_HEIGHT };
    SDL_SetRenderDrawColor(renderer, 34, 42, 53, 255);
    SDL_RenderFillRect(renderer, &uiArea);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Kẻ dọc
    SDL_RenderDrawLine(renderer, uiArea.x, 0, uiArea.x, SCREEN_HEIGHT);

    // 4. Vẽ Viền Phát Sáng Quanh Grid (Glow Border)
    // Lấy màu của người chơi hiện tại (ô 0,0)
    SDL_Color playerColor = { 255, 255, 255, 255 };
    if (!mGrid.getCells().empty()) {
        playerColor = mGrid.getCells()[0][0];
    }

    int mapAreaW = SCREEN_WIDTH - UI_PANEL_WIDTH;
    int gridSizePx = 680;
    int mapX = (mapAreaW - gridSizePx) / 2;
    int mapY = (SCREEN_HEIGHT - gridSizePx) / 2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < 15; ++i) { // 15 lớp viền mờ dần
        int alpha = 100 - (i * 6);
        if (alpha < 0) alpha = 0;
        SDL_SetRenderDrawColor(renderer, playerColor.r, playerColor.g, playerColor.b, alpha);
        SDL_Rect glowRect = { mapX - i, mapY - i, gridSizePx + i * 2, gridSizePx + i * 2 };
        SDL_RenderDrawRect(renderer, &glowRect);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // 5. Vẽ Grid (Nằm giữa khung viền)
    bool showStart = (mMoves == 0);
    mGrid.draw(renderer, true, showStart, mIsEventWarningActive);

    // 6. Vẽ UI Info
    int centerX = uiArea.x + uiArea.w / 2;
    int y_offset = 60;
    const int PADDING_SECTION = 60;

    // Tiêu đề Level
    TTF_Font* fontTitle = mResManager->getFontTitle();
    TTF_Font* fontUI = mResManager->getFontUI();
    std::stringstream ss;
    ss << mResManager->getText("game_level") << " " << mPlayerLevel;
    drawText(renderer, ss.str(), centerX, y_offset, fontTitle, TEXT_WHITE);
    y_offset += PADDING_SECTION + 20;

    // Moves
    drawText(renderer, mResManager->getText("game_moves"), centerX, y_offset, fontUI, PALETTE[3]);
    y_offset += 40;
    ss.str(""); ss << mMoves << " / " << mMaxMoves;
    drawText(renderer, ss.str(), centerX, y_offset, fontTitle, TEXT_WHITE);
    y_offset += PADDING_SECTION + 20;

    // Time
    drawText(renderer, mResManager->getText("game_time"), centerX, y_offset, fontUI, PALETTE[5]);
    y_offset += 40;
    int totalSeconds = static_cast<int>(mElapsedTime.count());
    int minutes = totalSeconds / 60; int seconds = totalSeconds % 60;
    ss.str(""); ss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;
    drawText(renderer, ss.str(), centerX, y_offset, fontTitle, TEXT_WHITE);

    mUI.draw(renderer);

    // 7. Vẽ Hint
    if (mIsHintActive) {
        Button* undoButton = mUI.getButton("game_undo");
        if (undoButton) {
            int hintBoxY = undoButton->getRect().y - 70;
            int hintTextY = hintBoxY - 35;
            drawText(renderer, mResManager->getText("game_ai_suggestion"), centerX, hintTextY, fontUI, PALETTE[3]);
            SDL_Rect hintColorRect = { centerX - 25, hintBoxY, 50, 50 };
            SDL_SetRenderDrawColor(renderer, mHintColor.r, mHintColor.g, mHintColor.b, 255);
            SDL_RenderFillRect(renderer, &hintColorRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &hintColorRect);
        }
    }
}

void PlayingState::resetTimer() {
    mSavedElapsedTime = 0.0;
    mElapsedTime = std::chrono::duration<double>::zero();
    mLevelStartTime = std::chrono::steady_clock::now();
}

void PlayingState::updateTimer() {
    if (mGameWon || mGameLost) return;
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> sessionTime = now - mLevelStartTime;
    mElapsedTime = sessionTime + std::chrono::duration<double>(mSavedElapsedTime);
}

void PlayingState::updateRandomEvent() {
    if (mGrid.isAnimating() || mGameWon || mGameLost) return;
    if (!mIsEventWarningActive) {
        auto now = std::chrono::steady_clock::now();
        double timeSinceLastEvent = std::chrono::duration_cast<std::chrono::duration<double>>(now - mRandomEventTimer).count();
        if (timeSinceLastEvent > EVENT_INTERVAL - EVENT_WARNING_DURATION) {
            mGrid.selectRandomCellsForChangeEvent(mEventCellChangeCount);
            if (!mGrid.getPendingChangeCells().empty()) {
                mIsEventWarningActive = true;
                mRandomEventTimer = std::chrono::steady_clock::now();
            }
        }
    }
    else {
        auto now = std::chrono::steady_clock::now();
        double warningTime = std::chrono::duration_cast<std::chrono::duration<double>>(now - mRandomEventTimer).count();
        if (warningTime > EVENT_WARNING_DURATION) {
            mGridHistory.push(mGrid.getCells());
            mMovesHistory.push(mMoves);
            mGrid.executeColorChangeEvent();
            mIsEventWarningActive = false;
            mEventCellChangeCount++;
            mRandomEventTimer = std::chrono::steady_clock::now();
            predictProgress();
            checkWinCondition();
        }
    }
}

void PlayingState::startFloodFill(const SDL_Color& newColor) {
    mIsHintActive = false;
    mGridHistory.push(mGrid.getCells());
    mMovesHistory.push(mMoves);
    if (mGrid.startFloodFill(newColor)) {
        mMoves++;
        if (mGrid.isAnimating() == false) {
            checkWinCondition();
            predictProgress();
            saveGameState();
        }
    }
    else {
        mGridHistory.pop();
        mMovesHistory.pop();
    }
}

void PlayingState::checkWinCondition() {
    if (mGameWon || mGameLost) return;
    if (mGrid.isWinConditionMet()) {
        mGameWon = true; deleteSaveFile();
        Mix_VolumeMusic(30);
        Mix_Chunk* winSfx = mResManager->getWinSound();
        if (winSfx) Mix_PlayChannel(-1, winSfx, 0);
        mGame->pushState(new GameOverState(mGame, true, mPlayerLevel, mElapsedTime.count()));
    }
    else if (mMoves >= mMaxMoves) {
        mGameLost = true; deleteSaveFile();
        Mix_VolumeMusic(30);
        Mix_Chunk* loseSfx = mResManager->getLoseSound();
        if (loseSfx) Mix_PlayChannel(-1, loseSfx, 0);
        mGame->pushState(new GameOverState(mGame, false, mPlayerLevel, mElapsedTime.count()));
    }
}

void PlayingState::undoLastMove() {
    if (!mGridHistory.empty() && !mMovesHistory.empty()) {
        mGrid.setCells(mGridHistory.top()); mGridHistory.pop();
        mMoves = mMovesHistory.top(); mMovesHistory.pop();
        mIsHintActive = false; predictProgress(); saveGameState();
    }
}

void PlayingState::retryCurrentLevel() {
    mGrid.resetToInitial(); mMoves = 0;
    mGameWon = false; mGameLost = false; mIsHintActive = false;
    resetTimer();
    while (!mGridHistory.empty()) mGridHistory.pop();
    while (!mMovesHistory.empty()) mMovesHistory.pop();
    mRandomEventTimer = std::chrono::steady_clock::now();
    mIsEventWarningActive = false; mEventCellChangeCount = 2;
    predictProgress(); saveGameState();
}

void PlayingState::predictProgress() {
    mPredictionResult = mGrid.predictProgress();
    mPredictionResult.possibleToWin = (mPredictionResult.predictedSteps <= (mMaxMoves - mMoves));
}

void PlayingState::deleteSaveFile() { remove("savegame.txt"); mGame->setSaveFileExists(false); }

void PlayingState::saveGameState() {
    if (mGameWon || mGameLost) return;
    std::ofstream saveFile("savegame.txt");
    if (!saveFile.is_open()) return;
    updateTimer();
    double timeToSave = mElapsedTime.count();
    saveFile << "Level: " << mPlayerLevel << "\nGridDimension: " << mGridDim << "\nNumColors: " << mNumColors
        << "\nMoves: " << mMoves << "\nMaxMoves: " << mMaxMoves << "\nElapsedTime: " << timeToSave << "\nInitialGrid:\n";
    const auto& initialGrid = mGrid.getInitialCells();
    for (int y = 0; y < mGridDim; ++y) { for (int x = 0; x < mGridDim; ++x) saveFile << (int)initialGrid[x][y].r << " " << (int)initialGrid[x][y].g << " " << (int)initialGrid[x][y].b << " "; saveFile << "\n"; }
    saveFile << "CurrentGrid:\n";
    const auto& currentGrid = mGrid.getCells();
    for (int y = 0; y < mGridDim; ++y) { for (int x = 0; x < mGridDim; ++x) saveFile << (int)currentGrid[x][y].r << " " << (int)currentGrid[x][y].g << " " << (int)currentGrid[x][y].b << " "; saveFile << "\n"; }
    saveFile.close();
    mGame->setSaveFileExists(true);
}