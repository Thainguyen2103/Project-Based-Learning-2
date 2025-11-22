#include "Game.h"
#include "GameState.h"
#include "MainMenuState.h" 
#include "PlayingState.h" 
#include "ResourceManager.h"
#include "Constants.h"
#include <iostream>
#include <fstream>
#include <cstdio> 
#include <sstream>
#include <SDL_mixer.h>

Game::Game()
    : mIsRunning(true), mWindow(nullptr), mRenderer(nullptr),
    mSaveFileExists(false), mHighestLevel(1),
    mCurrentAlgorithm(ALGO_BFS), mFillDirection(DIR_FOUR), mNumColors(6),
    mGameDifficulty(DIFF_NORMAL)
{
}

Game::~Game() {
    close();
}


bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        std::cerr << "SDL hoặc TTF không thể khởi tạo! SDL_Error: " << SDL_GetError() << ", TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0) {
        std::cerr << "SDL_mixer warning: Mix_Init failed. Mix_Error: " << Mix_GetError() << "\n";
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer error: Mix_OpenAudio failed! Mix_Error: " << Mix_GetError() << "\n";
    }

    mWindow = SDL_CreateWindow("Fill Color Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!mWindow) {
        std::cerr << "Không thể tạo cửa sổ! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer) {
        std::cerr << "Không thể tạo renderer! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!mResourceManager.loadFonts("Baloo2-Bold.ttf")) {
        return false;
    }

    mResourceManager.loadGameSounds();

    if (mResourceManager.loadMusic("background_music.mp3")) {
        if (Mix_PlayMusic(mResourceManager.getBackgroundMusic(), -1) == -1) {
            std::cerr << "Mix_PlayMusic Error: " << Mix_GetError() << std::endl;
        }
    }
    else {
        std::cerr << "Warning: Could not load background_music.mp3" << std::endl;
    }

    loadSettings();
    mResourceManager.loadTranslations();

    checkForSaveFile();
    loadHighestLevel();

    pushState(new MainMenuState(this));

    return true;
}

void Game::run() {
    while (mIsRunning) {
        handleEvents();
        update();
        draw();

        SDL_Delay(10);
    }
}

void Game::close() {
    saveSettings();
    saveHighestLevel();

    while (!mStates.empty()) {
        delete mStates.back();
        mStates.pop_back();
    }

    mResourceManager.freeFonts();

    mResourceManager.freeMusic();
    mResourceManager.freeGameSounds();

    Mix_CloseAudio();
    Mix_Quit();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    mRenderer = nullptr;
    mWindow = nullptr;

    TTF_Quit();
    SDL_Quit();
}


float Game::getDifficultyModifier() const {
    switch (mGameDifficulty) {
    case DIFF_EASY:   return 2.5f;
    case DIFF_HARD:   return 3.5f;
    case DIFF_NORMAL:
    default:          return 3.0f;
    }
}

void Game::saveSettings() {
    std::ofstream settingsFile("settings.ini");
    if (!settingsFile.is_open()) return;
    settingsFile << "Language=" << mResourceManager.getLanguage() << std::endl;
    settingsFile << "Algorithm=" << mCurrentAlgorithm << std::endl;
    settingsFile << "NumColors=" << mNumColors << std::endl;
    settingsFile << "FillDirection=" << mFillDirection << std::endl;
    settingsFile << "GameDifficulty=" << mGameDifficulty << std::endl;
    settingsFile.close();
}

void Game::loadSettings() {
    std::ifstream settingsFile("settings.ini");
    if (!settingsFile.is_open()) {
        saveSettings();
        return;
    }
    std::string line;
    while (std::getline(settingsFile, line)) {
        std::stringstream ss(line);
        std::string key;
        int value;
        if (std::getline(ss, key, '=') && (ss >> value)) {
            if (key == "Language") mResourceManager.setLanguage((Language)value);
            else if (key == "Algorithm") mCurrentAlgorithm = (AlgorithmType)value;
            else if (key == "NumColors") mNumColors = value;
            else if (key == "FillDirection") mFillDirection = (FillDirection)value;
            else if (key == "GameDifficulty") mGameDifficulty = (GameDifficulty)value;
        }
    }
    settingsFile.close();
}


void Game::pushState(GameState* state) {
    mStates.push_back(state);
}

void Game::popState() {
    if (!mStates.empty()) {
        delete mStates.back();
        mStates.pop_back();
    }
    if (mStates.empty()) {
        requestQuit();
    }
}

void Game::changeState(GameState* state) {
    if (!mStates.empty()) {
        delete mStates.back();
        mStates.pop_back();
    }
    pushState(state);
}

void Game::popStates(int count) {
    for (int i = 0; i < count; ++i) {
        if (mStates.empty()) break;
        delete mStates.back();
        mStates.pop_back();
    }
    if (mStates.empty()) {
        requestQuit();
    }
}

void Game::popAndChange(GameState* state) {
    if (!mStates.empty()) {
        delete mStates.back();
        mStates.pop_back();
    }
    if (!mStates.empty()) {
        delete mStates.back();
        mStates.pop_back();
    }
    pushState(state);
}

void Game::popAndRetry() {
    if (!mStates.empty()) {
        delete mStates.back();
        mStates.pop_back();
    }
    if (!mStates.empty()) {
        PlayingState* pState = dynamic_cast<PlayingState*>(mStates.back());
        if (pState) {
            pState->retryCurrentLevel();
        }
    }
}


GameState* Game::getTopState() {
    return mStates.empty() ? nullptr : mStates.back();
}


void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            requestQuit();
        }
        if (!mStates.empty()) {
            mStates.back()->handleEvents(e);
        }
    }
}

void Game::update() {
    if (!mStates.empty()) {
        mStates.back()->update();
    }
}

void Game::draw() {
    SDL_SetRenderDrawColor(mRenderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, 255);
    SDL_RenderClear(mRenderer);

    for (GameState* state : mStates) {
        state->draw(mRenderer);
    }

    SDL_RenderPresent(mRenderer);
}

void Game::checkForSaveFile() {
    std::ifstream saveFile("savegame.txt");
    mSaveFileExists = saveFile.good();
    saveFile.close();
}

void Game::deleteSaveFile() {
    remove("savegame.txt");
    mSaveFileExists = false;
}

void Game::loadHighestLevel() {
    std::ifstream levelFile("level_data.txt");
    if (levelFile.is_open()) {
        levelFile >> mHighestLevel;
        levelFile.close();
    }
    else {
        mHighestLevel = 1;
        saveHighestLevel();
    }
}

void Game::saveHighestLevel() {
    std::ofstream levelFile("level_data.txt");
    if (levelFile.is_open()) {
        levelFile << mHighestLevel;
        levelFile.close();
    }
}

void Game::setHighestLevel(int level) {
    mHighestLevel = level;
}