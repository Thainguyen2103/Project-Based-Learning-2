#include "ResourceManager.h"
#include <iostream>
#include <SDL_ttf.h>
#include <SDL_mixer.h> 
#include <cstdlib> // Cho rand()

ResourceManager::ResourceManager()
    : mFontTitle(nullptr), mFontUI(nullptr), mFontSmall(nullptr), mCurrentLanguage(LANG_VI),
    mBackgroundMusic(nullptr),
    mWinSound(nullptr), mLoseSound(nullptr), mClickBoxSound(nullptr)
{
}

ResourceManager::~ResourceManager() {
    freeFonts();
    freeMusic();
    freeGameSounds();
}

bool ResourceManager::loadFonts(const std::string& path) {
    mFontTitle = TTF_OpenFont(path.c_str(), 60);
    mFontUI = TTF_OpenFont(path.c_str(), 28);
    mFontSmall = TTF_OpenFont(path.c_str(), 18);

    if (!mFontTitle || !mFontUI || !mFontSmall) {
        std::cerr << "Loi: Khong the tai font '" << path << "'. Error: " << TTF_GetError() << "\n";
        return false;
    }
    return true;
}

void ResourceManager::freeFonts() {
    if (mFontTitle) { TTF_CloseFont(mFontTitle); mFontTitle = nullptr; }
    if (mFontUI) { TTF_CloseFont(mFontUI); mFontUI = nullptr; }
    if (mFontSmall) { TTF_CloseFont(mFontSmall); mFontSmall = nullptr; }
}

bool ResourceManager::loadMusic(const std::string& musicPath) {
    mBackgroundMusic = Mix_LoadMUS(musicPath.c_str());
    if (mBackgroundMusic == nullptr) {
        std::cerr << "Loi: Khong the tai nhac. Mix_Error: " << Mix_GetError() << "\n";
        return false;
    }
    return true;
}

void ResourceManager::freeMusic() {
    if (mBackgroundMusic) {
        Mix_FreeMusic(mBackgroundMusic);
        mBackgroundMusic = nullptr;
    }
}

// === CẬP NHẬT HÀM TẢI ÂM THANH ===
bool ResourceManager::loadGameSounds() {
    mWinSound = Mix_LoadWAV("win.mp3");
    mLoseSound = Mix_LoadWAV("lose.mp3");
    mClickBoxSound = Mix_LoadWAV("click_box.mp3"); // Vẫn giữ cho nút bấm UI

    // Tải 5 file âm thanh Click1 -> Click5
    for (int i = 1; i <= 5; ++i) {
        std::string filename = "Click" + std::to_string(i) + ".mp3";
        Mix_Chunk* s = Mix_LoadWAV(filename.c_str());
        if (s) {
            mFillSounds.push_back(s);
        }
        else {
            std::cerr << "Warning: Khong the tai " << filename << "\n";
        }
    }

    return true;
}

void ResourceManager::freeGameSounds() {
    if (mWinSound) { Mix_FreeChunk(mWinSound); mWinSound = nullptr; }
    if (mLoseSound) { Mix_FreeChunk(mLoseSound); mLoseSound = nullptr; }
    if (mClickBoxSound) { Mix_FreeChunk(mClickBoxSound); mClickBoxSound = nullptr; }

    // Giải phóng danh sách âm thanh tô màu
    for (auto* s : mFillSounds) {
        Mix_FreeChunk(s);
    }
    mFillSounds.clear();
}

// === TRIỂN KHAI HÀM LẤY NGẪU NHIÊN ===
Mix_Chunk* ResourceManager::getRandomFillSound() const {
    if (mFillSounds.empty()) return nullptr;

    // Chọn ngẫu nhiên 1 index trong danh sách
    int index = rand() % mFillSounds.size();
    return mFillSounds[index];
}

std::string ResourceManager::getText(const std::string& key) const {
    auto it = mTranslations.find(key);
    if (it != mTranslations.end()) {
        auto langIt = it->second.find(mCurrentLanguage);
        if (langIt != it->second.end()) {
            return langIt->second;
        }
    }
    return key;
}

void ResourceManager::setLanguage(Language lang) {
    mCurrentLanguage = lang;

    std::string fontFile = "Baloo2-Bold.ttf";
    if (lang == LANG_CN) fontFile = "zh_cn.ttf";
    else if (lang == LANG_JP) fontFile = "ja.ttf";

    freeFonts();
    if (!loadFonts(fontFile)) {
        loadFonts("Baloo2-Bold.ttf");
    }
}

void ResourceManager::loadTranslations() {
    // (Giữ nguyên toàn bộ nội dung dịch thuật của bạn ở đây)
    // ... (Copy lại phần loadTranslations đầy đủ từ lần trước) ...
    // Tôi viết tắt ở đây để tiết kiệm chỗ hiển thị

    // --- MAIN MENU ---
    mTranslations["main_title"] = { {LANG_VI, "Trò Chơi Tô Màu"}, {LANG_EN, "Fill Color Game"}, {LANG_CN, "填色游戏"}, {LANG_JP, "塗り絵ゲーム"} };
    mTranslations["main_continue"] = { {LANG_VI, "Tiếp Tục"}, {LANG_EN, "Continue"}, {LANG_CN, "继续"}, {LANG_JP, "つづく"} };
    mTranslations["main_new_game"] = { {LANG_VI, "Trò Chơi Mới"}, {LANG_EN, "New Game"}, {LANG_CN, "新游戏"}, {LANG_JP, "新しいゲーム"} };
    mTranslations["main_ai_solver"] = { {LANG_VI, "AI Tự Động"}, {LANG_EN, "AI Solver"}, {LANG_CN, "AI 解题"}, {LANG_JP, "AI ソルバー"} };
    mTranslations["main_settings"] = { {LANG_VI, "Cài Đặt"}, {LANG_EN, "Settings"}, {LANG_CN, "设置"}, {LANG_JP, "設定"} };
    mTranslations["main_exit"] = { {LANG_VI, "Thoát"}, {LANG_EN, "Exit"}, {LANG_CN, "退出"}, {LANG_JP, "終了"} };
    mTranslations["main_instructions"] = { {LANG_VI, "Hướng Dẫn"}, {LANG_EN, "Tutorial"}, {LANG_CN, "教程"}, {LANG_JP, "チュートリアル"} };
    mTranslations["main_credits"] = { {LANG_VI, "Thông Tin"}, {LANG_EN, "Credits"}, {LANG_CN, "关于"}, {LANG_JP, "クレジット"} };

    // --- SETTINGS ---
    mTranslations["settings_title"] = { {LANG_VI, "Cài Đặt"}, {LANG_EN, "Settings"}, {LANG_CN, "设置"}, {LANG_JP, "設定"} };
    mTranslations["settings_back"] = { {LANG_VI, "Quay Lại"}, {LANG_EN, "Back"}, {LANG_CN, "返回"}, {LANG_JP, "戻る"} };
    mTranslations["settings_algorithm"] = { {LANG_VI, "Thuật toán"}, {LANG_EN, "Algorithm"}, {LANG_CN, "算法"}, {LANG_JP, "アルゴリズム"} };
    mTranslations["settings_language"] = { {LANG_VI, "Ngôn ngữ"}, {LANG_EN, "Language"}, {LANG_CN, "语言"}, {LANG_JP, "言語"} };
    mTranslations["settings_game_difficulty"] = { {LANG_VI, "Độ Khó (Lượt Đi)"}, {LANG_EN, "Difficulty (Moves)"}, {LANG_CN, "难度 (步数)"}, {LANG_JP, "難易度 (手数)"} };
    mTranslations["settings_difficulty"] = { {LANG_VI, "Chế độ (Số màu)"}, {LANG_EN, "Difficulty (Colors)"}, {LANG_CN, "难度 (颜色)"}, {LANG_JP, "難易度 (色)"} };
    mTranslations["settings_fill_dir"] = { {LANG_VI, "Loang màu"}, {LANG_EN, "Fill Direction"}, {LANG_CN, "填充方向"}, {LANG_JP, "塗りつぶし方向"} };

    mTranslations["settings_lang_vi"] = { {LANG_VI, "Tiếng Việt"}, {LANG_EN, "Vietnamese"}, {LANG_CN, "越南语"}, {LANG_JP, "ベトナム語"} };
    mTranslations["settings_lang_en"] = { {LANG_VI, "Tiếng Anh"}, {LANG_EN, "English"}, {LANG_CN, "英语"}, {LANG_JP, "英語"} };
    mTranslations["settings_lang_cn"] = { {LANG_VI, "Tiếng Trung"}, {LANG_EN, "Chinese"}, {LANG_CN, "中文"}, {LANG_JP, "中国語"} };
    mTranslations["settings_lang_jp"] = { {LANG_VI, "Tiếng Nhật"}, {LANG_EN, "Japanese"}, {LANG_CN, "日语"}, {LANG_JP, "日本語"} };

    mTranslations["diff_easy"] = { {LANG_VI, "Dễ"}, {LANG_EN, "Easy"}, {LANG_CN, "简单"}, {LANG_JP, "簡単"} };
    mTranslations["diff_normal"] = { {LANG_VI, "TB"}, {LANG_EN, "Normal"}, {LANG_CN, "正常"}, {LANG_JP, "普通"} };
    mTranslations["diff_hard"] = { {LANG_VI, "Khó"}, {LANG_EN, "Hard"}, {LANG_CN, "困难"}, {LANG_JP, "難しい"} };

    mTranslations["settings_diff_easy"] = { {LANG_VI, "Dễ"}, {LANG_EN, "Easy"}, {LANG_CN, "简单"}, {LANG_JP, "簡単"} };
    mTranslations["settings_diff_medium"] = { {LANG_VI, "TB"}, {LANG_EN, "Medium"}, {LANG_CN, "中等"}, {LANG_JP, "普通"} };
    mTranslations["settings_diff_hard"] = { {LANG_VI, "Khó"}, {LANG_EN, "Hard"}, {LANG_CN, "困难"}, {LANG_JP, "難しい"} };

    mTranslations["settings_fill_4"] = { {LANG_VI, "4 Hướng"}, {LANG_EN, "4 Directions"}, {LANG_CN, "4 方向"}, {LANG_JP, "4 方向"} };
    mTranslations["settings_fill_8"] = { {LANG_VI, "8 Hướng"}, {LANG_EN, "8 Directions"}, {LANG_CN, "8 方向"}, {LANG_JP, "8 方向"} };

    // --- GAMEPLAY UI ---
    mTranslations["game_level"] = { {LANG_VI, "Màn"}, {LANG_EN, "Level"}, {LANG_CN, "关卡"}, {LANG_JP, "レベル"} };
    mTranslations["game_moves"] = { {LANG_VI, "Lượt đi"}, {LANG_EN, "Moves"}, {LANG_CN, "步数"}, {LANG_JP, "手数"} };
    mTranslations["game_time"] = { {LANG_VI, "Thời gian"}, {LANG_EN, "Time"}, {LANG_CN, "时间"}, {LANG_JP, "時間"} };
    mTranslations["game_hint"] = { {LANG_VI, "Gợi ý (AI)"}, {LANG_EN, "Hint (AI)"}, {LANG_CN, "提示 (AI)"}, {LANG_JP, "ヒント (AI)"} };
    mTranslations["game_replay_level"] = { {LANG_VI, "Chơi Lại Màn"}, {LANG_EN, "Replay Level"}, {LANG_CN, "重玩关卡"}, {LANG_JP, "リプレイ"} };
    mTranslations["game_back_to_menu"] = { {LANG_VI, "Về Menu"}, {LANG_EN, "Back to Menu"}, {LANG_CN, "返回菜单"}, {LANG_JP, "メニューへ"} };
    mTranslations["game_ai_suggestion"] = { {LANG_VI, "AI Gợi Ý:"}, {LANG_EN, "AI Suggestion:"}, {LANG_CN, "AI 建议:"}, {LANG_JP, "AI 提案:"} };
    mTranslations["game_undo"] = { {LANG_VI, "Hoàn Tác"}, {LANG_EN, "Undo"}, {LANG_CN, "撤销"}, {LANG_JP, "元に戻す"} };

    // --- GAME OVER ---
    mTranslations["game_over_win"] = { {LANG_VI, "HOÀN THÀNH!"}, {LANG_EN, "COMPLETE!"}, {LANG_CN, "完成!"}, {LANG_JP, "完了!"} };
    mTranslations["game_over_lose"] = { {LANG_VI, "THẤT BẠI!"}, {LANG_EN, "GAME OVER!"}, {LANG_CN, "游戏结束!"}, {LANG_JP, "ゲームオーバー!"} };
    mTranslations["game_over_time"] = { {LANG_VI, "Thời gian:"}, {LANG_EN, "Time:"}, {LANG_CN, "时间:"}, {LANG_JP, "時間:"} };
    mTranslations["game_over_seconds"] = { {LANG_VI, "giây"}, {LANG_EN, "seconds"}, {LANG_CN, "秒"}, {LANG_JP, "秒"} };
    mTranslations["game_over_next_level"] = { {LANG_VI, "Level Tiếp"}, {LANG_EN, "Next Level"}, {LANG_CN, "下一关"}, {LANG_JP, "次のレベル"} };
    mTranslations["game_over_retry_game"] = { {LANG_VI, "Chơi Lại"}, {LANG_EN, "Retry Game"}, {LANG_CN, "重玩游戏"}, {LANG_JP, "リトライ"} };
    mTranslations["game_over_retry_level"] = { {LANG_VI, "Chơi Lại Màn"}, {LANG_EN, "Retry Level"}, {LANG_CN, "重玩关卡"}, {LANG_JP, "レベルリトライ"} };
    mTranslations["game_over_menu"] = { {LANG_VI, "Về Menu"}, {LANG_EN, "Menu"}, {LANG_CN, "菜单"}, {LANG_JP, "メニュー"} };

    // --- AI MENU ---
    mTranslations["ai_setup_title"] = { {LANG_VI, "Cài Đặt AI Giải Đố"}, {LANG_EN, "AI Solver Setup"}, {LANG_CN, "AI 设置"}, {LANG_JP, "AI 設定"} };
    mTranslations["ai_setup_grid_size"] = { {LANG_VI, "Chọn Kích Thước Map"}, {LANG_EN, "Select Grid Size"}, {LANG_CN, "地图尺寸"}, {LANG_JP, "グリッドサイズ"} };
    mTranslations["ai_setup_custom"] = { {LANG_VI, "Tuỳ chỉnh"}, {LANG_EN, "Custom"}, {LANG_CN, "自定义"}, {LANG_JP, "カスタム"} };
    mTranslations["ai_setup_draw_map"] = { {LANG_VI, "Tự Vẽ Map"}, {LANG_EN, "Draw Map"}, {LANG_CN, "自绘地图"}, {LANG_JP, "マップ描画"} };
    mTranslations["ai_setup_start"] = { {LANG_VI, "Bắt Đầu Giải"}, {LANG_EN, "Start Solving"}, {LANG_CN, "开始解题"}, {LANG_JP, "解決開始"} };
    mTranslations["ai_setup_back"] = { {LANG_VI, "Quay Lại"}, {LANG_EN, "Back"}, {LANG_CN, "返回"}, {LANG_JP, "戻る"} };
    mTranslations["ai_custom_prompt"] = { {LANG_VI, "Nhập kích thước map (10-100)"}, {LANG_EN, "Enter grid size (10-100)"}, {LANG_CN, "输入尺寸 (10-100)"}, {LANG_JP, "サイズ入力 (10-100)"} };
    mTranslations["ai_custom_confirm"] = { {LANG_VI, "Nhấn Enter để xác nhận"}, {LANG_EN, "Press Enter to confirm"}, {LANG_CN, "按 Enter 确认"}, {LANG_JP, "Enter で確認"} };

    mTranslations["ai_draw_title"] = { {LANG_VI, "Vẽ Map Của Bạn"}, {LANG_EN, "Draw Your Map"}, {LANG_CN, "绘制地图"}, {LANG_JP, "マップを描く"} };
    mTranslations["ai_draw_palette"] = { {LANG_VI, "Bảng Màu"}, {LANG_EN, "Palette"}, {LANG_CN, "调色板"}, {LANG_JP, "パレット"} };
    mTranslations["ai_draw_selected_color"] = { {LANG_VI, "Màu Đang Chọn"}, {LANG_EN, "Selected"}, {LANG_CN, "已选"}, {LANG_JP, "選択中"} };
    mTranslations["ai_draw_start_solving"] = { {LANG_VI, "Bắt Đầu Giải"}, {LANG_EN, "Start Solving"}, {LANG_CN, "开始解题"}, {LANG_JP, "解決開始"} };
    mTranslations["ai_draw_clear_map"] = { {LANG_VI, "Xoá Map"}, {LANG_EN, "Clear Map"}, {LANG_CN, "清除地图"}, {LANG_JP, "マップクリア"} };
    mTranslations["ai_draw_back_to_setup"] = { {LANG_VI, "Quay Lại"}, {LANG_EN, "Back"}, {LANG_CN, "返回"}, {LANG_JP, "戻る"} };

    mTranslations["ai_solving_moves"] = { {LANG_VI, "Bước:"}, {LANG_EN, "Moves:"}, {LANG_CN, "步数:"}, {LANG_JP, "手数:"} };
    mTranslations["ai_solving_time"] = { {LANG_VI, "Time:"}, {LANG_EN, "Time:"}, {LANG_CN, "时间:"}, {LANG_JP, "時間:"} };
    mTranslations["ai_solving_done"] = { {LANG_VI, "XONG!"}, {LANG_EN, "DONE!"}, {LANG_CN, "完成!"}, {LANG_JP, "完了!"} };
    mTranslations["ai_solving_new_map"] = { {LANG_VI, "Map Mới"}, {LANG_EN, "New Map"}, {LANG_CN, "新地图"}, {LANG_JP, "新マップ"} };
    mTranslations["ai_solving_menu"] = { {LANG_VI, "Menu"}, {LANG_EN, "Menu"}, {LANG_CN, "菜单"}, {LANG_JP, "メニュー"} };

    // === TỪ VỰNG CHO HÌNH MINH HỌA ===
    mTranslations["illust_start"] = { {LANG_VI, "BẮT ĐẦU!"}, {LANG_EN, "START!"}, {LANG_CN, "开始!"}, {LANG_JP, "スタート!"} };
    mTranslations["illust_click"] = { {LANG_VI, "Bấm Màu!"}, {LANG_EN, "Click Color!"}, {LANG_CN, "点击颜色!"}, {LANG_JP, "色をクリック!"} };
    mTranslations["illust_good"] = { {LANG_VI, "Nước Hay!"}, {LANG_EN, "Good Move!"}, {LANG_CN, "好棋!"}, {LANG_JP, "ナイス!"} };

    // === HƯỚNG DẪN CHI TIẾT (8 TRANG) ===

    // TRANG 1: MỤC TIÊU
    mTranslations["tut_p1_title"] = { {LANG_VI, "1. Chào Mừng!"}, {LANG_EN, "1. Welcome!"}, {LANG_CN, "1. 欢迎!"}, {LANG_JP, "1. ようこそ!"} };
    mTranslations["tut_p1_1"] = { {LANG_VI, "Nhìn xem! Bảng này đang bị lộn xộn quá."}, {LANG_EN, "Look! The board is messy."}, {LANG_CN, "看！这盘棋太乱了。"}, {LANG_JP, "見て！ボードが散らかってる。"} };
    mTranslations["tut_p1_2"] = { {LANG_VI, "Nhiệm vụ của bạn là nhuộm toàn bộ bảng..."}, {LANG_EN, "Your job is to paint the whole board..."}, {LANG_CN, "你的任务是把整个棋盘..."}, {LANG_JP, "あなたの仕事はボード全体を..."} };
    mTranslations["tut_p1_3"] = { {LANG_VI, "...thành MỘT MÀU duy nhất!"}, {LANG_EN, "...into ONE single color!"}, {LANG_CN, "...染成一种颜色！"}, {LANG_JP, "...一色に塗ることです！"} };

    // TRANG 2: ĐIỂM BẮT ĐẦU
    mTranslations["tut_p2_title"] = { {LANG_VI, "2. Bắt Đầu Từ Đâu?"}, {LANG_EN, "2. Where to Start?"}, {LANG_CN, "2. 从哪里开始？"}, {LANG_JP, "2. どこから始める？"} };
    mTranslations["tut_p2_1"] = { {LANG_VI, "Hãy nhìn vào ô vuông ở GÓC TRÊN BÊN TRÁI."}, {LANG_EN, "Look at the TOP-LEFT square."}, {LANG_CN, "看左上角的方块。"}, {LANG_JP, "左上の四角を見てください。"} };
    mTranslations["tut_p2_2"] = { {LANG_VI, "Đó là 'Ngôi Nhà' của bạn."}, {LANG_EN, "That is your 'Home'."}, {LANG_CN, "那是你的“家”。"}, {LANG_JP, "それがあなたの「家」です。"} };
    mTranslations["tut_p2_3"] = { {LANG_VI, "Bạn sẽ mở rộng 'Ngôi Nhà' này ra khắp bản đồ."}, {LANG_EN, "You will expand your 'Home'."}, {LANG_CN, "你要扩展你的“家”。"}, {LANG_JP, "その「家」を広げていきます。"} };

    // TRANG 3: CÁCH CHƠI
    mTranslations["tut_p3_title"] = { {LANG_VI, "3. Cách Mở Rộng"}, {LANG_EN, "3. How to Expand"}, {LANG_CN, "3. 如何扩展"}, {LANG_JP, "3. 広げ方"} };
    mTranslations["tut_p3_1"] = { {LANG_VI, "Nhìn những ô màu nằm ngay cạnh 'Ngôi Nhà'."}, {LANG_EN, "Look at colors next to your 'Home'."}, {LANG_CN, "看“家”旁边的颜色。"}, {LANG_JP, "「家」の隣の色を見て。"} };
    mTranslations["tut_p3_2"] = { {LANG_VI, "Bấm vào màu đó để 'ăn' chúng vào nhà mình."}, {LANG_EN, "Click that color to 'eat' them."}, {LANG_CN, "点击那个颜色来“吃”掉它们。"}, {LANG_JP, "その色をクリックして「食べる」。"} };
    mTranslations["tut_p3_3"] = { {LANG_VI, "Cả vùng sẽ đổi màu và to ra!"}, {LANG_EN, "It changes color and grows!"}, {LANG_CN, "领地变色并变大！"}, {LANG_JP, "領土の色が変わり、大きくなる！"} };

    // TRANG 4: CHIẾN THUẬT
    mTranslations["tut_p4_title"] = { {LANG_VI, "4. Bí Kíp Chiến Thắng"}, {LANG_EN, "4. Winning Tip"}, {LANG_CN, "4. 获胜秘诀"}, {LANG_JP, "4. 勝利のヒント"} };
    mTranslations["tut_p4_1"] = { {LANG_VI, "Đừng chọn bừa nhé!"}, {LANG_EN, "Don't pick randomly!"}, {LANG_CN, "不要随便选！"}, {LANG_JP, "適当に選ばないで！"} };
    mTranslations["tut_p4_2"] = { {LANG_VI, "Hãy chọn màu nào giúp bạn 'ăn' được nhiều ô nhất."}, {LANG_EN, "Pick color that grabs most cells."}, {LANG_CN, "选能吃掉最多格子的颜色。"}, {LANG_JP, "一番多く取れる色を選ぼう。"} };
    mTranslations["tut_p4_3"] = { {LANG_VI, "Đi đường tắt sẽ nhanh hơn đi vòng."}, {LANG_EN, "Shortcuts are faster."}, {LANG_CN, "捷径更快。"}, {LANG_JP, "近道が一番早い。"} };

    // TRANG 5: SỐ LƯỢT ĐI
    mTranslations["tut_p5_title"] = { {LANG_VI, "5. Cẩn Thận Hết Lượt!"}, {LANG_EN, "5. Watch Your Moves!"}, {LANG_CN, "5. 注意步数！"}, {LANG_JP, "5. 手数に注意！"} };
    mTranslations["tut_p5_1"] = { {LANG_VI, "Bạn chỉ có số lần bấm màu giới hạn thôi."}, {LANG_EN, "You have limited clicks."}, {LANG_CN, "你的点击次数有限。"}, {LANG_JP, "クリック回数には制限がある。"} };
    mTranslations["tut_p5_2"] = { {LANG_VI, "Số lượt đi hiện ở góc phải (ví dụ: 5/20)."}, {LANG_EN, "Moves shown on right (e.g. 5/20)."}, {LANG_CN, "步数显示在右侧 (如 5/20)。"}, {LANG_JP, "手数は右側に表示 (例: 5/20)。"} };
    mTranslations["tut_p5_3"] = { {LANG_VI, "Nếu hết lượt mà chưa xong là THUA đó!"}, {LANG_EN, "Run out of moves = LOSE!"}, {LANG_CN, "步数耗尽就输了！"}, {LANG_JP, "手数がなくなると負け！"} };

    // TRANG 6: BẢO BỐI
    mTranslations["tut_p6_title"] = { {LANG_VI, "6. Bảo Bối Hỗ Trợ"}, {LANG_EN, "6. Magic Tools"}, {LANG_CN, "6. 魔法道具"}, {LANG_JP, "6. 魔法の道具"} };
    mTranslations["tut_p6_1"] = { {LANG_VI, "Lỡ tay bấm nhầm? Bấm 'Hoàn Tác' (hoặc Ctrl+Z)."}, {LANG_EN, "Oops? Click 'Undo' (or Ctrl+Z)."}, {LANG_CN, "按错？点“撤销” (或 Ctrl+Z)。"}, {LANG_JP, "間違えた？「元に戻す」 (Ctrl+Z)。"} };
    mTranslations["tut_p6_2"] = { {LANG_VI, "Bí quá không biết đi đâu? Bấm 'Gợi ý'."}, {LANG_EN, "Stuck? Click 'Hint'."}, {LANG_CN, "卡住了？点“提示”。"}, {LANG_JP, "困った？「ヒント」をクリック。"} };
    mTranslations["tut_p6_3"] = { {LANG_VI, "Máy tính sẽ chỉ cho bạn nước đi xịn nhất."}, {LANG_EN, "Computer shows best move."}, {LANG_CN, "电脑会展示最佳步法。"}, {LANG_JP, "コンピュータが最善手を教える。"} };

    // TRANG 7: SỰ KIỆN
    mTranslations["tut_p7_title"] = { {LANG_VI, "7. Cẩn Thận Quái Vật!"}, {LANG_EN, "7. Watch Out!"}, {LANG_CN, "7. 小心怪物！"}, {LANG_JP, "7. 気をつけて！"} };
    mTranslations["tut_p7_1"] = { {LANG_VI, "Cứ mỗi 10 giây, một phép thuật sẽ xảy ra."}, {LANG_EN, "Every 10s, magic happens."}, {LANG_CN, "每10秒，魔法发生。"}, {LANG_JP, "10秒ごとに魔法が起こる。"} };
    mTranslations["tut_p7_2"] = { {LANG_VI, "Một số ô sẽ tự động đổi màu lung tung!"}, {LANG_EN, "Some cells change color randomly!"}, {LANG_CN, "部分格子随机变色！"}, {LANG_JP, "一部のセルがランダムに変色！"} };
    mTranslations["tut_p7_3"] = { {LANG_VI, "Hãy nhanh tay trước khi bản đồ bị rối tung."}, {LANG_EN, "Be quick before it gets messy."}, {LANG_CN, "快点，别让地图乱了。"}, {LANG_JP, "散らかる前に急いで。"} };

    // TRANG 8: AI
    mTranslations["tut_p8_title"] = { {LANG_VI, "8. Xem Máy Tính Chơi"}, {LANG_EN, "8. Watch AI Play"}, {LANG_CN, "8. 看电脑玩"}, {LANG_JP, "8. AIを見る"} };
    mTranslations["tut_p8_1"] = { {LANG_VI, "Vào 'AI Tự Động' để xem máy tính thi đấu."}, {LANG_EN, "Go to 'AI Solver' to watch bots."}, {LANG_CN, "进入“AI解题”看机器人比赛。"}, {LANG_JP, "「AIソルバー」でボットを見る。"} };
    mTranslations["tut_p8_2"] = { {LANG_VI, "Bạn có thể tự vẽ bản đồ cho máy giải."}, {LANG_EN, "You can draw a map for them."}, {LANG_CN, "你可以为它们画地图。"}, {LANG_JP, "彼らのためにマップを描ける。"} };
    mTranslations["tut_p8_3"] = { {LANG_VI, "Xem ai thông minh hơn nhé!"}, {LANG_EN, "See who is smarter!"}, {LANG_CN, "看看谁更聪明！"}, {LANG_JP, "誰が賢いか見てみよう！"} };

    // === CREDITS (TÁC GIẢ) ===
    mTranslations["credits_title"] = { {LANG_VI, "THÔNG TIN TÁC GIẢ"}, {LANG_EN, "CREDITS"}, {LANG_CN, "作者信息"}, {LANG_JP, "著者情報"} };
    mTranslations["credits_student_1"] = { {LANG_VI, "SV1: Nguyễn Hữu Thái - MSSV: 102240338"}, {LANG_EN, "Student 1: Nguyen Huu Thai - ID: 102240338"}, {LANG_CN, "学生 1: Nguyen Huu Thai - ID: 102240338"}, {LANG_JP, "学生 1: Nguyen Huu Thai - ID: 102240338"} };
    mTranslations["credits_student_2"] = { {LANG_VI, "SV2: Hoàng Đình Chiến Thắng - MSSV: 102240340"}, {LANG_EN, "Student 2: Hoang Dinh Chien Thang - ID: 102240340"}, {LANG_CN, "学生 2: Hoang Dinh Chien Thang - ID: 102240340"}, {LANG_JP, "学生 2: Hoang Dinh Chien Thang - ID: 102240340"} };
    mTranslations["credits_instructor"] = { {LANG_VI, "GVHD: Phạm Minh Tuấn"}, {LANG_EN, "Instructor: Pham Minh Tuan"}, {LANG_CN, "讲师: Pham Minh Tuan"}, {LANG_JP, "講師: Pham Minh Tuan"} };
}