#pragma once

#include <SDL_ttf.h>
#include "Constants.h"
#include <algorithm> // Cho std::max

class Button {
private:
    SDL_Rect mRect;
    std::string mTextKey;
    std::string mText;
    SDL_Color mColor;
    SDL_Color mHighlightColor;
    SDL_Color mTextColor;

    bool mIsPressed;

    // Hàm helper: Làm tối màu đi để vẽ cạnh 3D
    SDL_Color darkenColor(SDL_Color color, float factor) {
        return {
            (Uint8)(color.r * factor),
            (Uint8)(color.g * factor),
            (Uint8)(color.b * factor),
            color.a
        };
    }

public:
    Button();
    Button(SDL_Rect rect, std::string textKey, std::string text, SDL_Color color, SDL_Color highlightColor, SDL_Color textColor = TEXT_WHITE);

    void draw(SDL_Renderer* renderer, TTF_Font* font, bool highlighted = false);
    bool isClicked(int x, int y) const;

    void setText(const std::string& text);
    void setTextKey(const std::string& key);
    const std::string& getTextKey() const;
    const SDL_Rect& getRect() const;

    void setPressed(bool pressed) { mIsPressed = pressed; }
    bool isPressed() const { return mIsPressed; }
};