#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstring>
#include "juego.h"

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
    
    std::cout << " ---------------------\n"; // Borde superior agregado
    for (int i = 0; i < ROWS; ++i) {
        std::cout << i + 1 << " |";
        for (int j = 0; j < COLS; ++j) {
            std::cout << board[i][j] << "|";
        }
        std::cout << "\n";
    }
    std::cout << " ---------------------\n";
    std::cout << "  1  2  3  4  5  6  7\n";
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
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i][j + 1] == player && board[i][j + 2] == player && board[i][j + 3] == player) {
                return true;
            }
        }
    }
    for (int i = 0; i < ROWS - 3; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (board[i][j] == player && board[i + 1][j] == player && board[i + 2][j] == player && board[i + 3][j] == player) {
                return true;
            }
        }
    }
    for (int i = 0; i < ROWS - 3; ++i) {
        for (int j = 0; j < COLS - 3; ++j) {
            if (board[i][j] == player && board[i + 1][j + 1] == player && board[i + 2][j + 2] == player && board[i + 3][j + 3] == player) {
                return true;
            }
            if (board[i + 3][j] == player && board[i + 2][j + 1] == player && board[i + 1][j + 2] == player && board[i][j + 3] == player) {
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
    for (int j = 0; j < COLS; ++j) {
        for (int i = ROWS - 1; i >= 0; --i) {
            if (board[i][j] == ' ') {
                board[i][j] = 'O';
                if (checkWin('O')) {
                    board[i][j] = ' ';
                    return j;
                }
                board[i][j] = ' ';
                break;
            }
        }
    }
    for (int j = 0; j < COLS; ++j) {
        for (int i = ROWS - 1; i >= 0; --i) {
            if (board[i][j] == ' ') {
                board[i][j] = 'X';
                if (checkWin('X')) {
                    board[i][j] = ' ';
                    return j;
                }
                board[i][j] = ' ';
                break;
            }
        }
    }
    return rand() % COLS;
}

void sendBoardState(int socket_cliente) {
    std::string clearScreen = "\033[H\033[J"; // ANSI escape code to clear screen and move cursor to top
    send(socket_cliente, clearScreen.c_str(), clearScreen.size(), 0);

    std::string boardString;
    boardString += " ---------------------\n"; // Borde superior agregado
    for (int i = 0; i < ROWS; ++i) {
        boardString += std::to_string(i + 1);
        boardString += " |";
        for (int j = 0; j < COLS; ++j) {
            boardString += board[i][j];
            boardString += "|";
        }
        boardString += "\n";
    }
    boardString += " ---------------------\n";
    boardString += "  1  2  3  4  5  6  7\n";
    send(socket_cliente, boardString.c_str(), boardString.size(), 0);
}



void startGame(int socket_cliente) {
    initializeBoard();
    srand(time(0));
    char currentPlayer = (rand() % 2 == 0) ? 'X' : 'O';

    while (true) {
        sendBoardState(socket_cliente);

        if (currentPlayer == 'X') {
            char buffer[1024];
            int bytes_recibidos = recv(socket_cliente, buffer, sizeof(buffer), 0);
            if (bytes_recibidos == -1) {
                std::cerr << "Error al recibir datos del cliente" << std::endl;
                close(socket_cliente);
                return;
            }
            if (bytes_recibidos == 0) {
                std::cout << "El cliente cerró la conexión" << std::endl;
                close(socket_cliente);
                return;
            }
            buffer[bytes_recibidos] = '\0';
            int column = atoi(buffer) - 1;

            if (column >= 0 && column < COLS) {
                if (dropPiece(column, currentPlayer)) {
                    if (checkWin(currentPlayer)) {
                        sendBoardState(socket_cliente);
                        send(socket_cliente, "¡Felicidades, has ganado!\n", 26, 0);
                        break;
                    } else if (isBoardFull()) {
                        sendBoardState(socket_cliente);
                        send(socket_cliente, "¡Empate!\n", 9, 0);
                        break;
                    }
                    currentPlayer = 'O';
                } else {
                    send(socket_cliente, "¡La columna está llena! Escoge otra.\n", 38, 0);
                }
            } else {
                send(socket_cliente, "Entrada inválida! Por favor, ingresa un número de columna válido (1-7).\n", 67, 0);
            }
        } else {
            int cpuColumn = getCPUMove();
            if (dropPiece(cpuColumn, currentPlayer)) {
                if (checkWin(currentPlayer)) {
                    sendBoardState(socket_cliente);
                    send(socket_cliente, "¡La máquina ha ganado!\n", 25, 0);
                    break;
                } else if (isBoardFull()) {
                    sendBoardState(socket_cliente);
                    send(socket_cliente, "¡Empate!\n", 9, 0);
                    break;
                }
                currentPlayer = 'X';
            }
        }
    }
    close(socket_cliente);
}

void notifyNewGame(const std::string &ip, int puerto) {
    std::cout << "Juego nuevo [" << ip << ":" << puerto << "]\n";
}

void notifyGameStart(const std::string &ip, int puerto, bool isClient) {
    std::string role = isClient ? "cliente" : "servidor";
    std::cout << "Juego [" << ip << ":" << puerto << "]: inicia juego el " << role << ".\n";
}

void notifyMove(const std::string &ip, int puerto, char player, int column) {
    std::cout << "Juego [" << ip << ":" << puerto << "]: " << player << " juega columna " << column + 1 << ".\n";
}

void notifyGameEnd(const std::string &ip, int puerto, const std::string &result) {
    std::cout << "Juego [" << ip << ":" << puerto << "]: " << result << ".\n";
}
