#pragma once

#include <vector>
#include <string>
#include "Constants.h"
#include "ResourceManager.h"
#include <SDL_mixer.h>

class GameState;

/**
 * @class Game
 * @brief Lớp chính điều khiển toàn bộ trò chơi, bao gồm vòng lặp game, quản lý trạng thái, và các cài đặt chung.
 */
class Game {
public:
    /**
     * @brief Hàm khởi tạo lớp Game.
     */
    Game();

    /**
     * @brief Hàm hủy lớp Game, giải phóng tài nguyên.
     */
    ~Game();

    /**
     * @brief Khởi tạo các thành phần cốt lõi của game (SDL, cửa sổ, renderer).
     * @return true nếu khởi tạo thành công, false nếu thất bại.
     */
    bool init();

    /**
     * @brief Bắt đầu vòng lặp chính của game.
     */
    void run();

    /**
     * @brief Đẩy một trạng thái mới vào stack quản lý trạng thái.
     * @param state Con trỏ tới trạng thái cần đẩy vào.
     */
    void pushState(GameState* state);

    /**
     * @brief Lấy ra (xóa) trạng thái hiện tại khỏi stack.
     */
    void popState();

    /**
     * @brief Thay đổi trạng thái hiện tại bằng một trạng thái mới.
     * @param state Con trỏ tới trạng thái mới.
     */
    void changeState(GameState* state);

    /**
     * @brief Lấy ra một số lượng trạng thái nhất định khỏi stack.
     * @param count Số lượng trạng thái cần lấy ra.
     */
    void popStates(int count);

    /**
     * @brief Lấy ra trạng thái hiện tại và thay đổi bằng một trạng thái mới.
     * @param state Con trỏ tới trạng thái mới.
     */
    void popAndChange(GameState* state);

    /**
     * @brief Lấy ra trạng thái hiện tại và thử lại (thường dùng trong PlayingState).
     */
    void popAndRetry();

    /**
     * @brief Lấy con trỏ tới trạng thái ở đỉnh stack.
     * @return Con trỏ tới GameState hiện tại.
     */
    GameState* getTopState();

    /**
     * @brief Yêu cầu thoát game.
     */
    void requestQuit() { mIsRunning = false; }

    /**
     * @brief Lấy renderer của game.
     * @return Con trỏ tới SDL_Renderer.
     */
    SDL_Renderer* getRenderer() { return mRenderer; }

    /**
     * @brief Lấy con trỏ tới đối tượng quản lý tài nguyên.
     * @return Con trỏ tới ResourceManager.
     */
    ResourceManager* getResources() { return &mResourceManager; }

    /**
     * @brief Kiểm tra xem tệp lưu game có tồn tại không.
     * @return true nếu tệp tồn tại, false nếu không.
     */
    bool getSaveFileExists() const { return mSaveFileExists; }

    /**
     * @brief Đặt trạng thái tồn tại của tệp lưu game.
     * @param exists Trạng thái tồn tại.
     */
    void setSaveFileExists(bool exists) { mSaveFileExists = exists; }

    /**
     * @brief Kiểm tra sự tồn tại của tệp lưu game.
     */
    void checkForSaveFile();

    /**
     * @brief Xóa tệp lưu game.
     */
    void deleteSaveFile();

    /**
     * @brief Tải cài đặt từ tệp.
     */
    void loadSettings();

    /**
     * @brief Lưu cài đặt hiện tại vào tệp.
     */
    void saveSettings();

    /**
     * @brief Tải màn chơi cao nhất đã đạt được.
     */
    void loadHighestLevel();

    /**
     * @brief Lưu màn chơi cao nhất đã đạt được.
     */
    void saveHighestLevel();

    /**
     * @brief Đặt thuật toán giải đố.
     * @param algo Loại thuật toán.
     */
    void setAlgorithm(AlgorithmType algo) { mCurrentAlgorithm = algo; }

    /**
     * @brief Lấy thuật toán giải đố hiện tại.
     * @return Loại thuật toán.
     */
    AlgorithmType getAlgorithm() const { return mCurrentAlgorithm; }

    /**
     * @brief Đặt hướng tô màu.
     * @param dir Hướng tô.
     */
    void setFillDirection(FillDirection dir) { mFillDirection = dir; }

    /**
     * @brief Lấy hướng tô màu hiện tại.
     * @return Hướng tô.
     */
    FillDirection getFillDirection() const { return mFillDirection; }

    /**
     * @brief Đặt số lượng màu sắc trong game.
     * @param colors Số lượng màu.
     */
    void setNumColors(int colors) { mNumColors = colors; }

    /**
     * @brief Lấy số lượng màu sắc hiện tại.
     * @return Số lượng màu.
     */
    int getNumColors() const { return mNumColors; }

    /**
     * @brief Lấy màn chơi cao nhất đã đạt được.
     * @return Số thứ tự màn chơi cao nhất.
     */
    int getHighestLevel() const { return mHighestLevel; }

    /**
     * @brief Đặt màn chơi cao nhất.
     * @param level Số thứ tự màn chơi.
     */
    void setHighestLevel(int level);

    /**
     * @brief Đặt độ khó của game.
     * @param diff Độ khó.
     */
    void setGameDifficulty(GameDifficulty diff) { mGameDifficulty = diff; }

    /**
     * @brief Lấy độ khó của game.
     * @return Độ khó hiện tại.
     */
    GameDifficulty getGameDifficulty() const { return mGameDifficulty; }

    /**
     * @brief Lấy hệ số điều chỉnh dựa trên độ khó.
     * @return Hệ số độ khó.
     */
    float getDifficultyModifier() const;

private:
    /**
     * @brief Xử lý các sự kiện đầu vào (chuột, bàn phím).
     */
    void handleEvents();

    /**
     * @brief Cập nhật logic của game.
     */
    void update();

    /**
     * @brief Vẽ các đối tượng game lên màn hình.
     */
    void draw();

    /**
     * @brief Dọn dẹp và giải phóng tài nguyên trước khi thoát.
     */
    void close();

    bool mIsRunning; ///< Cờ kiểm soát vòng lặp game.
    SDL_Window* mWindow; ///< Con trỏ tới cửa sổ game.
    SDL_Renderer* mRenderer; ///< Con trỏ tới renderer để vẽ.

    ResourceManager mResourceManager; ///< Đối tượng quản lý tài nguyên (hình ảnh, âm thanh).
    std::vector<GameState*> mStates; ///< Stack chứa các trạng thái của game.
    bool mSaveFileExists; ///< Cờ báo hiệu tệp lưu game có tồn tại không.
    int mHighestLevel; ///< Màn chơi cao nhất người chơi đã đạt được.
    AlgorithmType mCurrentAlgorithm; ///< Thuật toán AI đang được sử dụng.
    FillDirection mFillDirection; ///< Hướng tô màu của AI.
    int mNumColors; ///< Số lượng màu trong màn chơi AI.

    GameDifficulty mGameDifficulty; ///< Độ khó hiện tại của game.
};