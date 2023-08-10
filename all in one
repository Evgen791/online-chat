#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

const int PORT = 2000;

void run_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        return;
    }

    if (listen(server_fd, 1) == -1) {
        perror("listen");
        return;
    }

    sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_size = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_fd == -1) {
        perror("accept");
        return;
    }

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int recv_len = recv(client_fd, buffer, sizeof(buffer), 0);
        if (recv_len <= 0) break;
        std::cout << "Client: " << buffer << std::endl;
        send(client_fd, buffer, recv_len, 0);
    }

#ifdef _WIN32
    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
#else
    close(client_fd);
    close(server_fd);
#endif
}

void run_client() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return;
    }

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        return;
    }

    char buffer[1024];
    while (true) {
        std::cout << "Enter message: ";
        std::cin.getline(buffer, sizeof(buffer));

        if (std::cin.eof()) {
            break;
        }

        send(sock_fd, buffer, strlen(buffer), 0);
        int recv_len = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (recv_len <= 0) break;
        buffer[recv_len] = '\0';
        std::cout << "Server: " << buffer << std::endl;
    }

#ifdef _WIN32
    closesocket(sock_fd);
    WSACleanup();
#else
    close(sock_fd);
#endif
}

int main() {
#ifdef _WIN32
    WSAData wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }
#endif

    std::thread server_thread(run_server);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread client_thread(run_client);

    server_thread.join();
    client_thread.join();

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
