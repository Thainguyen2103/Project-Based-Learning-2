#pragma once
#include "GameState.h"
#include "UIManager.h"
#include "Constants.h"
#include <vector>
#include <string>

struct RainDrop {
    float x, y;
    float speed;
    int length;
    int width;
    int alpha;
    SDL_Color color;
};

class CreditsState : public GameState {
public:
    CreditsState(Game* game);
    virtual ~CreditsState() {}

    virtual void handleEvents(SDL_Event& e) override;
    virtual void update() override;
    virtual void draw(SDL_Renderer* renderer) override;

private:
    void initButtons();

    // === HIỆU ỨNG MƯA ===
    void initRain();
    void updateRain();
    void drawRain(SDL_Renderer* renderer);

    // === HIỆU ỨNG ĐÁNH MÁY (MỚI) ===
    void initTypewriter(); // Chuẩn bị nội dung
    void updateTypewriter(); // Tính toán chữ hiện ra

    // Hàm hỗ trợ lấy độ dài byte của ký tự UTF-8 (để không bị lỗi font)
    int getUTF8CharLength(unsigned char c);

    UIManager mUI;
    std::vector<RainDrop> mRain;

    // Biến cho hiệu ứng đánh máy
    std::vector<std::string> mFullTextLines;    // Nội dung đầy đủ của từng dòng
    std::vector<std::string> mCurrentTextLines; // Nội dung đang hiện ra
    int mCurrentLineIndex; // Đang gõ dòng nào
    int mCurrentCharIndex; // Đang gõ ký tự nào trong dòng
    int mTypingTimer;      // Bộ đếm thời gian
    bool mIsTypingDone;    // Đã gõ xong chưa
};