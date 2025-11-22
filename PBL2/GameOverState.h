#pragma once
#include "GameState.h"
#include "UIManager.h"
#include <chrono>
class GameOverState : public GameState {
public:
    GameOverState(Game* game, bool won, int level, double time);
    virtual ~GameOverState() {}

    virtual void handleEvents(SDL_Event& e) override;
    virtual void update() override;    virtual void draw(SDL_Renderer* renderer) override;

private:
    void initButtons();

    UIManager mUI;
    bool mGameWon;
    int mPlayerLevel;
    double mTime;

    std::chrono::steady_clock::time_point mStartTime;    bool mVolumeRestored;};