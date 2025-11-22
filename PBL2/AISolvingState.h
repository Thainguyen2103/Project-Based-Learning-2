#pragma once
#include "GameState.h"
#include "UIManager.h"
#include "AISolver.h" 

class AISolvingState : public GameState {
public:
    AISolvingState(Game* game, int gridDim, int numColors, bool useDrawnGrid, const std::vector<std::vector<SDL_Color>>& drawnGrid);
    virtual ~AISolvingState() {}

    virtual void handleEvents(SDL_Event& e) override;
    virtual void update() override;
    virtual void draw(SDL_Renderer* renderer) override;

private:
    void initUI();
    void generateMap(std::vector<std::vector<SDL_Color>>& grid, int dim, int numColors);

    UIManager mUI;
    AISolver mSolvers[4];
};