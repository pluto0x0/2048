#include <conio.h>

#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <random>
#include <vector>

#define N 4           // 棋盘大小
#define WIN_NUM 2048  // 获胜数字

class Board {
    int grid[N][N] = {0};
    float prob4, probnew;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

   public:
    Board(int nNum = 2, float prob4 = 0.2f, float probnew = 0.6f)
        // nNum: 初始数字个数, prob4: 生成4的概率, probnew: 移动后生成新数字的概率
        : prob4(prob4),
          probnew(probnew),
          gen(std::chrono::steady_clock::now().time_since_epoch().count()),
          dis(0, 1) {
        while (nNum--) addRandomNum();
    }
    void print() {
        std::system("cls");
        std::string title = "[2048 Game]";
        std::cout << std::string((N * 5 + 1 - title.size()) / 2, ' ') << title
                  << std::endl;
        for (int i = 0; i <= N; ++i) {
            for (int j = 0; j < N; ++j) std::cout << "+----";
            std::cout << "+" << std::endl;
            if (i == N) break;
            for (int j = 0; j < N; ++j) {
                std::cout << "|";
                std::cout.width(4);
                if (grid[i][j] != 0) {
                    std::cout << grid[i][j];
                } else
                    std::cout << " ";
            }
            std::cout << "|" << std::endl;
        }
    }
    bool addRandomNum() {
        int num = (dis(gen) < prob4) ? 4 : 2;
        int gridProb[N * N];
        for (int i = 0; i < N * N; ++i) gridProb[i] = grid[i / N][i % N] ? 0 : 1;
        std::discrete_distribution<> pos(gridProb, gridProb + N * N);
        int idx = pos(gen);
        int r = idx / N, c = idx % N;
        if (grid[r][c] != 0) return false;
        grid[r][c] = num;
        return true;
    }
    void moveLeft() {
        leftShift();
        print();
    }
    void moveRight() {
        rotateCCW(2);
        leftShift();
        rotateCCW(2);
        print();
    }
    void moveUp() {
        rotateCCW(1);
        leftShift();
        rotateCCW(3);
        print();
    }
    void moveDown() {
        rotateCCW(3);
        leftShift();
        rotateCCW(1);
        print();
    }
    signed won() {
        signed ret = -1;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) {
                if (grid[i][j] >= WIN_NUM) return 1;
                if (grid[i][j] == 0) ret = 0;
            }
        if (ret == -1) {
            int tmp[N][N];
            memcpy(tmp, grid, sizeof(grid));
            for (int t = 0; t < 4; ++t) {
                if (leftShift()) {
                    ret = 0;
                    break;
                }
                rotateCCW(1);
            }
            memcpy(grid, tmp, sizeof(grid));
        }
        return ret;
    }

   private:
    void rotateCCW(unsigned times = 1) {
        int tmp[N][N];
        while (times--) {
            memcpy(tmp, grid, sizeof(grid));
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j) grid[N - j - 1][i] = tmp[i][j];
        }
    }
    bool leftShift() {
        bool moved = false;
        for (int i = 0; i < N; ++i) {
            int head = 0;
            for (int j = 0; j < N; ++j)
                if (grid[i][j]) {
                    int t = grid[i][j];
                    grid[i][j] = 0;
                    if (j != head) moved = true;
                    grid[i][head++] = t;
                }
            if (head == 0) continue;
            for (int j = 0; j < N - 1; ++j) {
                if (grid[i][j] && grid[i][j] == grid[i][j + 1]) {
                    moved = true;
                    for (int k = j + 1; k < N - 1; ++k) grid[i][k] = grid[i][k + 1];
                    grid[i][N - 1] = 0;
                    grid[i][j] *= 2;
                    --j;
                    if (j >= 0) --j;
                }
            }
        }
        if (moved && dis(gen) < probnew) addRandomNum();
        return moved;
    }
};

int main() {
    Board board;
    board.print();
    std::cout << "Use arrow keys to move" << std::endl;
    int ch = 0;
    for (;;) {
        if (_kbhit()) {
            if (ch == 224) {
                ch = _getch();
                switch (ch) {
                    case 75:
                        board.moveLeft();
                        break;
                    case 77:
                        board.moveRight();
                        break;
                    case 72:
                        board.moveUp();
                        break;
                    case 80:
                        board.moveDown();
                        break;
                    default:
                        continue;
                }
                signed status = board.won();
                if (status != 0) {
                    std::cout << "You " << (status == 1 ? "Win" : "Lose") << "!"
                              << std::endl;
                    break;
                }
                std::cout << "Press ESC to exit." << std::endl;
                continue;
            }
            ch = _getch();
            if (ch == 27) {
                std::cout << "Exit." << std::endl;
                break;
            }
        }
    }

    return 0;
}