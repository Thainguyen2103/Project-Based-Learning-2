#include "CreditsState.h"
#include "Game.h"
#include <cstdlib> 

CreditsState::CreditsState(Game* game) : GameState(game) {
    mUI.init(mGame->getResources());
    initButtons();
    initRain();

    // === KHỞI TẠO HIỆU ỨNG ĐÁNH MÁY ===
    initTypewriter();
}

void CreditsState::initButtons() {
    mUI.addButton("back", { SCREEN_WIDTH / 2 - 100, 620, 200, 60 }, "settings_back", BUTTON_GRAY, BUTTON_HIGHLIGHT);
    mUI.updateButtonTexts();
}

// === 1. CHUẨN BỊ NỘI DUNG ===
void CreditsState::initTypewriter() {
    auto res = mGame->getResources();

    // Lấy toàn bộ nội dung cần hiển thị vào mảng
    mFullTextLines.clear();
    mFullTextLines.push_back(res->getText("credits_title"));
    mFullTextLines.push_back(res->getText("credits_student_1"));
    mFullTextLines.push_back(res->getText("credits_student_2"));
    mFullTextLines.push_back(res->getText("credits_instructor"));

    // Khởi tạo mảng hiển thị rỗng
    mCurrentTextLines.clear();
    for (size_t i = 0; i < mFullTextLines.size(); ++i) {
        mCurrentTextLines.push_back(""); // Ban đầu chưa có chữ nào
    }

    mCurrentLineIndex = 0;
    mCurrentCharIndex = 0;
    mTypingTimer = 0;
    mIsTypingDone = false;
}

// Hàm helper: Xác định 1 ký tự UTF-8 chiếm bao nhiêu byte
// (Để không cắt đôi chữ cái có dấu, gây lỗi font)
int CreditsState::getUTF8CharLength(unsigned char c) {
    if ((c & 0x80) == 0) return 1;        // ASCII (Tiếng Anh)
    if ((c & 0xE0) == 0xC0) return 2;     // 2 bytes
    if ((c & 0xF0) == 0xE0) return 3;     // 3 bytes (Tiếng Việt, Trung, Nhật thường ở đây)
    if ((c & 0xF8) == 0xF0) return 4;     // 4 bytes
    return 1; // Fallback
}

// === 2. CẬP NHẬT CHỮ (GÕ MÁY) ===
void CreditsState::updateTypewriter() {
    if (mIsTypingDone) return;

    // Tốc độ gõ: Cứ 2 frame thì hiện 1 chữ (bạn có thể chỉnh số này)
    // Số càng nhỏ gõ càng nhanh
    mTypingTimer++;
    if (mTypingTimer < 2) return;
    mTypingTimer = 0;

    // Lấy dòng hiện tại đang gõ
    std::string& fullLine = mFullTextLines[mCurrentLineIndex];

    if (mCurrentCharIndex < fullLine.length()) {
        // Lấy ký tự tiếp theo (xử lý UTF-8)
        unsigned char c = (unsigned char)fullLine[mCurrentCharIndex];
        int charLen = getUTF8CharLength(c);

        // Thêm cả cụm byte của ký tự đó vào chuỗi hiển thị
        mCurrentTextLines[mCurrentLineIndex] += fullLine.substr(mCurrentCharIndex, charLen);

        // Nhảy qua ký tự đó
        mCurrentCharIndex += charLen;

        // (Tùy chọn) Thêm âm thanh lạch cạch ở đây nếu muốn: Mix_PlayChannel(...)
    }
    else {
        // Đã gõ xong dòng này, chuyển sang dòng tiếp theo
        mCurrentLineIndex++;
        mCurrentCharIndex = 0;

        if (mCurrentLineIndex >= mFullTextLines.size()) {
            mIsTypingDone = true; // Hoàn tất toàn bộ
        }
    }
}

// ... (initRain, updateRain giữ nguyên) ...
void CreditsState::initRain() {
    int numDrops = 100;
    for (int i = 0; i < numDrops; ++i) {
        RainDrop d;
        d.x = (float)(rand() % SCREEN_WIDTH); d.y = (float)(rand() % SCREEN_HEIGHT);
        d.speed = 2.0f + (rand() % 50) / 10.0f; d.length = 10 + rand() % 30;
        d.width = 2 + rand() % 3; d.alpha = 50 + rand() % 150;
        if (rand() % 2 == 0) d.color = { 0, 255, 255, 255 }; else d.color = { 0, 255, 100, 255 };
        mRain.push_back(d);
    }
}

void CreditsState::updateRain() {
    for (auto& d : mRain) {
        d.y += d.speed;
        if (d.y > SCREEN_HEIGHT) {
            d.y = -(float)d.length; d.x = (float)(rand() % SCREEN_WIDTH);
        }
    }
}

void CreditsState::handleEvents(SDL_Event& e) {
    std::string action = mUI.handleEvents(e);
    if (action == "back") {
        mGame->popState();
    }
    // Nếu người dùng click chuột khi đang gõ, hiện toàn bộ ngay lập tức (Skip)
    else if (e.type == SDL_MOUSEBUTTONDOWN && !mIsTypingDone) {
        for (size_t i = 0; i < mFullTextLines.size(); ++i) {
            mCurrentTextLines[i] = mFullTextLines[i];
        }
        mIsTypingDone = true;
    }
}

void CreditsState::update() {
    updateRain();
    updateTypewriter(); // <--- CẬP NHẬT HIỆU ỨNG
}

void CreditsState::draw(SDL_Renderer* renderer) {
    auto res = mGame->getResources();
    auto fontTitle = res->getFontTitle();
    auto fontUI = res->getFontUI();

    SDL_SetRenderDrawColor(renderer, 10, 15, 20, 255);
    SDL_RenderClear(renderer);

    // 1. Vẽ mưa
    drawRain(renderer);

    // 2. Vẽ khung
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect infoBox = { SCREEN_WIDTH / 2 - 400, 200, 800, 350 };
    SDL_RenderFillRect(renderer, &infoBox);
    SDL_SetRenderDrawColor(renderer, PALETTE[3].r, PALETTE[3].g, PALETTE[3].b, 255);
    SDL_RenderDrawRect(renderer, &infoBox);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // 3. === VẼ NỘI DUNG DỰA TRÊN BIẾN ĐÃ GÕ ===

    // Tiêu đề (Dòng 0)
    if (!mCurrentTextLines[0].empty()) {
        // Bóng đổ
        drawText(renderer, mCurrentTextLines[0], SCREEN_WIDTH / 2 + 2, 80 + 2, fontTitle, { 0,0,0,150 });
        // Chữ chính
        drawText(renderer, mCurrentTextLines[0], SCREEN_WIDTH / 2, 80, fontTitle, PALETTE[3]);
    }

    // Nội dung chi tiết (Dòng 1, 2, 3)
    int y_pos = 250;

    // SV 1
    if (!mCurrentTextLines[1].empty())
        drawText(renderer, mCurrentTextLines[1], SCREEN_WIDTH / 2, y_pos, fontUI, TEXT_WHITE);
    y_pos += 60;

    // SV 2
    if (!mCurrentTextLines[2].empty())
        drawText(renderer, mCurrentTextLines[2], SCREEN_WIDTH / 2, y_pos, fontUI, TEXT_WHITE);
    y_pos += 100;

    // GVHD
    if (!mCurrentTextLines[3].empty())
        drawText(renderer, mCurrentTextLines[3], SCREEN_WIDTH / 2, y_pos, fontUI, PALETTE[5]);

    // 4. Vẽ nút Back
    mUI.draw(renderer);
}

void CreditsState::drawRain(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (const auto& d : mRain) {
        SDL_SetRenderDrawColor(renderer, d.color.r, d.color.g, d.color.b, d.alpha);
        SDL_Rect r = { (int)d.x, (int)d.y, d.width, d.length };
        SDL_RenderFillRect(renderer, &r);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}