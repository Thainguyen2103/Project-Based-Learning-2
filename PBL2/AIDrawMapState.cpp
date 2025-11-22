#include "AIDrawMapState.h"
#include "AISolvingState.h"
#include "Game.h"

AIDrawMapState::AIDrawMapState(Game* game, int gridDim, int numColors)
    : GameState(game), mGridDim(gridDim), mNumColors(numColors)
{
    mUI.init(mGame->getResources());
    mSelectedColor = PALETTE[0];

    mCellSize = MAP_AREA_SIZE / mGridDim;
    mMapOffsetX = (SCREEN_WIDTH - UI_PANEL_WIDTH - MAP_AREA_SIZE) / 2;
    mMapOffsetY = (SCREEN_HEIGHT - MAP_AREA_SIZE) / 2;

    initUI();
    clearMap();
}

void AIDrawMapState::initUI() {
    int uiX = SCREEN_WIDTH - UI_PANEL_WIDTH;
    int uiW = UI_PANEL_WIDTH;

    mUI.addButton("start_solving", { uiX + 25, 450, uiW - 50, 60 }, "ai_draw_start_solving", BUTTON_GREEN, BUTTON_HIGHLIGHT);
    mUI.addButton("clear_map", { uiX + 25, 530, uiW - 50, 50 }, "ai_draw_clear_map", BUTTON_YELLOW, BUTTON_HIGHLIGHT);
    mUI.addButton("back_to_setup", { uiX + 25, 600, uiW - 50, 50 }, "ai_draw_back_to_setup", BUTTON_GRAY, BUTTON_HIGHLIGHT);

    mUI.updateButtonTexts();

    int colorBoxSize = 40;
    int colorBoxPad = 10;
    int cols = 4;
    int paletteTotalW = cols * colorBoxSize + (cols - 1) * colorBoxPad;
    int paletteStartX = uiX + (uiW - paletteTotalW) / 2;

    mPaletteRects.clear();    for (int i = 0; i < 8; ++i) {        int r = i / cols;
        int c = i % cols;
        mPaletteRects.push_back({
            paletteStartX + c * (colorBoxSize + colorBoxPad),
            180 + r * (colorBoxSize + colorBoxPad),
            colorBoxSize, colorBoxSize
            });
    }
}

void AIDrawMapState::clearMap() {
    mDrawnGrid.assign(mGridDim, std::vector<SDL_Color>(mGridDim, mSelectedColor));
}

void AIDrawMapState::handlePaint(int mX, int mY) {
    if (mCellSize == 0) return;
    int gridX = (mX - mMapOffsetX) / mCellSize;
    int gridY = (mY - mMapOffsetY) / mCellSize;

    if (gridX >= 0 && gridX < mGridDim && gridY >= 0 && gridY < mGridDim) {
        mDrawnGrid[gridX][gridY] = mSelectedColor;
    }
}

void AIDrawMapState::handleEvents(SDL_Event& e) {
    std::string action = mUI.handleEvents(e);

    if (action == "start_solving") {
        mGame->changeState(new AISolvingState(mGame, mGridDim, mNumColors, true, mDrawnGrid));
    }
    else if (action == "clear_map") {
        clearMap();
    }
    else if (action == "back_to_setup") {
        mGame->popState();    }
    else if (e.type == SDL_MOUSEBUTTONDOWN) {
        for (int i = 0; i < (int)mPaletteRects.size(); ++i) {
            if (i < (int)PALETTE.size()) {                SDL_Point mousePoint = { e.button.x, e.button.y };
                if (SDL_PointInRect(&mousePoint, &mPaletteRects[i])) {
                    mSelectedColor = PALETTE[i];
                    break;
                }
            }
        }
        handlePaint(e.button.x, e.button.y);
    }
    else if (e.type == SDL_MOUSEMOTION && (e.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT))) {
        handlePaint(e.motion.x, e.motion.y);
    }
}

void AIDrawMapState::update() {
}

void AIDrawMapState::draw(SDL_Renderer* renderer) {
    auto res = mGame->getResources();

    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, 255);
    SDL_RenderClear(renderer);

    SDL_Rect cellRect = { 0, 0, mCellSize, mCellSize };
    for (int y = 0; y < mGridDim; ++y) {
        for (int x = 0; x < mGridDim; ++x) {
            cellRect.x = mMapOffsetX + x * mCellSize;
            cellRect.y = mMapOffsetY + y * mCellSize;
            SDL_Color c = mDrawnGrid[x][y];
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
            SDL_RenderFillRect(renderer, &cellRect);
        }
    }
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for (int i = 0; i <= mGridDim; ++i) {
        SDL_RenderDrawLine(renderer, mMapOffsetX + i * mCellSize, mMapOffsetY, mMapOffsetX + i * mCellSize, mMapOffsetY + (mCellSize * mGridDim));
        SDL_RenderDrawLine(renderer, mMapOffsetX, mMapOffsetY + i * mCellSize, mMapOffsetX + (mCellSize * mGridDim), mMapOffsetY + i * mCellSize);
    }

    SDL_Rect uiArea = { SCREEN_WIDTH - UI_PANEL_WIDTH, 0, UI_PANEL_WIDTH, SCREEN_HEIGHT };
    SDL_SetRenderDrawColor(renderer, UI_BG_COLOR.r, UI_BG_COLOR.g, UI_BG_COLOR.b, 255);
    SDL_RenderFillRect(renderer, &uiArea);

    mUI.draw(renderer);

    int uiCenterX = uiArea.x + uiArea.w / 2;

    drawText(renderer, res->getText("ai_draw_title"), uiCenterX, 60, res->getFontUI(), TEXT_WHITE);

    drawText(renderer, res->getText("ai_draw_palette"), uiCenterX, 140, res->getFontUI(), PALETTE[3]);

    for (int i = 0; i < (int)mPaletteRects.size(); ++i) {
        if (i < (int)PALETTE.size()) {
            const auto& color = PALETTE[i];
            const auto& rect = mPaletteRects[i];
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_RenderFillRect(renderer, &rect);

            if (areColorsEqual(color, mSelectedColor)) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }

    drawText(renderer, res->getText("ai_draw_selected_color"), uiCenterX, 320, res->getFontUI(), PALETTE[5]);
    SDL_Rect selectedColorBox = { uiCenterX - 30, 360, 60, 60 };
    SDL_SetRenderDrawColor(renderer, mSelectedColor.r, mSelectedColor.g, mSelectedColor.b, 255);
    SDL_RenderFillRect(renderer, &selectedColorBox);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &selectedColorBox);
}