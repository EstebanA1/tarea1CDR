#include <iostream>
#include <cstring>      // Para memset()
#include <sys/socket.h> // Para socket(), connect()
#include <arpa/inet.h>  // Para sockaddr_in, inet_addr()
#include <unistd.h>     // Para close()
#include <string>       // Para std::string

using namespace std;

void interactuarConServidor(int socket_fd) {
    const int buffer_size = 1024;
    char buffer[buffer_size];
    fd_set readfds;
    struct timeval tv;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(socket_fd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int activity = select(socket_fd + 1, &readfds, nullptr, nullptr, &tv);

        if (activity < 0) {
            cout << "Error en la selección." << endl;
            break;
        }

        if (FD_ISSET(socket_fd, &readfds)) {
            memset(buffer, 0, buffer_size);
            int len = recv(socket_fd, buffer, buffer_size - 1, 0);
            if (len > 0) {
                cout << buffer << endl;
            } else {
                cout << "Conexión cerrada por el servidor." << endl;
                break;
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            string input;
            getline(cin, input);

            if (!input.empty()) {
                send(socket_fd, input.c_str(), input.length(), 0);
                if (input == "exit") {
                    cout << "Cerrando la conexión..." << endl;
                    break;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <IP> <puerto>" << endl;
        return 1;
    }

    int socket_fd;
    struct sockaddr_in server;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        cout << "No se pudo crear el socket." << endl;
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));

    if (connect(socket_fd, reinterpret_cast<struct sockaddr *>(&server), sizeof(server)) < 0) {
        cout << "Conexión fallida." << endl;
        return 1;
    }

    cout << "Conectado al servidor en " << argv[1] << ":" << argv[2] << "." << endl;

    interactuarConServidor(socket_fd);

    close(socket_fd);
    cout << "Conexión cerrada." << endl;
    return 0;
}
