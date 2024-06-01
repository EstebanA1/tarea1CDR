#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <string>
#include <signal.h>
#include <cstring>
#include "juego.h"

#define SOCKET_ERROR -1

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

// Parte donde se aceptan conexiones
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
