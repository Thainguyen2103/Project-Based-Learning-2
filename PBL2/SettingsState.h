#pragma once
#include "GameState.h"
#include "UIManager.h"

class SettingsState : public GameState {
public:
    SettingsState(Game* game);
    virtual ~SettingsState() {}

    virtual void handleEvents(SDL_Event& e) override;
    virtual void update() override;
    virtual void draw(SDL_Renderer* renderer) override;

private:
    void initButtons();
    void updateButtonTexts();

    // === HÀM TRANG TRÍ ===
    void drawMovingGrid(SDL_Renderer* renderer);

    UIManager mUI;

    // Biến cho hiệu ứng lưới trôi
    float mGridOffsetX;
    float mGridOffsetY;
};