#include "SettingsState.h"
#include "Game.h"
#include "MainMenuState.h" 
#include "Constants.h" 

SettingsState::SettingsState(Game* game)
    : GameState(game), mGridOffsetX(0), mGridOffsetY(0) // Khởi tạo
{
    mUI.init(mGame->getResources());
    initButtons();
}

void SettingsState::initButtons() {
    int sBtnW = 150, sBtnH = 50, sPad = 20;

    int y_pos = 165;
    int y_increment = 95;

    // --- 1. Algorithm ---
    int totalW_algo = 4 * sBtnW + 3 * sPad;
    int startX_algo = (SCREEN_WIDTH - totalW_algo) / 2 + 100;
    mUI.addButton("algo_bfs", { startX_algo, y_pos, sBtnW, sBtnH }, "BFS", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("algo_dfs", { startX_algo + sBtnW + sPad, y_pos, sBtnW, sBtnH }, "DFS", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("algo_scan", { startX_algo + 2 * (sBtnW + sPad), y_pos, sBtnW, sBtnH }, "Scanline", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("algo_uf", { startX_algo + 3 * (sBtnW + sPad), y_pos, sBtnW, sBtnH }, "UnionFind", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);

    y_pos += y_increment;

    // --- 2. Language ---
    int langBtnW = 100;
    int totalW_lang = 4 * langBtnW + 3 * sPad;
    int startX_lang = (SCREEN_WIDTH - totalW_lang) / 2 + 100;

    mUI.addButton("lang_vi", { startX_lang, y_pos, langBtnW, sBtnH }, "settings_lang_vi", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("lang_en", { startX_lang + langBtnW + sPad, y_pos, langBtnW, sBtnH }, "settings_lang_en", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("lang_cn", { startX_lang + 2 * (langBtnW + sPad), y_pos, langBtnW, sBtnH }, "settings_lang_cn", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("lang_jp", { startX_lang + 3 * (langBtnW + sPad), y_pos, langBtnW, sBtnH }, "settings_lang_jp", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);

    y_pos += y_increment;

    // --- 3. Game Difficulty (Moves) ---
    int totalW_gamediff = 3 * sBtnW + 2 * sPad;
    int startX_gamediff = (SCREEN_WIDTH - totalW_gamediff) / 2 + 100;
    mUI.addButton("diff_easy", { startX_gamediff, y_pos, sBtnW, sBtnH }, "diff_easy", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("diff_normal", { startX_gamediff + sBtnW + sPad, y_pos, sBtnW, sBtnH }, "diff_normal", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("diff_hard", { startX_gamediff + 2 * (sBtnW + sPad), y_pos, sBtnW, sBtnH }, "diff_hard", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);

    y_pos += y_increment;

    // --- 4. Color Difficulty (Số màu) ---
    int totalW_diff = 3 * sBtnW + 2 * sPad;
    int startX_diff = (SCREEN_WIDTH - totalW_diff) / 2 + 100;
    mUI.addButton("color_diff_6", { startX_diff, y_pos, sBtnW, sBtnH }, "settings_diff_easy", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("color_diff_7", { startX_diff + sBtnW + sPad, y_pos, sBtnW, sBtnH }, "settings_diff_medium", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("color_diff_8", { startX_diff + 2 * (sBtnW + sPad), y_pos, sBtnW, sBtnH }, "settings_diff_hard", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);

    y_pos += y_increment;

    // --- 5. Fill Direction ---
    int totalW_fill = 2 * sBtnW + 1 * sPad;
    int startX_fill = (SCREEN_WIDTH - totalW_fill) / 2 + 100;
    mUI.addButton("fill_4", { startX_fill, y_pos, sBtnW, sBtnH }, "settings_fill_4", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);
    mUI.addButton("fill_8", { startX_fill + sBtnW + sPad, y_pos, sBtnW, sBtnH }, "settings_fill_8", BUTTON_WHITE, BUTTON_HIGHLIGHT, TEXT_BLACK);

    // --- 6. Back Button ---
    y_pos += 80;
    mUI.addButton("back", { SCREEN_WIDTH / 2 - 100, y_pos, 200, 60 }, "settings_back", BUTTON_GRAY, BUTTON_HIGHLIGHT);
    updateButtonTexts();
}

void SettingsState::updateButtonTexts() {
    mUI.updateButtonTexts();
    mUI.getButton("color_diff_6")->setText(mGame->getResources()->getText("settings_diff_easy") + " (6)");
    mUI.getButton("color_diff_7")->setText(mGame->getResources()->getText("settings_diff_medium") + " (7)");
    mUI.getButton("color_diff_8")->setText(mGame->getResources()->getText("settings_diff_hard") + " (8)");
}

void SettingsState::handleEvents(SDL_Event& e) {
    std::string action = mUI.handleEvents(e);

    if (action == "back") {
        mGame->changeState(new MainMenuState(mGame));
    }
    else if (action == "algo_bfs") mGame->setAlgorithm(ALGO_BFS);
    else if (action == "algo_dfs") mGame->setAlgorithm(ALGO_DFS);
    else if (action == "algo_scan") mGame->setAlgorithm(ALGO_SCANLINE);
    else if (action == "algo_uf") mGame->setAlgorithm(ALGO_UNION_FIND);

    else if (action == "lang_vi") { mGame->getResources()->setLanguage(LANG_VI); updateButtonTexts(); }
    else if (action == "lang_en") { mGame->getResources()->setLanguage(LANG_EN); updateButtonTexts(); }
    else if (action == "lang_cn") { mGame->getResources()->setLanguage(LANG_CN); updateButtonTexts(); }
    else if (action == "lang_jp") { mGame->getResources()->setLanguage(LANG_JP); updateButtonTexts(); }

    else if (action == "diff_easy") mGame->setGameDifficulty(DIFF_EASY);
    else if (action == "diff_normal") mGame->setGameDifficulty(DIFF_NORMAL);
    else if (action == "diff_hard") mGame->setGameDifficulty(DIFF_HARD);
    else if (action == "color_diff_6") mGame->setNumColors(6);
    else if (action == "color_diff_7") mGame->setNumColors(7);
    else if (action == "color_diff_8") mGame->setNumColors(8);
    else if (action == "fill_4") mGame->setFillDirection(DIR_FOUR);
    else if (action == "fill_8") mGame->setFillDirection(DIR_EIGHT);
}

void SettingsState::update() {
    // === CẬP NHẬT HIỆU ỨNG LƯỚI ===
    mGridOffsetX += 0.5f; // Tốc độ trôi ngang
    mGridOffsetY += 0.5f; // Tốc độ trôi dọc

    // Reset khi đi qua 1 ô (để tạo cảm giác vô tận)
    int gridSize = 40;
    if (mGridOffsetX >= gridSize) mGridOffsetX = 0;
    if (mGridOffsetY >= gridSize) mGridOffsetY = 0;
}

// === HÀM VẼ LƯỚI TRANG TRÍ ===
void SettingsState::drawMovingGrid(SDL_Renderer* renderer) {
    int gridSize = 40; // Kích thước ô lưới

    // Màu lưới: Xanh lơ rất nhạt (Cyan)
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 30); // Alpha thấp để mờ ảo

    // Vẽ đường dọc
    for (int x = -gridSize; x < SCREEN_WIDTH; x += gridSize) {
        int drawX = x + (int)mGridOffsetX;
        SDL_RenderDrawLine(renderer, drawX, 0, drawX, SCREEN_HEIGHT);
    }

    // Vẽ đường ngang
    for (int y = -gridSize; y < SCREEN_HEIGHT; y += gridSize) {
        int drawY = y + (int)mGridOffsetY;
        SDL_RenderDrawLine(renderer, 0, drawY, SCREEN_WIDTH, drawY);
    }
}

void SettingsState::draw(SDL_Renderer* renderer) {
    auto res = mGame->getResources();
    auto font = res->getFontUI();
    auto fontTitle = res->getFontTitle();
    auto fontSmall = res->getFontSmall();

    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, 255);
    SDL_RenderClear(renderer);

    // === VẼ NỀN LƯỚI TRƯỚC ===
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    drawMovingGrid(renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Vẽ tiêu đề chính
    drawText(renderer, res->getText("settings_title"), SCREEN_WIDTH / 2, 60, fontTitle, PALETTE[2]);

    int sBtnW = 150, sPad = 20;
    int y_offset = 165 + 50 / 2;
    int y_increment = 95;

    // --- 1. Algorithm ---
    int totalW_algo = 4 * sBtnW + 3 * sPad;
    int startX_algo = (SCREEN_WIDTH - totalW_algo) / 2 + 100;
    int labelX = startX_algo - 20;
    drawTextRight(renderer, res->getText("settings_algorithm"), labelX, y_offset, font, TEXT_WHITE);
    AlgorithmType currentAlgo = mGame->getAlgorithm();
    mUI.getButton("algo_bfs")->draw(renderer, font, currentAlgo == ALGO_BFS);
    mUI.getButton("algo_dfs")->draw(renderer, font, currentAlgo == ALGO_DFS);
    mUI.getButton("algo_scan")->draw(renderer, font, currentAlgo == ALGO_SCANLINE);
    mUI.getButton("algo_uf")->draw(renderer, font, currentAlgo == ALGO_UNION_FIND);
    y_offset += y_increment;

    // --- 2. Language ---
    int langBtnW = 100;
    int totalW_lang = 4 * langBtnW + 3 * sPad;
    int startX_lang = (SCREEN_WIDTH - totalW_lang) / 2 + 100;
    labelX = startX_lang - 20;
    drawTextRight(renderer, res->getText("settings_language"), labelX, y_offset, font, TEXT_WHITE);
    Language currentLang = res->getLanguage();
    mUI.getButton("lang_vi")->draw(renderer, fontSmall, currentLang == LANG_VI);
    mUI.getButton("lang_en")->draw(renderer, fontSmall, currentLang == LANG_EN);
    mUI.getButton("lang_cn")->draw(renderer, fontSmall, currentLang == LANG_CN);
    mUI.getButton("lang_jp")->draw(renderer, fontSmall, currentLang == LANG_JP);
    y_offset += y_increment;

    // --- 3. Game Difficulty ---
    int totalW_gamediff = 3 * sBtnW + 2 * sPad;
    int startX_gamediff = (SCREEN_WIDTH - totalW_gamediff) / 2 + 100;
    labelX = startX_gamediff - 20;
    drawTextRight(renderer, res->getText("settings_game_difficulty"), labelX, y_offset, font, TEXT_WHITE);
    GameDifficulty currentDiff = mGame->getGameDifficulty();
    mUI.getButton("diff_easy")->draw(renderer, fontSmall, currentDiff == DIFF_EASY);
    mUI.getButton("diff_normal")->draw(renderer, fontSmall, currentDiff == DIFF_NORMAL);
    mUI.getButton("diff_hard")->draw(renderer, fontSmall, currentDiff == DIFF_HARD);
    y_offset += y_increment;

    // --- 4. Color Difficulty ---
    int totalW_diff = 3 * sBtnW + 2 * sPad;
    int startX_diff = (SCREEN_WIDTH - totalW_diff) / 2 + 100;
    labelX = startX_diff - 20;
    drawTextRight(renderer, res->getText("settings_difficulty"), labelX, y_offset, font, TEXT_WHITE);
    int currentColors = mGame->getNumColors();
    mUI.getButton("color_diff_6")->draw(renderer, font, currentColors == 6);
    mUI.getButton("color_diff_7")->draw(renderer, font, currentColors == 7);
    mUI.getButton("color_diff_8")->draw(renderer, font, currentColors == 8);
    y_offset += y_increment;

    // --- 5. Fill Direction ---
    int totalW_fill = 2 * sBtnW + 1 * sPad;
    int startX_fill = (SCREEN_WIDTH - totalW_fill) / 2 + 100;
    labelX = startX_fill - 20;
    drawTextRight(renderer, res->getText("settings_fill_dir"), labelX, y_offset, font, TEXT_WHITE);
    FillDirection currentFill = mGame->getFillDirection();
    mUI.getButton("fill_4")->draw(renderer, font, currentFill == DIR_FOUR);
    mUI.getButton("fill_8")->draw(renderer, font, currentFill == DIR_EIGHT);

    // --- 6. Back Button ---
    mUI.getButton("back")->draw(renderer, font);
}