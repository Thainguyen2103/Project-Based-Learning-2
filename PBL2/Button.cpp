#include "Button.h"

Button::Button()
    : mRect({ 0, 0, 0, 0 }), mTextKey(""), mText(""), mColor({ 0,0,0,0 }),
    mHighlightColor({ 0,0,0,0 }), mTextColor(TEXT_WHITE), mIsPressed(false)
{
}

Button::Button(SDL_Rect rect, std::string textKey, std::string text, SDL_Color color, SDL_Color highlightColor, SDL_Color textColor)
    : mRect(rect), mTextKey(textKey), mText(text), mColor(color),
    mHighlightColor(highlightColor), mTextColor(textColor), mIsPressed(false)
{
}

void Button::draw(SDL_Renderer* renderer, TTF_Font* font, bool highlighted) {
    // Độ dày của khối 3D (Pixel)
    const int THICKNESS = 8;

    SDL_Color baseColor = highlighted ? mHighlightColor : mColor;

    // Tạo màu cho phần cạnh (Tối hơn màu chính 30%)
    SDL_Color sideColor = darkenColor(baseColor, 0.6f);

    SDL_Rect topFace = mRect; // Mặt trên (phần sáng)

    if (mIsPressed) {
        // === TRẠNG THÁI NHẤN ===
        // Nút lún xuống hoàn toàn:
        // 1. Mặt trên dịch xuống đúng bằng độ dày
        topFace.y += THICKNESS;

        // 2. Không vẽ phần cạnh (hoặc vẽ rất mỏng nếu muốn)
        // -> Vẽ mặt trên
        SDL_SetRenderDrawColor(renderer, baseColor.r, baseColor.g, baseColor.b, 255);
        SDL_RenderFillRect(renderer, &topFace);
    }
    else {
        // === TRẠNG THÁI THƯỜNG ===

        // 1. Vẽ phần CẠNH (Khối 3D ở dưới)
        SDL_Rect sideFace = mRect;
        sideFace.y += THICKNESS; // Cạnh nằm thấp hơn mặt trên
        // Vẽ màu tối
        SDL_SetRenderDrawColor(renderer, sideColor.r, sideColor.g, sideColor.b, 255);
        SDL_RenderFillRect(renderer, &sideFace);

        // 2. Vẽ mặt TRÊN (Ở vị trí gốc)
        SDL_SetRenderDrawColor(renderer, baseColor.r, baseColor.g, baseColor.b, 255);
        SDL_RenderFillRect(renderer, &topFace);
    }

    // (Tùy chọn) Vẽ viền đen mỏng bao quanh để nhìn sắc nét hơn
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 50);
    SDL_RenderDrawRect(renderer, &topFace);

    // 3. VẼ CHỮ
    // Chữ phải đi theo mặt trên (topFace)
    drawText(renderer, mText, topFace.x + topFace.w / 2, topFace.y + topFace.h / 2, font, mTextColor);
}

bool Button::isClicked(int x, int y) const {
    SDL_Point mousePoint = { x, y };
    // Vùng bấm chuột vẫn là hình chữ nhật gốc (khi chưa nhấn)
    // Hoặc bạn có thể mở rộng vùng bấm xuống dưới để bao gồm cả phần cạnh 3D nếu muốn dễ bấm hơn:
    SDL_Rect hitBox = mRect;
    hitBox.h += 8; // Cộng thêm độ dày vào vùng bấm
    return SDL_PointInRect(&mousePoint, &hitBox);
}

void Button::setText(const std::string& text) {
    mText = text;
}

void Button::setTextKey(const std::string& key) {
    mTextKey = key;
}

const std::string& Button::getTextKey() const {
    return mTextKey;
}

const SDL_Rect& Button::getRect() const {
    return mRect;
}