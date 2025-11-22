#pragma once

#include "Constants.h"
#include "DSU.h"
#include <queue>
#include <stack>
#include <algorithm>
class Grid {
private:
    int mDim;
    int mNumColors;
    int mCellSize;
    int mMapOffsetX, mMapOffsetY;

    std::vector<std::vector<SDL_Color>> mCells;
    std::vector<std::vector<SDL_Color>> mInitialCells;

    AlgorithmType mAlgoType;
    FillDirection mFillDir;

    bool mIsAnimating;
    SDL_Color mTargetColor, mReplacementColor;
    std::queue<std::pair<int, int>> mBfsQueue;
    std::stack<std::pair<int, int>> mDfsStack;
    std::stack<std::pair<int, int>> mScanlineStack;

    std::vector<std::pair<int, int>> mPendingChangeCells;

    int countConnectedRegion(const std::vector<std::vector<SDL_Color>>& grid) const;
    void recalculateRenderParams();

public:
    Grid();
    void init(int dim, int numColors, AlgorithmType algo, FillDirection fillDir);

    void generateMap();
    void setGrid(const std::vector<std::vector<SDL_Color>>& grid);
    void setInitialGrid(const std::vector<std::vector<SDL_Color>>& grid);
    void resetToInitial();

    void draw(SDL_Renderer* renderer, bool showControlledRegion, bool showStartHint, bool showEventWarning) const;
    void drawInViewport(SDL_Renderer* renderer, int startX, int startY, int cellSize) const;

    std::vector<std::vector<bool>> findControlledRegion() const;

    bool startFloodFill(const SDL_Color& newColor);
    bool animateFillStep();

    bool isWinConditionMet() const;
    PredictionResult predictProgress() const;
    SDL_Color findBestMove() const;

    void selectRandomCellsForChangeEvent(int count);
    bool executeColorChangeEvent();
    const std::vector<std::pair<int, int>>& getPendingChangeCells() const { return mPendingChangeCells; }

    const std::vector<std::vector<SDL_Color>>& getCells() const { return mCells; }
    std::vector<std::vector<SDL_Color>>& getCells_NonConst() { return mCells; }

    void setCells(const std::vector<std::vector<SDL_Color>>& cells) { mCells = cells; }
    const std::vector<std::vector<SDL_Color>>& getInitialCells() const { return mInitialCells; }
    SDL_Color getColorFromClick(int mX, int mY) const;
    bool isClickInBounds(int mX, int mY) const;
    int getDim() const { return mDim; }
    bool isAnimating() const { return mIsAnimating; }

};