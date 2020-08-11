// [ Guard ]
#pragma once

// [ Dependencies ]
#include <peanats/peanats.h>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

PEANATS_NAMESPACE_BEGIN

class Client :
  public Peanats
{
  using Peanats::Peanats;

  size_t tcp_connect()
  {
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
      logn("WSAStartup failed with error: " + iResult);
      return Error;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(server_ip.c_str(), server_port.c_str(), &hints, &result);
    if (iResult != 0) {
      logn("getaddrinfo failed with error: " + std::to_string(iResult));
      WSACleanup();
      return peanats::Error;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
      // Create a SOCKET for connecting to server
      ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
      if (ConnectSocket == INVALID_SOCKET) {
        logn("socket failed with error: " + WSAGetLastError());
        WSACleanup();
        return peanats::Error;
      }

      // Connect to server.
      iResult = ::connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
      if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
      }
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
      logn("Unable to connect to socket!");
      WSACleanup();
      return peanats::Error;
    }

    logn("Connected to " + server_ip + ":" + server_port);
    connected = true;
    return 0;
  }

  void tcp_disconnect() {
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
    logn("Disconnected from " + std::string(server_ip) + ":" + std::string(server_port));
    connected = false;
  }


  //! automatically called by peanats to send data
  size_t tcp_receive(char* ptr, const size_t maxlen) override {
    iResult = recv(ConnectSocket, ptr, maxlen, 0);
    return iResult ? iResult : Error;
  }

  //! automatically called by peanats to receive data
  size_t tcp_transmit(const char* data, const size_t size) {
    // Send an initial buffer
    iResult = send(ConnectSocket, data, size, 0);
    if (iResult == SOCKET_ERROR) {
      logn("tcp_error: socket send failed with error: " + WSAGetLastError());
      closesocket(ConnectSocket);
      WSACleanup();
      this->connected = false;
      return peanats::Error;
    }

    return Error;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
private:
  WSADATA wsaData;
  SOCKET ConnectSocket = INVALID_SOCKET;
  struct addrinfo* result = NULL, * ptr = NULL, hints;
  int iResult;
};

PEANATS_NAMESPACE_END