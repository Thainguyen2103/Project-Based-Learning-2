#pragma once
#include "GameState.h"
#include "UIManager.h"
#include "Constants.h"
#include <vector>

// Cấu trúc cho điểm nối (Node) trong mạng Neural
struct Node {
    float x, y;
    float vx, vy; // Vận tốc
};

class AISetupState : public GameState {
public:
    AISetupState(Game* game);
    virtual ~AISetupState() {}

    virtual void handleEvents(SDL_Event& e) override;
    virtual void update() override;
    virtual void draw(SDL_Renderer* renderer) override;

private:
    void initButtons();
    void confirmCustomInput();
    void drawCustomInputOverlay(SDL_Renderer* renderer);

    // === HÀM TRANG TRÍ ===
    void initNetwork();
    void updateNetwork();
    void drawNetwork(SDL_Renderer* renderer);

    UIManager mUI;
    int mAiGridDim;
    int mAiNumColors;

    bool mShowCustomInput;
    std::string mInputText;

    // Danh sách các điểm
    std::vector<Node> mNodes;
};