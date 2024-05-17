#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <string>

void initializeBoard();
void printBoard();
bool isBoardFull();
bool checkWin(char player);
bool dropPiece(int col, char player);
int getCPUMove();
void sendBoardState(int socket_cliente);
void startGame(int socket_cliente);
void notifyNewGame(const std::string &ip, int puerto);
void notifyGameStart(const std::string &ip, int puerto, bool isClient);
void notifyMove(const std::string &ip, int puerto, char player, int column);
void notifyGameEnd(const std::string &ip, int puerto, const std::string &result);

#endif // GAME_LOGIC_H
