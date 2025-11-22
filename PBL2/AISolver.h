#pragma once

#include "Constants.h"
#include "Grid.h"
#include "ResourceManager.h"
#include <chrono>
#include <queue>
#include <stack>

/**
 * @class AISolver
 * @brief Lớp chịu trách nhiệm giải quyết màn chơi bằng các thuật toán tô màu khác nhau.
 *
 * Lớp này quản lý logic của AI, bao gồm việc chọn nước đi tiếp theo,
 * thực hiện thuật toán tô màu (BFS, DFS, Scanline), và theo dõi tiến trình giải đố.
 */
class AISolver {
public:
    /**
     * @brief Hàm khởi tạo mặc định.
     */
    AISolver();

    /**
     * @brief Khởi tạo AI solver với một thuật toán, hướng tô, bản đồ và số màu cụ thể.
     * @param algo Loại thuật toán sẽ sử dụng (BFS, DFS, Scanline).
     * @param fillDir Hướng tô (4 hướng hoặc 8 hướng).
     * @param masterGrid Trạng thái ban đầu của lưới.
     * @param numColors Số lượng màu sắc có trong màn chơi.
     */
    void init(AlgorithmType algo, FillDirection fillDir, const std::vector<std::vector<SDL_Color>>& masterGrid, int numColors);

    /**
     * @brief Cập nhật trạng thái của AI solver.
     *
     * Nếu chưa hoàn thành, nó sẽ tìm nước đi tốt nhất và bắt đầu tô màu.
     * Nếu đang trong quá trình tô màu, nó sẽ tiếp tục hiệu ứng tô màu.
     */
    void update();

    /**
     * @brief Vẽ trạng thái hiện tại của lưới và thông tin của AI solver.
     * @param renderer Con trỏ tới SDL_Renderer.
     * @param viewport Hình chữ nhật xác định khu vực vẽ.
     * @param font Font chữ để hiển thị thông tin chính.
     * @param smallFont Font chữ nhỏ hơn cho các chi tiết phụ.
     * @param res Con trỏ tới ResourceManager để truy cập tài nguyên.
     */
    void draw(SDL_Renderer* renderer, SDL_Rect viewport, TTF_Font* font, TTF_Font* smallFont, ResourceManager* res);

    /**
     * @brief Lấy tên của thuật toán đang được sử dụng.
     * @return Chuỗi tên thuật toán.
     */
    std::string getName() const;

    /**
     * @brief Lấy số bước đi mà AI đã thực hiện.
     * @return Số bước đi.
     */
    int getMoves() const { return mMoves; }

    /**
     * @brief Lấy thời gian đã trôi qua kể từ khi AI bắt đầu giải.
     * @return Thời gian đã trôi qua (tính bằng giây).
     */
    double getElapsedTime() const { return mElapsedTime.count(); }

    /**
     * @brief Kiểm tra xem AI đã giải xong chưa.
     * @return true nếu đã giải xong, false nếu chưa.
     */
    bool isDone() const { return mIsDone; }

private:
    /**
     * @brief Tìm màu tiếp theo tốt nhất để tô.
     *
     * Thuật toán này đánh giá mỗi màu có thể chọn và trả về màu mở rộng vùng được tô nhiều nhất.
     * @return Màu tốt nhất để chọn cho nước đi tiếp theo.
     */
    SDL_Color findBestNextMove() const;

    /**
     * @brief Bắt đầu quá trình tô màu loang (flood fill) với một màu mới.
     * @param newColor Màu sẽ được dùng để tô.
     */
    void startFloodFill(const SDL_Color& newColor);

    /**
     * @brief Thực hiện một bước của hiệu ứng tô màu.
     */
    void animateFillStep();

    /**
     * @brief Kiểm tra xem toàn bộ lưới đã được tô cùng một màu chưa (điều kiện thắng).
     */
    void checkWinCondition();

    /**
     * @brief Đếm số ô trong vùng được kết nối bắt đầu từ (0,0).
     * @param grid Lưới để kiểm tra.
     * @param fillDir Hướng kết nối (4 hoặc 8).
     * @return Số lượng ô trong vùng được kết nối.
     */
    static int countConnectedRegion(const std::vector<std::vector<SDL_Color>>& grid, FillDirection fillDir);

    Grid mGrid;                     ///< Lưới game mà AI đang giải.
    AlgorithmType mAlgoType;        ///< Loại thuật toán đang sử dụng.
    FillDirection mFillDir;         ///< Hướng tô màu (4 hoặc 8 hướng).
    int mNumColors;                 ///< Số lượng màu trong màn chơi.

    int mMoves;                     ///< Số bước đi đã thực hiện.
    bool mIsDone;                   ///< Cờ báo hiệu đã giải xong.
    bool mIsAnimating;              ///< Cờ báo hiệu đang trong hiệu ứng tô màu.

    std::chrono::steady_clock::time_point mStartTime; ///< Thời điểm bắt đầu giải.
    std::chrono::duration<double> mElapsedTime;       ///< Thời gian đã trôi qua.

    SDL_Color mTargetColor, mReplacementColor; ///< Màu mục tiêu và màu thay thế cho thuật toán tô màu.
    std::queue<std::pair<int, int>> mBfsQueue;   ///< Hàng đợi cho thuật toán BFS.
    std::stack<std::pair<int, int>> mDfsStack;   ///< Ngăn xếp cho thuật toán DFS.
    std::stack<std::pair<int, int>> mScanlineStack; ///< Ngăn xếp cho thuật toán Scanline.
};