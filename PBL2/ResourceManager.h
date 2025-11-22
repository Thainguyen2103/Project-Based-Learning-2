#pragma once

#include <SDL_ttf.h>
#include "Constants.h"
#include <iostream>
#include <map>
#include <string>
#include <vector> // <--- CẦN THƯ VIỆN NÀY
#include <SDL_mixer.h> 

class ResourceManager {
private:
    TTF_Font* mFontTitle;
    TTF_Font* mFontUI;
    TTF_Font* mFontSmall;

    Mix_Music* mBackgroundMusic;

    Mix_Chunk* mWinSound;
    Mix_Chunk* mLoseSound;
    Mix_Chunk* mClickBoxSound; // Dùng cho UI Buttons

    // === THÊM DANH SÁCH ÂM THANH TÔ MÀU ===
    std::vector<Mix_Chunk*> mFillSounds;

    std::map<std::string, std::map<Language, std::string>> mTranslations;
    Language mCurrentLanguage;

public:
    ResourceManager();
    ~ResourceManager();

    bool loadFonts(const std::string& path);
    void freeFonts();
    void loadTranslations();

    bool loadMusic(const std::string& musicPath);
    void freeMusic();

    bool loadGameSounds();
    void freeGameSounds();

    TTF_Font* getFontTitle() const { return mFontTitle; }
    TTF_Font* getFontUI() const { return mFontUI; }
    TTF_Font* getFontSmall() const { return mFontSmall; }
    Mix_Music* getBackgroundMusic() const { return mBackgroundMusic; }

    Mix_Chunk* getWinSound() const { return mWinSound; }
    Mix_Chunk* getLoseSound() const { return mLoseSound; }
    Mix_Chunk* getClickBoxSound() const { return mClickBoxSound; }

    // === THÊM HÀM LẤY ÂM THANH NGẪU NHIÊN ===
    Mix_Chunk* getRandomFillSound() const;

    std::string getText(const std::string& key) const;
    void setLanguage(Language lang);
    Language getLanguage() const { return mCurrentLanguage; }
};