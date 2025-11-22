#include "AISolvingState.h"
#include "MainMenuState.h"
#include "AISetupState.h"
#include "Game.h"
#include <cstdlib>
AISolvingState::AISolvingState(Game* game, int gridDim, int numColors, bool useDrawnGrid, const std::vector<std::vector<SDL_Color>>& drawnGrid)
    : GameState(game)
{
    std::vector<std::vector<SDL_Color>> masterGrid;
    if (useDrawnGrid) {
        masterGrid = drawnGrid;
    }
    else {
        generateMap(masterGrid, gridDim, numColors);
    }

    FillDirection fillDir = mGame->getFillDirection();

    mSolvers[0].init(ALGO_BFS, fillDir, masterGrid, numColors);
    mSolvers[1].init(ALGO_DFS, fillDir, masterGrid, numColors);
    mSolvers[2].init(ALGO_SCANLINE, fillDir, masterGrid, numColors);
    mSolvers[3].init(ALGO_UNION_FIND, fillDir, masterGrid, numColors);

    initUI();
}

void AISolvingState::generateMap(std::vector<std::vector<SDL_Color>>& grid, int dim, int numColors) {
    grid.assign(dim, std::vector<SDL_Color>(dim));
    for (int y = 0; y < dim; ++y) {
        for (int x = 0; x < dim; ++x) {
            grid[x][y] = PALETTE[rand() % numColors];
        }
    }
}

void AISolvingState::initUI() {
    mUI.init(mGame->getResources());

    int bottom_panel_height = 80;
    int btn_h = 50;
    int bottom_panel_y = SCREEN_HEIGHT - bottom_panel_height + (bottom_panel_height - btn_h) / 2;

    mUI.addButton("new_map", { SCREEN_WIDTH / 2 - 220, bottom_panel_y, 200, btn_h }, "ai_solving_new_map", BUTTON_BLUE, BUTTON_HIGHLIGHT);

    mUI.addButton("menu", { SCREEN_WIDTH / 2 + 20, bottom_panel_y, 200, btn_h }, "ai_solving_menu", BUTTON_GRAY, BUTTON_HIGHLIGHT);

    mUI.updateButtonTexts();
}

void AISolvingState::handleEvents(SDL_Event& e) {
    std::string action = mUI.handleEvents(e);

    if (action == "new_map") {
        mGame->changeState(new AISetupState(mGame));
    }
    else if (action == "menu") {
        mGame->changeState(new MainMenuState(mGame));
    }
}

void AISolvingState::update() {
    for (int i = 0; i < 4; ++i) {
        mSolvers[i].update();
    }
}

void AISolvingState::draw(SDL_Renderer* renderer) {
    auto res = mGame->getResources();
    auto fontUI = res->getFontUI();
    auto fontSmall = res->getFontSmall();

    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, 255);
    SDL_RenderClear(renderer);

    const int bottom_panel_height = 80;
    int viewWidth = SCREEN_WIDTH / 2 - 20;
    int viewHeight = (SCREEN_HEIGHT - bottom_panel_height) / 2 - 20;

    SDL_Rect viewports[4] = {
        {10, 10, viewWidth, viewHeight},        {SCREEN_WIDTH / 2 + 10, 10, viewWidth, viewHeight},        {10, (SCREEN_HEIGHT - bottom_panel_height) / 2 + 10, viewWidth, viewHeight},        {SCREEN_WIDTH / 2 + 10, (SCREEN_HEIGHT - bottom_panel_height) / 2 + 10, viewWidth, viewHeight}    };

    for (int i = 0; i < 4; ++i) {
        mSolvers[i].draw(renderer, viewports[i], fontUI, fontSmall, res);
    }

    SDL_Rect bottomPanel = { 0, SCREEN_HEIGHT - bottom_panel_height, SCREEN_WIDTH, bottom_panel_height };
    SDL_SetRenderDrawColor(renderer, UI_BG_COLOR.r, UI_BG_COLOR.g, UI_BG_COLOR.b, 255);
    SDL_RenderFillRect(renderer, &bottomPanel);

    mUI.getButton("new_map")->draw(renderer, fontUI);
    mUI.getButton("menu")->draw(renderer, fontUI);
}