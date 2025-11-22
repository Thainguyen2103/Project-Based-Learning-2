#pragma once

#include <SDL.h>

class Game;

/**
 * @class GameState
 * @brief Lớp cơ sở trừu tượng cho tất cả các trạng thái trong game (ví dụ: MainMenu, Playing, Settings).
 *
 * Mỗi trạng thái cụ thể sẽ kế thừa từ lớp này và triển khai các phương thức ảo thuần túy
 * để xử lý sự kiện, cập nhật logic và vẽ đồ họa riêng cho trạng thái đó.
 */
class GameState {
public:
    /**
     * @brief Hàm khởi tạo, nhận một con trỏ tới đối tượng Game chính.
     * @param game Con trỏ tới đối tượng Game.
     */
    GameState(Game* game) : mGame(game) {}

    /**
     * @brief Hàm hủy ảo.
     */
    virtual ~GameState() {}

    /**
     * @brief Xử lý các sự kiện đầu vào (chuột, bàn phím) cho trạng thái này.
     * @param e Sự kiện SDL cần xử lý.
     */
    virtual void handleEvents(SDL_Event& e) = 0;

    /**
     * @brief Cập nhật logic của trạng thái.
     */
    virtual void update() = 0;

    /**
     * @brief Vẽ trạng thái lên màn hình.
     * @param renderer Con trỏ tới SDL_Renderer để vẽ.
     */
    virtual void draw(SDL_Renderer* renderer) = 0;

protected:
    Game* mGame; ///< Con trỏ tới đối tượng Game chính để truy cập các chức năng toàn cục.
};