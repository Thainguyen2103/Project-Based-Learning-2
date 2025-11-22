#pragma once
#include "GameState.h"
#include "UIManager.h"
#include "Constants.h"
#include <vector>

// Cấu trúc cho các ô vuông bay ở nền
struct FloatingBlock {
    float x, y;
    float speed;
    int size;
    SDL_Color color;
    int alpha;
};

class MainMenuState : public GameState {
public:
    MainMenuState(Game* game);
    virtual ~MainMenuState() {}

    virtual void handleEvents(SDL_Event& e) override;
    virtual void update() override;
    virtual void draw(SDL_Renderer* renderer) override;

private:
    void initButtons();
    void loadAndPushPlayingState(bool isContinue);

    // === HÀM TRANG TRÍ ===
    void initParticles();
    void updateParticles();
    void drawParticles(SDL_Renderer* renderer);

    // Hàm vẽ tiêu đề hiệu ứng Glitch
    void drawGlitchTitle(SDL_Renderer* renderer);

    UIManager mUI;
    std::vector<FloatingBlock> mParticles;

    // === BIẾN CHO HIỆU ỨNG GLITCH ===
    int mGlitchTimer;
    bool mIsGlitching;
    int mGlitchOffsetX;
    int mGlitchOffsetY;
};