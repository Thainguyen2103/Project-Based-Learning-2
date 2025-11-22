#pragma once
#include "GameState.h"
#include "UIManager.h"
#include "Constants.h" 
#include <vector>
#include <string>

struct InstructionPage {
    std::string titleKey;
    std::vector<std::string> contentKeys;
    IllustrationType illustType;
};

class InstructionState : public GameState {
public:
    InstructionState(Game* game);
    virtual ~InstructionState();

    virtual void handleEvents(SDL_Event& e) override;
    virtual void update() override;
    virtual void draw(SDL_Renderer* renderer) override;

private:
    void initButtons();
    void initPages();

    // === HÀM TRANG TRÍ ===
    void drawPageIndicator(SDL_Renderer* renderer); // Vẽ các chấm tròn

    // Các hàm vẽ minh họa (Giữ nguyên)
    void drawIllustGoal(SDL_Renderer* r, int x, int y, int w, int h);
    void drawIllustStart(SDL_Renderer* r, int x, int y, int w, int h);
    void drawIllustMechanics(SDL_Renderer* r, int x, int y, int w, int h);
    void drawIllustStrategy(SDL_Renderer* r, int x, int y, int w, int h);
    void drawIllustMoves(SDL_Renderer* r, int x, int y, int w, int h);
    void drawIllustFeatures(SDL_Renderer* r, int x, int y, int w, int h);
    void drawIllustEvent(SDL_Renderer* r, int x, int y, int w, int h);
    void drawIllustAI(SDL_Renderer* r, int x, int y, int w, int h);

    UIManager mUI;
    std::vector<InstructionPage> mPages;
    int mCurrentPageIndex;
};