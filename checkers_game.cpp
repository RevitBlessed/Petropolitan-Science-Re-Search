#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <cmath>
#include <sstream>
#include <thread>
#include <mutex>

using namespace std;

class CheckersGame {
private:
    array<array<char, 8>, 8> board;
    bool isWhiteTurn;    // true - ход белых, false - ход черных
    bool humanIsWhite;   // выбранный игроком цвет

public:
    CheckersGame() : isWhiteTurn(true), humanIsWhite(true) {
        initializeBoard();
    }

    // Компьютерный ход (упрощённо – только одноступенчатые ходы)
    string makeComputerMove(bool isComputerWhite) {
        auto validMoves = getValidMoves(isComputerWhite);
        if (validMoves.empty()) {
            return "";
        }
        int randomIndex = rand() % validMoves.size();
        auto move = validMoves[randomIndex];
        string moveStr;
        moveStr += char('A' + move.first.second);
        moveStr += char('0' + (8 - move.first.first));
        moveStr += " ";
        moveStr += char('A' + move.second.second);
        moveStr += char('0' + (8 - move.second.first));
        array<array<char, 8>, 8> tempBoard;
        // Для компьютерного хода используем стандартное преобразование (от белых)
        simulateMove(moveStr, isComputerWhite, false, tempBoard);
        board = tempBoard;
        isWhiteTurn = !isWhiteTurn;
        return moveStr;
    }

    void initializeBoard() {
        for (auto &row : board) {
            row.fill('.');
        }
        // Расставляем чёрные шашки (верхние 3 ряда)
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 8; j++) {
                if ((i + j) % 2 == 1) {
                    board[i][j] = 'B';
                }
            }
        }
        // Расставляем белые шашки (нижние 3 ряда)
        for (int i = 5; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if ((i + j) % 2 == 1) {
                    board[i][j] = 'W';
                }
            }
        }
    }

    // Функция вывода доски.
    // Для белых вывод стандартный (верхняя строка – 8, столбцы A...H),
    // для чёрных – доска развёрнута на 180° (верхняя строка имеет метку 1, столбцы: H...A).
    void printBoard() const {
        if (humanIsWhite) {
            cout << "  A B C D E F G H\n";
            cout << "  ---------------\n";
            for (int i = 0; i < 8; i++) {
                cout << 8 - i << "|";
                for (int j = 0; j < 8; j++) {
                    cout << board[i][j] << " ";
                }
                cout << "|" << 8 - i << "\n";
            }
            cout << "  ---------------\n";
            cout << "  A B C D E F G H\n";
        } else {
            cout << "  H G F E D C B A\n";
            cout << "  ---------------\n";
            for (int i = 0; i < 8; i++) {
                int internalRow = 7 - i; // При развороте: printed row i+1 соответствует internal row = 7 - i.
                cout << i + 1 << "|";
                for (int j = 7; j >= 0; j--) {
                    cout << board[internalRow][j] << " ";
                }
                cout << "|" << i + 1 << "\n";
            }
            cout << "  ---------------\n";
            cout << "  H G F E D C B A\n";
        }
    }

    // Функция симуляции хода.
    // moveStr – строка вида "F4 D6 B4" (рубка) или "A3 B4" (простой ход).
    // isPlayerWhite – цвет игрока, делающего ход.
    // isHuman – true, если ход введён пользователем, false для компьютерного.
    // Если ход корректен, обновляется tempBoard.
    // Для дамок (X/Y) реализована возможность перемещаться по диагонали на любое расстояние.
    bool simulateMove(const string &moveStr, bool isPlayerWhite, bool isHuman, array<array<char, 8>, 8> &tempBoard) {
        tempBoard = board;
        vector<string> coords;
        stringstream ss(moveStr);
        string token;
        while (ss >> token) {
            coords.push_back(token);
        }
        if (coords.size() < 2) return false;
        
        // Функция преобразования строки в координаты.
        // Если isHuman==true и игрок играет чёрными, используем развёрнутое преобразование:
        // printed column letter: H G F E D C B A  -> internal col = 7 - ('H' - letter)
        // printed row number: X -> internal row = 8 - X.
        auto getCoord = [this, isHuman](const string &s, int &row, int &col) -> bool {
            if (s.size() < 2) return false;
            char letter = toupper(s[0]);
            char digit = s[1];
            if (isHuman && !humanIsWhite) {
                col = 7 - ('H' - letter); // Например, если letter == 'F', то 'H'-'F' = 2, col = 7 - 2 = 5.
                row = 8 - (digit - '0');    // Если digit == '6', row = 8 - 6 = 2.
            } else {
                col = letter - 'A';
                row = 8 - (digit - '0');
            }
            return isValidPosition(row, col);
        };

        int fromRow, fromCol, toRow, toCol;
        if (!getCoord(coords[0], fromRow, fromCol)) return false;
        char piece = tempBoard[fromRow][fromCol];
        if (isPlayerWhite) {
            if (!(piece == 'W' || piece == 'X')) return false;
        } else {
            if (!(piece == 'B' || piece == 'Y')) return false;
        }
        
        bool multiStep = (coords.size() > 2);
        for (size_t i = 1; i < coords.size(); i++) {
            if (!getCoord(coords[i], toRow, toCol)) return false;
            int rowDiff = toRow - fromRow;
            int colDiff = toCol - fromCol;
            if (abs(rowDiff) != abs(colDiff)) return false;
            if (piece == 'X' || piece == 'Y') {
                int dx = (rowDiff > 0) ? 1 : -1;
                int dy = (colDiff > 0) ? 1 : -1;
                int enemyCount = 0;
                int enemyRow = -1, enemyCol = -1;
                int curRow = fromRow + dx, curCol = fromCol + dy;
                while (curRow != toRow || curCol != toCol) {
                    if (tempBoard[curRow][curCol] != '.') {
                        bool friendly = false;
                        char p = tempBoard[curRow][curCol];
                        if (isPlayerWhite)
                            friendly = (p == 'W' || p == 'X');
                        else
                            friendly = (p == 'B' || p == 'Y');
                        if (friendly) return false;
                        else {
                            enemyCount++;
                            enemyRow = curRow;
                            enemyCol = curCol;
                        }
                    }
                    curRow += dx;
                    curCol += dy;
                }
                if (tempBoard[toRow][toCol] != '.') return false;
                if (enemyCount > 1) return false;
                if (enemyCount == 1) {
                    tempBoard[enemyRow][enemyCol] = '.';
                }
                tempBoard[toRow][toCol] = piece;
                tempBoard[fromRow][fromCol] = '.';
            } else {
                if (abs(rowDiff) == 1) {
                    if (isPlayerWhite && rowDiff >= 0) return false;
                    if (!isPlayerWhite && rowDiff <= 0) return false;
                    if (tempBoard[toRow][toCol] != '.') return false;
                    tempBoard[toRow][toCol] = piece;
                    tempBoard[fromRow][fromCol] = '.';
                } else if (abs(rowDiff) == 2) {
                    int jumpRow = (fromRow + toRow) / 2;
                    int jumpCol = (fromCol + toCol) / 2;
                    char jumped = tempBoard[jumpRow][jumpCol];
                    if (isPlayerWhite) {
                        if (!(jumped == 'B' || jumped == 'Y')) return false;
                    } else {
                        if (!(jumped == 'W' || jumped == 'X')) return false;
                    }
                    tempBoard[jumpRow][jumpCol] = '.';
                    if (tempBoard[toRow][toCol] != '.') return false;
                    tempBoard[toRow][toCol] = piece;
                    tempBoard[fromRow][fromCol] = '.';
                } else {
                    return false;
                }
            }
            fromRow = toRow;
            fromCol = toCol;
        }
        if (piece != 'X' && piece != 'Y') {
            if (isPlayerWhite && fromRow == 0)
                tempBoard[fromRow][fromCol] = 'X';
            else if (!isPlayerWhite && fromRow == 7)
                tempBoard[fromRow][fromCol] = 'Y';
        }
        return true;
    }

    // Параллельная реализация поиска допустимых ходов для компьютера.
    vector<pair<pair<int, int>, pair<int, int>>> getValidMoves(bool isWhitePlayer) {
        vector<pair<pair<int, int>, pair<int, int>>> validMoves;
        mutex mtx;
        vector<thread> threads;
        for (int row = 0; row < 8; row++) {
            threads.push_back(thread([&, row, isWhitePlayer]() {
                vector<pair<pair<int, int>, pair<int, int>>> localMoves;
                for (int col = 0; col < 8; col++) {
                    char piece = board[row][col];
                    if (isWhitePlayer) {
                        if (piece != 'W' && piece != 'X') continue;
                    } else {
                        if (piece != 'B' && piece != 'Y') continue;
                    }
                    int rowDir = isWhitePlayer ? -1 : 1;
                    vector<int> colDirs = {-1, 1};
                    for (int d : colDirs) {
                        int toRow = row + rowDir;
                        int toCol = col + d;
                        if (isValidPosition(toRow, toCol) && board[toRow][toCol] == '.')
                            localMoves.push_back({{row, col}, {toRow, toCol}});
                        int jumpRow = row + 2 * rowDir;
                        int jumpCol = col + 2 * d;
                        int midRow = row + rowDir;
                        int midCol = col + d;
                        if (isValidPosition(jumpRow, jumpCol) && isValidPosition(midRow, midCol) &&
                            board[jumpRow][jumpCol] == '.' &&
                            board[midRow][midCol] != '.' &&
                            board[midRow][midCol] != board[row][col])
                            localMoves.push_back({{row, col}, {jumpRow, jumpCol}});
                    }
                }
                lock_guard<mutex> lock(mtx);
                validMoves.insert(validMoves.end(), localMoves.begin(), localMoves.end());
            }));
        }
        for (auto &t : threads) {
            t.join();
        }
        return validMoves;
    }

    // Совершение хода.
    void makeMove(const string &moveStr, bool isPlayerWhite) {
        array<array<char, 8>, 8> tempBoard;
        if (!simulateMove(moveStr, isPlayerWhite, true, tempBoard)) {
            cout << "Invalid move, try again!" << endl;
            return;
        }
        board = tempBoard;
        isWhiteTurn = !isWhiteTurn;
    }

    void clearInputStream() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    void playGame() {
        cout << "Choose your color (W/B): ";
        char choice;
        cin >> choice;
        choice = toupper(choice);
        clearInputStream();
        bool isPlayerWhite = (choice == 'W');
        humanIsWhite = isPlayerWhite;
        cout << "You are playing as " << (isPlayerWhite ? "White" : "Black") << endl;
        while (true) {
            printBoard();
            if (isWhiteTurn == isPlayerWhite) {
                cout << "Your move (e.g., A3 B4 [or multi-capture: F4 D6 B4]): ";
                string moveStr;
                getline(cin, moveStr);
                if (moveStr.empty())
                    continue;
                makeMove(moveStr, isPlayerWhite);
            } else {
                string computerMove = makeComputerMove(!isPlayerWhite);
                if (computerMove.empty()) {
                    cout << "Computer has no valid moves. You win!" << endl;
                    break;
                }
                cout << "Computer moved: " << computerMove << endl;
            }
        }
    }

private:
    bool isValidPosition(int row, int col) const {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }
};

int main() {
    srand(time(0));
    CheckersGame game;
    game.playGame();
    return 0;
}
