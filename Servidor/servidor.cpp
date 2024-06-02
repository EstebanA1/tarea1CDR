#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <thread>
#include <signal.h>
#include <cstring>

#define SOCKET_ERROR -1

const int ROWS = 6;
const int COLS = 7;
char board[ROWS][COLS];
int server_socket;
int client_socket;

// Señal para dar de baja el servidor
void handle_signal(int signal) {
    if (signal == SIGINT) {
        close(server_socket);
        close(client_socket);
        exit(0);
    }
}

void initializeBoard() {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            board[i][j] = ' ';
        }
    }
}

void printBoard() {
    std::cout << " ------------------\n";
    for (int i = 0; i < ROWS; ++i) {
        std::cout << i + 1 << " |";
        for (int j = 0; j < COLS; ++j) {
            std::cout << board[i][j] << "|";
        }
        std::cout << "\n";
    }
    std::cout << " ------------------\n";
    std::cout << "   1 2 3 4 5 6 7\n";
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
                board[i][j] = 'S';
                if (checkWin('S')) {
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
                board[i][j] = 'C';
                if (checkWin('C')) {
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
    std::string boardString;
    boardString += "\n ------------------\n";
    for (int i = 0; i < ROWS; ++i) {
        boardString += std::to_string(i + 1);
        boardString += " |";
        for (int j = 0; j < COLS; ++j) {
            boardString += board[i][j];
            boardString += "|";
        }
        boardString += "\n";
    }
    boardString += " ------------------\n";
    boardString += "   1 2 3 4 5 6 7\n";
    send(socket_cliente, boardString.c_str(), boardString.size(), 0);
}

void sendErrorMessage(int socket_cliente, const std::string& errorMsg) {
    std::string fullMessage = "Error: " + errorMsg + "\n";
    send(socket_cliente, fullMessage.c_str(), fullMessage.size(), 0);
}

void notifyNewGame(const std::string &ip, int puerto) {
    std::cout << "Juego nuevo [" << ip << ":" << puerto << "]\n";
}

void notifyGameStart(const std::string &ip, int puerto, bool isClient) {
    std::string role = isClient ? "cliente" : "servidor";
    std::cout << "Juego [" << ip << ":" << puerto << "]: inicia juego el " << role << ".\n";
}

void notifyMove(const std::string &ip, int puerto, char player, int column) {
    std::cout << "Juego [" << ip << ":" << puerto << "]: Cliente juega columna " << column + 1 << ".\n";
}

void notifyGameEnd(const std::string &ip, int puerto, const std::string &result) {
    std::cout << "Juego [" << ip << ":" << puerto << "]: " << result << ".\n";
}

void startGame(int socket_cliente, const std::string& ip_cliente, int puerto_cliente) {
    initializeBoard();
    srand(time(0));
    char currentPlayer = (rand() % 2 == 0) ? 'C' : 'S';

    // Si el cliente comienza, mostrar el tablero vacio
    if (currentPlayer == 'C') {
        sendBoardState(socket_cliente); 
    }

    while (true) {
        if (currentPlayer == 'C') {
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

            std::string input(buffer);
            input.erase(input.find_last_not_of(" \n\r\t") + 1); // Trim trailing whitespace

            int column;
            std::string errorMsg;
            try {
                column = std::stoi(input) - 1;
            } catch (const std::invalid_argument& e) {
                errorMsg = "Entrada inválida! Por favor, ingresa un número de columna válido (1-7).";
                sendErrorMessage(socket_cliente, errorMsg);
                continue;
            }

            if (column >= 0 && column < COLS) {
                if (dropPiece(column, currentPlayer)) {
                    sendBoardState(socket_cliente); // Mostrar el tablero después del movimiento del cliente
                    notifyMove(ip_cliente, puerto_cliente, currentPlayer, column);
                    if (checkWin(currentPlayer)) {
                        std::string winMsg = "¡Felicidades, has ganado!\n";
                        send(socket_cliente, winMsg.c_str(), winMsg.size(), 0);
                        notifyGameEnd(ip_cliente, puerto_cliente, "El cliente ha ganado");
                        break;
                    } else if (isBoardFull()) {
                        std::string drawMsg = "¡Empate!\n";
                        send(socket_cliente, drawMsg.c_str(), drawMsg.size(), 0);
                        notifyGameEnd(ip_cliente, puerto_cliente, "Empate");
                        break;
                    }
                    currentPlayer = 'S';
                } else {
                    errorMsg = "La columna está llena! Por favor, elige otra columna.";
                    sendErrorMessage(socket_cliente, errorMsg);
                }
            } else {
                errorMsg = "Entrada inválida! Por favor, ingresa un número de columna válido (1-7).";
                sendErrorMessage(socket_cliente, errorMsg);
            }
        } else {
            int cpuColumn = getCPUMove();
            if (dropPiece(cpuColumn, currentPlayer)) {
                sendBoardState(socket_cliente); // Mostrar el tablero después del movimiento del servidor
                if (checkWin(currentPlayer)) {
                    std::string cpuWinMsg = "¡La máquina ha ganado!\n";
                    send(socket_cliente, cpuWinMsg.c_str(), cpuWinMsg.size(), 0);
                    notifyGameEnd(ip_cliente, puerto_cliente, "El servidor ha ganado");
                    break;
                } else if (isBoardFull()) {
                    std::string drawMsg = "¡Empate!\n";
                    send(socket_cliente, drawMsg.c_str(), drawMsg.size(), 0);
                    notifyGameEnd(ip_cliente, puerto_cliente, "Empate");
                    break;
                }
                currentPlayer = 'C';
            }
        }
    }

    close(socket_cliente);
}

// Función para manejar la conexión del cliente
void handleClient(int socket_cliente, std::string ip, int puerto) {
    notifyNewGame(ip, puerto);
    notifyGameStart(ip, puerto, true);  // Cliente inicia
    send(socket_cliente, "Comienza el juego 4 en línea\n", 31, 0); // Mensaje inicial
    startGame(socket_cliente, ip, puerto); // Pasar la IP y el puerto del cliente
    notifyGameEnd(ip, puerto, "El juego ha terminado");
    std::cout << "El jugador " << ip << " ha cerrado la conexión" << std::endl;
    close(socket_cliente);
}

int main(int argc, char **argv) {
    signal(SIGINT, handle_signal);

    // Verificación del número de argumentos
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <puerto>" << std::endl;
        return 1;
    }

    // Convertir el argumento del puerto a entero
    int puerto = std::atoi(argv[1]);

    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == SOCKET_ERROR) {
        std::cerr << "Error al crear el socket del servidor" << std::endl;
        return 1;
    }

    // Configurar la dirección del servidor
    sockaddr_in direccion_servidor;
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccion_servidor.sin_port = htons(puerto);

    // Vincular el socket del servidor
    if (bind(server_socket, reinterpret_cast<const sockaddr *>(&direccion_servidor), sizeof(direccion_servidor)) == SOCKET_ERROR) {
        std::cerr << "Error al vincular el socket del servidor" << std::endl;
        close(server_socket);
        return 1;
    }

    // Poner el socket en modo de escucha
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error al poner el socket en modo de escucha" << std::endl;
        close(server_socket);
        return 1;
    }

    std::cout << "El servidor está escuchando en el puerto " << puerto << std::endl;

    // Aceptar conexiones entrantes
    while (true) {
        sockaddr_in direccion_cliente;
        socklen_t tamano_direccion_cliente = sizeof(direccion_cliente);
        client_socket = accept(server_socket, reinterpret_cast<sockaddr *>(&direccion_cliente), &tamano_direccion_cliente);
        if (client_socket == SOCKET_ERROR) {
            std::cerr << "Error al aceptar la conexión entrante" << std::endl;
            close(server_socket);
            return 1;
        }

        char *ip = inet_ntoa(direccion_cliente.sin_addr);
        int puerto_cliente = ntohs(direccion_cliente.sin_port);

        // Iniciar un hilo para manejar la conexión del cliente
        std::thread clientThread(handleClient, client_socket, std::string(ip), puerto_cliente);
        clientThread.detach();
    }

    // Cerrar el socket del servidor después de salir del bucle
    close(server_socket);

    return 0;
}
