#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <string>
 
#define SOCKET_ERROR -1

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
 std::cout << " ---------------\n";
 for (int i = 0; i < ROWS; ++i) {
 std::cout << i + 1 << " |";
 for (int j = 0; j < COLS; ++j) {
 std::cout << board[i][j] << "|";
 }
 std::cout << "\n";
 }
 std::cout << " ---------------\n";
 std::cout << " 1 2 3 4 5 6 7\n";
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
 // Comprobar filas
 for (int i = 0; i < ROWS; ++i) {
 for (int j = 0; j < COLS - 3; ++j) {
 if (board[i][j] == player && board[i][j + 1] == player && board[i][j + 2] == player && board[i][j + 3] == player) {
 return true;
 }
 }
 }
 // Comprobar columnas
 for (int i = 0; i < ROWS - 3; ++i) {
 for (int j = 0; j < COLS; ++j) {
 if (board[i][j] == player && board[i + 1][j] == player && board[i + 2][j] == player && board[i + 3][j] == player) {
 return true;
 }
 }
 }
 // Comprobar diagonales descendentes
 for (int i = 0; i < ROWS - 3; ++i) {
 for (int j = 0; j < COLS - 3; ++j) {
 if (board[i][j] == player && board[i + 1][j + 1] == player && board[i + 2][j + 2] == player && board[i + 3][j + 3] == player) {
 return true;
 }
 }
 }
 // Comprobar diagonales ascendentes
 for (int i = 3; i < ROWS; ++i) {
 for (int j = 0; j < COLS - 3; ++j) {
 if (board[i][j] == player && board[i - 1][j + 1] == player && board[i - 2][j + 2] == player && board[i - 3][j + 3] == player) {
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
 return false; // La columna está llena
}

int getCPUMove() {
 // Verificar si la CPU puede ganar en el próximo turno
 for (int j = 0; j < COLS; ++j) {
 for (int i = ROWS - 1; i >= 0; --i) {
 if (board[i][j] == ' ') {
 board[i][j] = 'O'; // Intentar colocar la ficha de la CPU
 if (checkWin('O')) {
 board[i][j] = ' '; // Quitar la ficha de la CPU
 return j;
 }
 board[i][j] = ' '; // Quitar la ficha de la CPU
 break;
 }
 }
 }
 // Verificar si el jugador puede ganar en el próximo turno y bloquear ese movimiento
 for (int j = 0; j < COLS; ++j) {
 for (int i = ROWS - 1; i >= 0; --i) {
 if (board[i][j] == ' ') {
 board[i][j] = 'X'; // Intentar colocar la ficha del jugador
 if (checkWin('X')) {
 board[i][j] = ' '; // Quitar la ficha del jugador
 return j;
 }
 board[i][j] = ' '; // Quitar la ficha del jugador
 break;
 }
 }
 }
 // Si no se encontró ninguna jugada ganadora o bloqueadora, elegir una columna al azar
 return rand() % COLS;
}

void startGame() {
 initializeBoard();
 srand(time(0));

 char currentPlayer = (rand() % 2 == 0) ? 'X' : 'O'; // Inicialización aleatoria del primer jugador
 std::cout << "Bienvenido al juego 4 en línea\n";
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
 std::cout << "Turno de la máquina...\n";
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
 std::cout << "La maquina ha ganado!\n";
 }
 break;
 }

 if (isBoardFull()) {
 printBoard();
 std::cout << "Empate!\n";
 break;
 }

 currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
 }
}

void notifyNewGame(const std::string& ip, int puerto) {
 std::cout << "Juego nuevo [" << ip << ":" << puerto << "]\n";
}

void notifyGameStart(const std::string& ip, int puerto, bool isClient) {
 std::string role = isClient ? "cliente" : "servidor";
 std::cout << "Juego [" << ip << ":" << puerto << "]: inicia juego el " << role << ".\n";
}

void notifyMove(const std::string& ip, int puerto, char player, int column) {
 std::cout << "Juego [" << ip << ":" << puerto << "]: " << player << " juega columna " << column + 1 << ".\n";
}

void notifyGameEnd(const std::string& ip, int puerto, const std::string& result) {
 std::cout << "Juego [" << ip << ":" << puerto << "]: " << result << ".\n";
}


void sendBoardState(int socket_cliente) {
 std::string boardString;
 for (int i = 0; i < ROWS; ++i) {
 for (int j = 0; j < COLS; ++j) {
 boardString += board[i][j];
 }
 boardString += '\n'; // Agregar un salto de línea al final de cada fila
 }
 send(socket_cliente, boardString.c_str(), boardString.size(), 0);
}





void handleClient(int socket_cliente, std::string ip, int puerto) {
 notifyNewGame(ip, puerto);

 // Iniciar el juego y establecer la conexión con el cliente
 notifyGameStart(ip, puerto, false);

 // Implementar la lógica del juego aquí
 startGame();
 
 // Envío y recepción de datos con el cliente
 sendBoardState(socket_cliente); // Enviar el estado inicial del tablero al cliente

 char buffer[1024];
 int bytes_recibidos;
 while ((bytes_recibidos = recv(socket_cliente, buffer, sizeof(buffer), 0)) > 0) {
 buffer[bytes_recibidos] = '\0'; // Asegurarse de que el buffer es una cadena válida

 // Imprimir los datos recibidos del cliente
 std::cout << "Datos recibidos del cliente [" << ip << ":" << puerto << "]: " << buffer << std::endl;

 int column = atoi(buffer); // Convertir la entrada a un número
 if (column >= 1 && column <= 7) {
 // dropPiece devuelve true si el jugador gana
 if (dropPiece(column, 'X')) {
 notifyGameEnd(ip, puerto, "El jugador ha ganado"); // Notificar al cliente
 break;
 }
 // Realizar el movimiento de la CPU
 int cpuColumn = getCPUMove();
 if (!dropPiece(cpuColumn, 'O')) {
 std::cerr << "Error: La columna de la CPU está llena!\n";
 break;
 }
 notifyMove(ip, puerto, 'O', cpuColumn); // Notificar al cliente
 if (checkWin('O')) {
 notifyGameEnd(ip, puerto, "La CPU ha ganado"); // Notificar al cliente
 break;
 }
 sendBoardState(socket_cliente); // Enviar el estado actualizado del tablero al cliente
 }
 }

 if (bytes_recibidos == 0) {
 std::cout << "El jugador " << ip << " ha cerrado la conexión" << std::endl;
 } else if (bytes_recibidos == SOCKET_ERROR) {
 std::cerr << "Error al recibir datos del jugador " << ip << std::endl;
 }
 close(socket_cliente);
}








int main(int argc, char **argv) {
 if (argc != 2) {
 std::cerr << "Uso: " << argv[0] << " <puerto>" << std::endl;
 return 1;
 }

 int puerto = std::atoi(argv[1]);

 // Crear socket del servidor
 int socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
 if (socket_servidor == SOCKET_ERROR) {
 std::cerr << "Error al crear el socket del servidor" << std::endl;
 return 1;
 }

 // Configurar dirección del servidor
 sockaddr_in direccion_servidor;
 direccion_servidor.sin_family = AF_INET;
 direccion_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
 direccion_servidor.sin_port = htons(puerto);

 // Vincular el socket
 if (bind(socket_servidor, reinterpret_cast<const sockaddr *>(&direccion_servidor), sizeof(direccion_servidor)) == SOCKET_ERROR) {
 std::cerr << "Error al vincular el socket del servidor" << std::endl;
 close(socket_servidor);
 return 1;
 }

 // Escuchar conexiones entrantes
 if (listen(socket_servidor, SOMAXCONN) == SOCKET_ERROR) {
 std::cerr << "Error al poner el socket en modo de escucha" << std::endl;
 close(socket_servidor);
 return 1;
 }

 std::cout << "El servidor está escuchando en el puerto " << puerto << std::endl;

 while (true) {
 sockaddr_in direccion_cliente;
 socklen_t tamano_direccion_cliente = sizeof(direccion_cliente);
 int socket_cliente = accept(socket_servidor, reinterpret_cast<sockaddr *>(&direccion_cliente), &tamano_direccion_cliente);
 if (socket_cliente == SOCKET_ERROR) {
 std::cerr << "Error al aceptar la conexión entrante" << std::endl;
 close(socket_servidor);
 return 1;
 }

 char *ip = inet_ntoa(direccion_cliente.sin_addr); // Usar inet_ntoa en lugar de inet_ntop

 std::thread clientThread(handleClient, socket_cliente, std::string(ip), puerto);
 clientThread.detach();
 }

 // Cerrar el socket del servidor al finalizar (esto no se ejecutará en el bucle infinito)
 close(socket_servidor);

 return 0;
}