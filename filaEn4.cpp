#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

const int ROWS = 6;
const int COLS = 7;

char board[ROWS][COLS];

void initializeBoard() {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            board[i][j] = ' ';
        }
    }
}

void printBoard() {
    std::cout << " 1 2 3 4 5 6 7\n";
    std::cout << "---------------\n";
    for (int i = 0; i < ROWS; ++i) {
        std::cout << "|";
        for (int j = 0; j < COLS; ++j) {
            std::cout << board[i][j] << "|";
        }
        std::cout << "\n";
    }
    std::cout << "---------------\n";
}

bool isBoardFull() {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

bool checkWin(char player) {
    // Check horizontal
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i][j+1] == player && board[i][j+2] == player && board[i][j+3] == player) {
                return true;
            }
        }
    }
    // Check vertical
    for (int i = 0; i < ROWS - 3; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (board[i][j] == player && board[i+1][j] == player && board[i+2][j] == player && board[i+3][j] == player) {
                return true;
            }
        }
    }
    // Check diagonal (top-left to bottom-right)
    for (int i = 0; i < ROWS - 3; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i+1][j+1] == player && board[i+2][j+2] == player && board[i+3][j+3] == player) {
                return true;
            }
        }
    }
    // Check diagonal (bottom-left to top-right)
    for (int i = 3; i < ROWS; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i-1][j+1] == player && board[i-2][j+2] == player && board[i-3][j+3] == player) {
                return true;
            }
        }
    }
    return false;
}

bool dropPiece(int col, char player) {
    for (int i = ROWS - 1; i >= 0; --i) {
        if (board[i][col] == ' ') {
            board[i][col] = player;
            return true;
        }
    }
    return false; 
}

int getCPUMove() {
    return rand() % COLS;
}

int main() {
    initializeBoard();
    srand(time(0)); 

    char currentPlayer = 'X'; 
    std::cout << "Bienvenido al juego 4 en linea\n";
    std::cout << "Juegas como '" << currentPlayer << "'\n";

    while (true) {
        printBoard();

        int column;
        if (currentPlayer == 'X') {
            std::cout << "Es tu turno. Ingresa el numero de columna donde deseas colocar tu ficha (1-7): ";
            std::cin >> column;
            if (std::cin.fail() || column < 1 || column > 7) {
                std::cout << "Entrada invalida! Por favor ingresa un numero de columna valido (1-7).\n";
                std::cin.clear(); 
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                continue;
            }
            column--; 
        } else {
            std::cout << "Turno de la maquina...\n";
            column = getCPUMove();
        }

        if (!dropPiece(column, currentPlayer)) {
            std::cout << "La columna esta llena! Escoge otra.\n";
            continue;
        }

        if (checkWin(currentPlayer)) {
            printBoard();
            if (currentPlayer == 'X') {
                std::cout << "Felicidades, has ganado!\n";
            } else {
                std::cout << "La CPU ha ganado!\n";
            }
            break;
        }

        if (isBoardFull()) {
            printBoard();
            std::cout << "El juego termino en empate!\n";
            break;
        }

        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; 
    }

    return 0;
}
