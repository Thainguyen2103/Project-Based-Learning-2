#include "GameOverState.h"
#include "PlayingState.h"
#include "MainMenuState.h"
#include "Game.h"
#include <sstream> 

GameOverState::GameOverState(Game* game, bool won, int level, double time)
    : GameState(game), mGameWon(won), mPlayerLevel(level), mTime(time)
{
    mUI.init(mGame->getResources());
    initButtons();

    if (mGameWon) {
        mGame->setHighestLevel(mPlayerLevel + 1);
        mGame->saveHighestLevel();
    }
}

void GameOverState::initButtons() {
    auto res = mGame->getResources();
    int centerX = (SCREEN_WIDTH - UI_PANEL_WIDTH) / 2;
    int centerY = SCREEN_HEIGHT / 2;

    if (mGameWon) {
        mUI.addButton("next_level", { centerX - 125, centerY, 250, 60 }, "game_over_next_level", BUTTON_GREEN, BUTTON_HIGHLIGHT);
    }
    else {
        mUI.addButton("retry_game", { centerX - 125, centerY, 250, 60 }, "game_over_retry_game", BUTTON_BLUE, BUTTON_HIGHLIGHT);
    }
    mUI.addButton("retry_level", { centerX - 125, centerY + 70, 250, 60 }, "game_over_retry_level", BUTTON_YELLOW, BUTTON_HIGHLIGHT);
    mUI.addButton("menu", { centerX - 125, centerY + 140, 250, 60 }, "game_over_menu", BUTTON_GRAY, BUTTON_HIGHLIGHT);
    mUI.updateButtonTexts();
}

void GameOverState::handleEvents(SDL_Event& e) {
    std::string action = mUI.handleEvents(e);
    if (action.empty()) return;

    if (action == "next_level") {
        int nextLevel = mPlayerLevel + 1;
        int gridDim = 6 + (nextLevel - 1) * 2; gridDim = std::min(gridDim, 40);
        int numColors = mGame->getNumColors();

        float modifier = mGame->getDifficultyModifier();
        int maxMoves = static_cast<int>((gridDim * gridDim * numColors) / (gridDim * modifier) + 5);

        mGame->popAndChange(new PlayingState(mGame, mGame->getResources(), nextLevel, gridDim, numColors, maxMoves,
            mGame->getAlgorithm(), mGame->getFillDirection()));
    }
    else if (action == "retry_game") {
        int level = 1;
        mGame->setHighestLevel(1);
        mGame->saveHighestLevel();
        mGame->deleteSaveFile();

        int gridDim = 6 + (level - 1) * 2;
        int numColors = mGame->getNumColors();

        float modifier = mGame->getDifficultyModifier();
        int maxMoves = static_cast<int>((gridDim * gridDim * numColors) / (gridDim * modifier) + 5);

        mGame->popAndChange(new PlayingState(mGame, mGame->getResources(), level, gridDim, numColors, maxMoves,
            mGame->getAlgorithm(), mGame->getFillDirection()));
    }
    else if (action == "retry_level") {
        mGame->popAndRetry();
    }
    else if (action == "menu") {
        mGame->popStates(2);
    }
}

void GameOverState::update() {}

void GameOverState::draw(SDL_Renderer* renderer) {
    int mapOffsetX = (SCREEN_WIDTH - UI_PANEL_WIDTH - MAP_AREA_SIZE) / 2;
    int mapOffsetY = (SCREEN_HEIGHT - MAP_AREA_SIZE) / 2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlayRect = { mapOffsetX, mapOffsetY, MAP_AREA_SIZE, MAP_AREA_SIZE };
    SDL_RenderFillRect(renderer, &overlayRect);

    auto res = mGame->getResources();
    int centerX = mapOffsetX + MAP_AREA_SIZE / 2;
    int centerY = mapOffsetY + MAP_AREA_SIZE / 2;

    std::string msg = mGameWon ? res->getText("game_over_win") : res->getText("game_over_lose");
    drawText(renderer, msg, centerX, centerY - 120, res->getFontTitle(), { 255,255,0,255 });

    if (mGameWon) {
        std::stringstream ss;
        ss << res->getText("game_over_time") << " " << static_cast<int>(mTime) << " " << res->getText("game_over_seconds");
        drawText(renderer, ss.str(), centerX, centerY - 50, res->getFontUI(), TEXT_WHITE);
    }

    mUI.draw(renderer);
}