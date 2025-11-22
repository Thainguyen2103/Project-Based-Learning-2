#pragma once
#include "GameState.h"
#include "UIManager.h"

class AIDrawMapState : public GameState {
public:
    AIDrawMapState(Game* game, int gridDim, int numColors);
    virtual ~AIDrawMapState() {}

    virtual void handleEvents(SDL_Event& e) override;
    virtual void update() override;
    virtual void draw(SDL_Renderer* renderer) override;

private:
    void initUI();
    void clearMap();
    void handlePaint(int mX, int mY);

    UIManager mUI;

    int mGridDim;
    int mNumColors;
    std::vector<std::vector<SDL_Color>> mDrawnGrid;

    SDL_Color mSelectedColor;
    std::vector<SDL_Rect> mPaletteRects;
    int mCellSize;
    int mMapOffsetX, mMapOffsetY;
};