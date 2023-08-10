#include <iostream>
#include <cstring>
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

const int PORT = 8080;

int main() {
#ifdef _WIN32
  WSAData wsaData;
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    std::cerr << "WSAStartup failed: " << iResult << std::endl;
    return 1;
  }
#endif

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket");
    return 1;
  }

  sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("bind");
    return 1;
  }

  if (listen(server_fd, 1) == -1) {
    perror("listen");
    return 1;
  }

  sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(client_addr));
  socklen_t client_addr_size = sizeof(client_addr);
  int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_size);
  if (client_fd == -1) {
    perror("accept");
    return 1;
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

  return 0;
}
