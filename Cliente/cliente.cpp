#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define BUFFER_SIZE 1024

void playGame(int socket_fd) {
    char buffer[BUFFER_SIZE];
    while (true) {
        // Leer datos del servidor
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(socket_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            std::cout << "Conexión cerrada por el servidor." << std::endl;
            break;
        }
        buffer[bytes_read] = '\0';
        std::cout << buffer << std::endl;

        // Enviar datos al servidor
        std::string input;
        std::cout << "Ingresa el número de columna (1-7): ";
        std::cin >> input;
        input += '\n';
        write(socket_fd, input.c_str(), input.size());
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <IP> <puerto>" << std::endl;
        return 1;
    }

    const char *server_ip = argv[1];
    int server_port = std::stoi(argv[2]);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cerr << "Error al crear el socket." << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        std::cerr << "Dirección IP no válida." << std::endl;
        close(socket_fd);
        return 1;
    }

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error al conectar al servidor." << std::endl;
        close(socket_fd);
        return 1;
    }

    std::cout << "Conectado al servidor " << server_ip << ":" << server_port << std::endl;
    playGame(socket_fd);
    close(socket_fd);
    return 0;
}
