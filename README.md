🎨 Fill Color Game - C++ SDL2 Project

Đồ án Lập trình PBL2: Trò chơi giải đố thuật toán Flood-Fill kết hợp mô phỏng AI.

📖 Giới thiệu (Introduction)

Fill Color Game là một trò chơi giải đố trí tuệ dựa trên cơ chế "Flood Fill". Người chơi bắt đầu từ một ô màu ở góc và phải nhuộm toàn bộ bàn cờ thành một màu duy nhất trong số lượt đi giới hạn.

Dự án còn tích hợp chế độ AI Simulation, nơi người dùng có thể trực quan hóa và so sánh hiệu suất của các thuật toán duyệt đồ thị như BFS, DFS, Scanline và Union-Find.

🚀 Tính Năng Nổi Bật (Key Features)
🎮 1. Chế Độ Người Chơi (Single Player)

Chọn màu để mở rộng vùng đang chiếm.

Giới hạn số lượt đi (Max Moves) và thời gian.

Sự kiện ngẫu nhiên: Sau mỗi 10s, một số ô tự đổi màu để tăng độ khó.

Chức năng hỗ trợ:

🔙 Undo (Ctrl+Z)

💡 Hint – AI gợi ý nước đi

🔄 Replay màn chơi

🤖 2. Chế Độ AI (AI Simulation)

So sánh hiệu suất 4 thuật toán:

BFS (Breadth-First Search)

DFS (Depth-First Search)

Scanline Flood Fill

Union-Find (DSU)

Cho phép người dùng tự vẽ bản đồ và cho AI giải.

⚙️ 3. Hệ Thống & Giao Diện

Hỗ trợ 4 ngôn ngữ: 🇻🇳 Tiếng Việt, 🇺🇸 English, 🇨🇳 中文, 🇯🇵 日本語.

Tùy chỉnh kích thước bàn cờ (10x10 → 40x40), số lượng màu (6–8).

Hiệu ứng giao diện, animation, âm thanh đầy đủ.

🛠️ Công Nghệ Sử Dụng (Tech Stack)

Ngôn ngữ: C++ (C++14/17)

Đồ họa: SDL2

Thư viện mở rộng:
SDL2_image, SDL2_ttf, SDL2_mixer

IDE: Visual Studio 2022 / 2026

Kiến trúc: OOP + State Pattern

📥 Cài Đặt & Chạy (Installation)
Yêu cầu:

Visual Studio 2022+

SDL2 Development Libraries (VC)

Các bước:

Clone repository:

git clone https://github.com/Thainguyen2103/Project-Based-Learning-2.git


Mở file .sln bằng Visual Studio.

Cấu hình SDL2:

Include Directories → thư mục include của SDL2

Library Directories → thư mục lib

Linker → thêm:
SDL2.lib, SDL2main.lib, SDL2_image.lib, SDL2_ttf.lib, SDL2_mixer.lib

Copy các file .dll của SDL2 vào thư mục chạy .exe (Debug/Release).

Đảm bảo thư mục chạy có:

Font: arial.ttf, Baloo2-Bold.ttf, zh_cn.ttf, ja.ttf

Âm thanh: *.mp3

Build & chạy bằng F5.

👥 Tác Giả (Credits)

Dự án được thực hiện bởi sinh viên Đại học Bách Khoa Đà Nẵng (DUT):

Nguyễn Hữu Thái (102240338) – Thuật toán, hệ thống, AI

Hoàng Đình Chiến Thắng (102240340) – UI/UX, Gameplay Logic

Giảng viên hướng dẫn: Thầy Phạm Minh Tuấn.

Dự án phục vụ mục đích học tập. Không sử dụng thương mại.
