#include "UIManager.h"
#include <SDL_mixer.h> 

UIManager::UIManager() : mResManager(nullptr) {}

void UIManager::init(ResourceManager* resManager) {
    mResManager = resManager;
}

void UIManager::addButton(const std::string& key, const SDL_Rect& rect, const std::string& textKey,
    SDL_Color color, SDL_Color highlightColor, SDL_Color textColor) {
    if (!mResManager) {
        return;
    }
    std::string text = mResManager->getText(textKey);
    mButtons[key] = Button(rect, textKey, text, color, highlightColor, textColor);
}

std::string UIManager::handleEvents(SDL_Event& e) {
    // 1. KHI NHẤN CHUỘT XUỐNG (MOUSE DOWN) -> Nút lún xuống
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        for (auto& pair : mButtons) {
            Button& button = pair.second;
            if (button.isClicked(e.button.x, e.button.y)) {
                button.setPressed(true); // Kích hoạt hiệu ứng 3D lún xuống

                // Phát âm thanh tách tách cho sướng tai
                if (mResManager) {
                    Mix_Chunk* sfx = mResManager->getClickBoxSound();
                    if (sfx) Mix_PlayChannel(-1, sfx, 0);
                }
                return "";
            }
        }
    }
    // 2. KHI THẢ CHUỘT RA (MOUSE UP) -> Thực hiện hành động
    else if (e.type == SDL_MOUSEBUTTONUP) {
        std::string triggeredAction = "";

        for (auto& pair : mButtons) {
            const std::string& key = pair.first;
            Button& button = pair.second;

            // Nếu nút đang bị nhấn VÀ chuột vẫn nằm trong nút khi thả ra
            if (button.isPressed() && button.isClicked(e.button.x, e.button.y)) {
                triggeredAction = key; // Xác nhận hành động
            }

            // Reset trạng thái nút (nảy lên lại)
            button.setPressed(false);
        }
        return triggeredAction;
    }

    return "";
}

void UIManager::updateButtonTexts() {
    if (!mResManager) return;

    for (auto& pair : mButtons) {
        Button& button = pair.second;
        button.setText(mResManager->getText(button.getTextKey()));
    }
}

void UIManager::draw(SDL_Renderer* renderer) {
    if (!mResManager) return;

    TTF_Font* font = mResManager->getFontUI();
    if (!font) return;

    // === ĐÃ SỬA: BỎ LOGIC CHECK CHUỘT ĐỂ ĐỔI MÀU ===
    for (auto& pair : mButtons) {
        Button& button = pair.second;
        // Luôn truyền false vào tham số 'highlighted'
        // Nút sẽ không đổi màu khi di chuột nữa
        button.draw(renderer, font, false);
    }
}

Button* UIManager::getButton(const std::string& key) {
    if (mButtons.count(key)) {
        return &mButtons[key];
    }
    return nullptr;
}