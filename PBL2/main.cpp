#include <cstdlib>
#include <ctime>
#include "Game.h"

/**
 * @file main.cpp
 * @brief Điểm khởi đầu của ứng dụng game.
 *
 * Chứa hàm `main` để khởi tạo và chạy đối tượng Game.
 */

/**
 * @brief Hàm chính của chương trình.
 *
 * Khởi tạo bộ sinh số ngẫu nhiên, tạo một đối tượng `Game`,
 * khởi tạo nó, và sau đó chạy vòng lặp chính của game.
 *
 * @param argc Số lượng đối số dòng lệnh.
 * @param args Mảng các đối số dòng lệnh.
 * @return 0 nếu chương trình kết thúc thành công, 1 nếu có lỗi.
 */
int main(int argc, char* args[]) {
    // Khởi tạo bộ sinh số ngẫu nhiên với seed là thời gian hiện tại
    srand(static_cast<unsigned int>(time(0)));

    // Tạo một đối tượng Game
    Game game;

    // Khởi tạo game. Nếu thất bại, in ra lỗi và thoát.
    if (!game.init()) {
        std::cerr << "Khởi tạo Game thất bại!" << std::endl;
        return 1;
    }

    // Bắt đầu vòng lặp chính của game
    game.run();

    // Trả về 0 để báo hiệu kết thúc thành công
    return 0;
}