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

  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    perror("socket");
    return 1;
  }

  sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
    perror("inet_pton");
    return 1;
  }

  if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("connect");
    return 1;
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

  return 0;
}
