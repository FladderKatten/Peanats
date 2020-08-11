#pragma once
//
// Peanats socketcode for lunux, based on:
// https://www.binarytides.com/server-client-example-c-sockets-linux/
//
// wip

#include <peanats/internal/globals.h>
#include <peanats/peanats.h>

#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>

PEANATS_NAMESPACE_BEGIN

class Client :
  public Peanats
{
  using Peanats::Peanats;

  size_t tcp_connect() override
  {
    //Create socket
    sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
      logn("tcp_connect failed: Could not create socket");
      return 0;
    }

    server.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(std::atoi(server_port.c_str()));

    //Connect to remote server
    if (::connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
      logn("connect failed. Error");
      return 1;
    }

    logn("Connected to " + server_ip + ":" + server_port);
    connected = true;
  }

  //! called by peanats when requesting a transmit
  size_t tcp_transmit(const char* data, size_t len) {
    //Send some data
    if (::send(sock, data, len, 0) < 0) {
      logn("tcp_transmit failed!");
      return Error;
    }
    return 0;
  }

  //! called by peanats when requesting a receive
  size_t tcp_receive(char* where, size_t max_len) override {
    //Receive a reply from the server
    auto n = ::recv(sock, where, max_len, 0);
    if (n < 0) {
      logn("tcp_receive failed!");
      return Error;
    }
    return n;
  }

  //! called by peanats when requesting a disconnect
  void tcp_disconnect() override {
    close(sock);
    logn("Disconnected from " + std::string(server_ip) + ":" + std::string(server_port));
  }

  // Members
  int sock = {0};
  struct sockaddr_in server = {0};
};

PEANATS_NAMESPACE_END