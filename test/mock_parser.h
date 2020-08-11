#pragma once
#include <vector>
#include <string>
#include <iostream>

#define PEANATS_DISABLE_LOGGING


#include "peanats/internal/globals.h"
#include "peanats/internal/parser.h"

// Convinience macro that also stops mscv from
// "fixing" indentation
#define ANON_NAMESPACE_BEGIN namespace {
#define ANON_NAMESPACE_END   }


PEANATS_NAMESPACE_BEGIN

struct MockMessage {
    MockMessage(Message& m)
      : sid(m.sid),
        subject(m.subject),
        replyto(m.replyto),
        payload(m.payload) {}

    // Members
    int sid;
    std::string subject;
    std::string replyto;
    std::string payload;
};


// A receive buffer that contains a hidden nullterminator
// for easier debugging with msvc
class MockReceiver :
  public Receiver
{
public:
  MockReceiver(size_t size)
    : Receiver(size + 1) { _capacity--; }

  void push_back(const char c) {
    Receiver::push_back(c);
    _data[_size] = 0;
  }
};

class MockParser :
  public Parser
{
public:
  MockParser(size_t buff_size = 512)
    : logger([](const std::string& s) {std::cerr << s;}),
      Parser(nullptr),
      receiver(buff_size) {}
      

  void  log(const std::string& s) { logger.log(s); }
  void logn(const std::string& s) { logger.log(s + "\n"); }

  /* Info callback */
  void on_parser_info(Peastring& name, Peastring& value) override {
    info_capture.push_back({ std::string(name), std::string(value) });
  }

  void on_parser_ping()            override { counters.ping++; }
  void on_parser_pong()            override { counters.pong++; }
  void on_parser_ok()              override { counters.ok++; }
  void on_parser_err(Peastring& s) override { err_capture.push_back(std::string(s)); }
  void on_parser_msg(Message& m)   override { msg_capture.push_back(MockMessage(m)); }

  // Feeds a string into the buffer until the buffer is full
  void fill_buf(std::string& from) {
    while (receiver.want() && !from.empty()) {
      receiver.push_back(from.front());
      from.erase(from.begin());
    }
  }

  // Parses a provided string, if the string is too large to hold in the
  // buffer it's splitted and parsed in segments to emulate partial reads
  // and overflow
  void mock_parse(const std::string& str) {
    std::string s = str;

    do {
      fill_buf(s);
      parse(receiver);
    } while (!s.empty());
  }


  // Members
  Logger logger;


  std::vector<std::pair<std::string, std::string>>   info_capture;
  std::vector<std::string>   err_capture;
  std::vector<MockMessage> msg_capture;

  struct {
    int ping;
    int pong;
    int info;
    int err;
    int ok;
    int msg;
  } counters = { 0 };

  MockReceiver receiver;
};



PEANATS_NAMESPACE_END