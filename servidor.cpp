#include "juego.cpp"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")

SOCKET socket_servidor; // Definir socket_servidor en el ámbito global

std::string boardToString()
{
    std::string boardString;
    for (int i = 0; i < ROWS; ++i)
    {
        for (int j = 0; j < COLS; ++j)
        {
            boardString += board[i][j];
        }
        boardString += '\n'; // Agregar un salto de línea al final de cada fila
    }
    return boardString;
}

void handleClient(SOCKET socket_cliente, std::string ip)
{
    startGame();

    std::string boardString = boardToString();
    send(socket_cliente, boardString.c_str(), boardString.size(), 0);

    char buffer[1024];
    int bytes_recibidos;
    while ((bytes_recibidos = recv(socket_cliente, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[bytes_recibidos] = '\0';   // Asegurarse de que el buffer es una cadena válida
        std::cout << buffer << std::endl; // Imprimir el mensaje recibido
        int column = atoi(buffer);        // Convertir la entrada a un número
        if (column >= 1 && column <= 7)
        {
            // dropPiece devuelve true si el jugador gana
            if (dropPiece(column, 'X'))
            {
                system("cls"); // Limpiar la terminal
                printBoard();  // Imprimir el tablero
                std::cout << "jugador " << ip << ", ha ganado" << std::endl;
                break;
            }
            std::cout << "jugador " << ip << ", dejo una ficha en columna " << column << std::endl;
            if (checkWin('X'))
            {
                system("cls"); // Limpiar la terminal
                printBoard();  // Imprimir el tablero
                std::cout << "jugador " << ip << ", ha ganado" << std::endl;
                break;
            }
            system("cls"); // Limpiar la terminal
            printBoard();  // Imprimir el tablero

            // Aquí es donde debes agregar las líneas para enviar el tablero actualizado al cliente
            boardString = boardToString();
            send(socket_cliente, boardString.c_str(), boardString.size(), 0);
        }
    }
    if (bytes_recibidos == 0)
    {
        std::cout << "El jugador " << ip << " ha cerrado la conexión" << std::endl;
    }
    else if (bytes_recibidos == SOCKET_ERROR)
    {
        std::cerr << "Error al recibir datos del jugador " << ip << std::endl;
    }
    closesocket(socket_cliente);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <puerto>" << std::endl;
        return 1;
    }

    int puerto = std::atoi(argv[1]);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Error al inicializar Winsock" << std::endl;
        return 1;
    }

    // Crear socket del servidor
    SOCKET socket_servidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_servidor == INVALID_SOCKET)
    {
        std::cerr << "Error al crear el socket del servidor" << std::endl;
        WSACleanup();
        return 1;
    }

    // Configurar dirección del servidor
    sockaddr_in direccion_servidor;
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccion_servidor.sin_port = htons(puerto);

    // Vincular el socket
    if (bind(socket_servidor, reinterpret_cast<const sockaddr *>(&direccion_servidor), sizeof(direccion_servidor)) == SOCKET_ERROR)
    {
        std::cerr << "Error al vincular el socket del servidor" << std::endl;
        closesocket(socket_servidor);
        WSACleanup();
        return 1;
    }

    // Escuchar conexiones entrantes
    if (listen(socket_servidor, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Error al poner el socket en modo de escucha" << std::endl;
        closesocket(socket_servidor);
        WSACleanup();
        return 1;
    }

    std::cout << "El servidor está escuchando en el puerto " << puerto << std::endl;

    while (true)
    {
        sockaddr_in direccion_cliente;
        int tamano_direccion_cliente = sizeof(direccion_cliente);
        SOCKET socket_cliente = accept(socket_servidor, reinterpret_cast<sockaddr *>(&direccion_cliente), &tamano_direccion_cliente);
        if (socket_cliente == INVALID_SOCKET)
        {
            std::cerr << "Error al aceptar la conexión entrante" << std::endl;
            closesocket(socket_servidor);
            WSACleanup();
            return 1;
        }

        char *ip = inet_ntoa(direccion_cliente.sin_addr); // Usar inet_ntoa en lugar de inet_ntop

        std::thread clientThread(handleClient, socket_cliente, ip);
        clientThread.detach();
    }

    // Cerrar el socket del servidor al finalizar (esto no se ejecutará en el bucle infinito)
    closesocket(socket_servidor);
    WSACleanup();

    return 0;
}