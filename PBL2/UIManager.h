#pragma once

#include <vector>
#include <string>
#include <map>
#include "Button.h"
#include "ResourceManager.h"

class UIManager {
private:
    std::map<std::string, Button> mButtons;
    ResourceManager* mResManager;
public:
    UIManager();
    void init(ResourceManager* resManager);

    void addButton(const std::string& key, const SDL_Rect& rect, const std::string& textKey,
        SDL_Color color, SDL_Color highlightColor, SDL_Color textColor = TEXT_WHITE);

    std::string handleEvents(SDL_Event& e);

    void updateButtonTexts();

    void draw(SDL_Renderer* renderer);

    Button* getButton(const std::string& key);
};