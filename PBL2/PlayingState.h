#pragma once

#include "GameState.h"
#include "Grid.h"
#include "UIManager.h"
#include "ResourceManager.h"
#include "Constants.h"
#include <chrono>
#include <stack>
#include <vector> 

// Struct cho hạt trang trí (Hiệu ứng nền)
struct GameParticle {
    float x, y;
    float speed;
    int size;
    int alpha;
    SDL_Color color;
};

class Game;

class PlayingState : public GameState {
public:
    PlayingState(Game* game, ResourceManager* resManager, int level, int gridDim, int numColors, int maxMoves, AlgorithmType algo, FillDirection fillDir);

    PlayingState(Game* game, ResourceManager* resManager, int level, int gridDim, int numColors, int maxMoves, AlgorithmType algo, FillDirection fillDir,
        int moves, double savedTime, const std::vector<std::vector<SDL_Color>>& initialGrid, const std::vector<std::vector<SDL_Color>>& currentGrid);

    virtual ~PlayingState();

    virtual void handleEvents(SDL_Event& e) override;
    virtual void update() override;
    virtual void draw(SDL_Renderer* renderer) override;

    void saveGameState();
    void deleteSaveFile();
    void retryCurrentLevel();

private:
    static const double EVENT_INTERVAL;
    static const double EVENT_WARNING_DURATION;

    Grid mGrid;
    UIManager mUI;
    ResourceManager* mResManager;

    int mPlayerLevel;
    int mGridDim;
    int mNumColors;
    int mMaxMoves;
    int mMoves;
    bool mGameWon;
    bool mGameLost;

    std::stack<std::vector<std::vector<SDL_Color>>> mGridHistory;
    std::stack<int> mMovesHistory;

    bool mIsHintActive;
    SDL_Color mHintColor;

    std::chrono::steady_clock::time_point mLevelStartTime;
    std::chrono::duration<double> mElapsedTime;
    double mSavedElapsedTime;

    PredictionResult mPredictionResult;

    std::chrono::steady_clock::time_point mRandomEventTimer;
    bool mIsEventWarningActive;
    int mEventCellChangeCount;

    // === BIẾN TRANG TRÍ ===
    std::vector<GameParticle> mParticles;

    void initUI();
    void resetTimer();
    void updateTimer();
    void updateRandomEvent();

    // === HÀM TRANG TRÍ ===
    void initParticles();
    void updateParticles();
    void drawDecoration(SDL_Renderer* renderer);

    void startFloodFill(const SDL_Color& newColor);
    void checkWinCondition();
    void undoLastMove();
    void predictProgress();
};