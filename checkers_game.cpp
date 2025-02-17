#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;

class CheckersGame {
private:
    array<array<char, 8>, 8> board;
    bool isWhiteTurn;

    struct Move {
        int fromRow, fromCol, toRow, toCol;
        bool isCapture;
        vector<pair<int, int>> capturedPieces;
    };

public:
    CheckersGame() : isWhiteTurn(true) {
        initializeBoard();
    }

    void initializeBoard() {
        for (auto& row : board) {
            row.fill('.');
        }
        // Расставляем черные шашки (B)
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 8; j++) {
                if ((i + j) % 2 == 1) {
                    board[i][j] = 'B';
                }
            }
        }
        // Расставляем белые шашки (W)
        for (int i = 5; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if ((i + j) % 2 == 1) {
                    board[i][j] = 'W';
                }
            }
        }
    }

    void printBoard() const {
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
    }

    bool isValidMove(const string& moveStr, bool isPlayerWhite) {
        if (moveStr.length() != 5 || moveStr[2] != ' ') {
            return false;
        }

        int fromCol = toupper(moveStr[0]) - 'A';
        int fromRow = 8 - (moveStr[1] - '0');
        int toCol = toupper(moveStr[3]) - 'A';
        int toRow = 8 - (moveStr[4] - '0');

        if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol)) {
            return false;
        }

        char piece = board[fromRow][fromCol];
        bool isKing = (piece == 'K' || piece == 'Q');
        
        // Проверяем, что это шашка правильного цвета
        if (!isKing && piece != (isPlayerWhite ? 'W' : 'B')) {
            return false;
        }
        if (isKing && ((isPlayerWhite && piece != 'K') || (!isPlayerWhite && piece != 'Q'))) {
            return false;
        }

        // Проверяем направление движения
        int rowDiff = toRow - fromRow;
        int colDiff = toCol - fromCol;

        // Базовая проверка диагонального движения
        if (abs(rowDiff) != abs(colDiff)) {
            return false;
        }

        // Проверка правильного направления движения для обычных шашек
        if (!isKing) {
            if (isPlayerWhite && rowDiff >= 0) { // Белые двигаются вверх
                return false;
            }
            if (!isPlayerWhite && rowDiff <= 0) { // Черные двигаются вниз
                return false;
            }
        }

        // Для дамок - проверяем путь до цели
        if (isKing) {
            int rowStep = (rowDiff > 0) ? 1 : -1;
            int colStep = (colDiff > 0) ? 1 : -1;
            int row = fromRow + rowStep;
            int col = fromCol + colStep;
            bool foundCapture = false;
            pair<int, int> capturePos;

            while (row != toRow && col != toCol) {
                if (board[row][col] != '.') {
                    if (foundCapture) {
                        return false; // Больше одной шашки на пути
                    }
                    // Проверяем, что это шашка противника
                    char pieceOnWay = board[row][col];
                    if ((isPlayerWhite && (pieceOnWay == 'B' || pieceOnWay == 'Q')) ||
                        (!isPlayerWhite && (pieceOnWay == 'W' || pieceOnWay == 'K'))) {
                        foundCapture = true;
                        capturePos = {row, col};
                    } else {
                        return false;
                    }
                }
                row += rowStep;
                col += colStep;
            }

            // Если нашли взятие, проверяем что после него нет других шашек
            if (foundCapture) {
                row = capturePos.first + rowStep;
                col = capturePos.second + colStep;
                while (row != toRow && col != toCol) {
                    if (board[row][col] != '.') {
                        return false;
                    }
                    row += rowStep;
                    col += colStep;
                }
            }

            return board[toRow][toCol] == '.';
        }

        // Для обычных шашек - проверка на простой ход
        if (abs(rowDiff) == 1) {
            return board[toRow][toCol] == '.';
        }

        // Для обычных шашек - проверка на взятие
        if (abs(rowDiff) == 2) {
            int jumpRow = (fromRow + toRow) / 2;
            int jumpCol = (fromCol + toCol) / 2;
            char jumpPiece = board[jumpRow][jumpCol];
            bool isEnemyPiece = (isPlayerWhite && (jumpPiece == 'B' || jumpPiece == 'Q')) ||
                               (!isPlayerWhite && (jumpPiece == 'W' || jumpPiece == 'K'));
            return board[toRow][toCol] == '.' && jumpPiece != '.' && isEnemyPiece;
        }

        return false;
    }

        vector<pair<pair<int, int>, pair<int, int>>> getValidMoves(bool isWhitePlayer) {
        vector<pair<pair<int, int>, pair<int, int>>> validMoves;
        
        for (int fromRow = 0; fromRow < 8; fromRow++) {
            for (int fromCol = 0; fromCol < 8; fromCol++) {
                // Проверяем, что это шашка нужного цвета
                if (board[fromRow][fromCol] != (isWhitePlayer ? 'W' : 'B')) {
                    continue;
                }
                
                // Направления движения
                int rowDir = isWhitePlayer ? -1 : 1;
                vector<int> colDirs = {-1, 1};
                
                // Простые ходы
                for (int colDir : colDirs) {
                    int toRow = fromRow + rowDir;
                    int toCol = fromCol + colDir;
                    
                    if (isValidPosition(toRow, toCol) && board[toRow][toCol] == '.') {
                        validMoves.push_back({{fromRow, fromCol}, {toRow, toCol}});
                    }
                }
                
                // Ходы со взятием
                for (int colDir : colDirs) {
                    int toRow = fromRow + 2 * rowDir;
                    int toCol = fromCol + 2 * colDir;
                    int jumpRow = fromRow + rowDir;
                    int jumpCol = fromCol + colDir;
                    
                    if (isValidPosition(toRow, toCol) && isValidPosition(jumpRow, jumpCol) &&
                        board[toRow][toCol] == '.' &&
                        board[jumpRow][jumpCol] != '.' &&
                        board[jumpRow][jumpCol] != board[fromRow][fromCol] &&
                        ((board[fromRow][fromCol] == 'W' && board[jumpRow][jumpCol] == 'B') ||
                         (board[fromRow][fromCol] == 'B' && board[jumpRow][jumpCol] == 'W'))) {
                        validMoves.push_back({{fromRow, fromCol}, {toRow, toCol}});
                    }
                }
            }
        }
        
        return validMoves;
    }

    void makeMove(const string& moveStr, bool isPlayerWhite) {
        int fromCol = toupper(moveStr[0]) - 'A';
        int fromRow = 8 - (moveStr[1] - '0');
        int toCol = toupper(moveStr[3]) - 'A';
        int toRow = 8 - (moveStr[4] - '0');

        char piece = board[fromRow][fromCol];
        bool isKing = (piece == 'K' || piece == 'Q');

        // Если это взятие для обычной шашки
        if (!isKing && abs(toRow - fromRow) == 2) {
            int jumpRow = (fromRow + toRow) / 2;
            int jumpCol = (fromCol + toCol) / 2;
            board[jumpRow][jumpCol] = '.';
        }
        // Если это взятие для дамки
        else if (isKing && abs(toRow - fromRow) > 1) {
            int rowStep = (toRow > fromRow) ? 1 : -1;
            int colStep = (toCol > fromCol) ? 1 : -1;
            int row = fromRow + rowStep;
            int col = fromCol + colStep;
            while (row != toRow && col != toCol) {
                if (board[row][col] != '.') {
                    board[row][col] = '.';
                    break;
                }
                row += rowStep;
                col += colStep;
            }
        }

        // Перемещаем шашку
        board[toRow][toCol] = board[fromRow][fromCol];
        board[fromRow][fromCol] = '.';

        // Проверяем, нужно ли превратить шашку в дамку
        if (!isKing) {
            // Белая шашка достигла верхнего края
            if (isPlayerWhite && toRow == 0) {
                board[toRow][toCol] = 'K';
            }
            // Черная шашка достигла нижнего края
            else if (!isPlayerWhite && toRow == 7) {
                board[toRow][toCol] = 'Q';
            }
        }

        isWhiteTurn = !isWhiteTurn;
    }

    string makeComputerMove(bool isComputerWhite) {
        auto validMoves = getValidMoves(isComputerWhite);
        
        if (validMoves.empty()) {
            return "";
        }
        
        // Выбираем случайный ход
        int randomIndex = rand() % validMoves.size();
        auto move = validMoves[randomIndex];
        
        // Преобразуем ход в строковый формат
        string moveStr;
        moveStr += 'A' + move.first.second; // fromCol
        moveStr += '0' + (8 - move.first.first); // fromRow
        moveStr += ' ';
        moveStr += 'A' + move.second.second; // toCol
        moveStr += '0' + (8 - move.second.first); // toRow
        
        makeMove(moveStr, isComputerWhite);
        return moveStr;
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
        cout << "You are playing as " << (isPlayerWhite ? "White" : "Black") << endl;

        while (true) {
            printBoard();
            
            if (isWhiteTurn == isPlayerWhite) {
                cout << "Your move (e.g., A3 B4): ";
                string moveStr;
                getline(cin, moveStr);

                if (moveStr.length() != 5) {
                    cout << "Invalid input format. Use format 'A3 B4'" << endl;
                    continue;
                }

                if (isValidMove(moveStr, isPlayerWhite)) {
                    makeMove(moveStr, isPlayerWhite);
                } else {
                    cout << "Invalid move, try again!" << endl;
                }
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
    srand(time(0)); // Инициализация генератора случайных чисел
    CheckersGame game;
    game.playGame();
    return 0;
}