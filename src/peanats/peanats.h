// [Guard ]
#pragma once

// [ Dependencies ]
#include "peanats/internal/globals.h"
#include "peanats/internal/parser.h"
#include "peanats/internal/logger.h"
#include "peanats/internal/message.h"
#include "peanats/internal/support.h"

#include <string.h>
#include <inttypes.h>
#include <functional>

PEANATS_NAMESPACE_BEGIN

// [ Forward decl ]
class Peanats;

using Sid = uint32_t;

using VoidCallback      = std::function<void()>;
using MessageCallback   = std::function<void(Message&)>;
using OnConnectCallback = std::function<void(Peanats*)>;

static constexpr size_t Error = ~0;

// ---====================================================================---
// [ Subscription]
// ---====================================================================---
//
// Class for keeping subscription data. We avoid keeping the actual name of the
// subject as it's not needed for unsubscribing nor of any use I can think of in
// callbacks.
struct Subscription
{
  Subscription(const MessageCallback cb=0, const uint32_t max_msg=0, const uint32_t timeout=0)
    : max_msg(max_msg),
      timeout(timeout),
      cb(cb) {}

  void       clear()              { cb  = 0; }
  const bool empty() const { return cb == 0; }

  // [ Members ]
  uint32_t max_msg;
  uint32_t timeout;
  MessageCallback cb;
};


// Baseclass for the peanats client
class Peanats :
  protected Parser
{
  // --------------------------------------------------------------------------
  // [Constructors / Destructors]
  // --------------------------------------------------------------------------
public:
  Peanats(size_t buffer_size = 1024)
    : Parser(this),
      server_ip(),
      server_port(),
      receiver(buffer_size),
      inbox_counter(0),
      last_removed(0),
      _logger(0),
      connected(0),
      subs(8),
      shutting_down(false) {}

  // --------------------------------------------------------------------------
  // [ Virtuals ]
  // --------------------------------------------------------------------------
protected:
  //! called by peanats when requesting data
  virtual size_t tcp_receive(char* ptr, size_t maxlen) = 0;

  //! called by peanats when sending data
  virtual size_t tcp_transmit(const char* ptr, const size_t len) = 0;

  //! called by peanats when requesting a connection
  virtual size_t tcp_connect() = 0;

  //! called by peanats when requesting a disconnection
  virtual void   tcp_disconnect() = 0;

  //! Internal helper function for sending data
  bool transmit(const std::string& s) {
    tcp_transmit(s.data(), s.length());
    logn("[>>]: " + support::human_readable(std::string(s)));
    return 0;
  }

  // --------------------------------------------------------------------------
  // [ Parser Virtuals ]
  // --------------------------------------------------------------------------
  void on_parser_ok() {};

  void on_parser_err(Peastring str) {};

  // Just answer?
  void on_parser_ping() { transmit("PONG\r\n");  }

  void on_parser_pong() {}
  
  // Handler for splitted info message
  // Disabled for now, TODO fix
  void on_parser_info(Peastring& a, Peastring& b) override {
    //if (a == "max_payload")
    //  max_payload = std::atoi(b.data());
  }

  //! Internal parser callback that dispatches 'MSG' to 'Subscribers'
  void on_parser_msg(Message& m) {
    if (m.sid < subs.capacity())
      if (subs[m.sid].cb != nullptr)
        subs[m.sid].cb(m);
  }

public:
  // Attach a 'Logger' to the client
  void attach(Logger* logger) { _logger = logger; }

  // ---====================================================================---
  // [ Api ]
  // ---====================================================================---

  // publish a 'Payload' to a 'Subject'
  void publish(const std::string& topic, const std::string& payload = "") {
    std::string s = "PUB " + topic + " " + std::to_string(payload.length()) + "\r\n";
    s += payload + "\r\n";
    transmit(s);
  }

  //! Subscribe to a subject 
  Sid subscribe(const std::string& subject, MessageCallback cb) {
    auto sid = register_subscription(cb);
    transmit("SUB " + subject + " " + std::to_string(sid) + "\r\n");
    return sid;
  }

  // Unsubscribe from a 'Sid'  TODO max_msg
  void unsubscribe(uint32_t sid) {
    unregister_subscription(sid);
    transmit("UNSUB " + sid);
  }

  //! internal: generate a psuedo unique inbox name
  const std::string generate_inbox_name() {
    return "INBOX." + inbox_counter++;
  }

  //! performs one receive/parse cycle
  void process()
  {
    auto len = tcp_receive(receiver._cursor, receiver.want());
    if (len == Error) {
      receiver.clear();
      return;
    }

    receiver._size += len;
    parse(receiver);
  }

  //! disconnect and shut down the client
  void shutdown() { shutting_down = true; }

  // Runs forever
  int run(std::string&& ip, std::string&& port) {
    server_ip = ip;
    server_port = port;
    return run();
  }

  // Runs forever
  int run() {
    shutting_down = false;

    while (!shutting_down) {

      while (!connected)
        connect();

      if (!shutting_down)
        process();
    }

    if (connected)
      tcp_disconnect();

    return 0;
  }

  //! request a tcp connection to the internal server ip/port
  //! also calls the on_connect_callback
  //! TODO: send connect packet
  void connect() {
    if (tcp_connect() != Error) {
      // Grab the info packet
      process();
      on_connect_cb(this);
    }
  }

  //! log a string to the logger
  void log(const std::string& s) override  { _logger->log( s); }
  
  //! log a string to the logger plus a linefeed character
  void logn(const std::string& s) override { _logger->logn(s); }


  // Internal
protected:

  //! Create a new sid but we try to reuse old removed sid's
  //! if possible.
  //! TODO: Speedup
  //!   Maybe keeping track of where we can start walking from
  Sid allocate_subscriber()
  {
    // Try to use the last removed slot if possible
    if (subs[last_removed].empty())
      return last_removed;

    // Otherwise we wheelchair through the table
    for (size_t sid = 0; sid < subs.size(); sid++)
      if (subs[sid].empty())
        return sid;

    // if that also fails we allocate a slot
    subs.push_back({});
    return subs.size();
  }

  //! Remove a subscription from the list
  Sid register_subscription(MessageCallback cb) {
    auto sid = allocate_subscriber();
    subs[sid].cb = cb;
    return sid;
  }


  //! Remove a subscription from the list
  void unregister_subscription(Sid sid) {
    if (sid < subs.capacity())
      subs[sid].clear();
  }

public:
    // [ Members ]
  bool connected;
  bool shutting_down;                 //!< true if peanats is shutting down
  int  inbox_counter;                 //!< number for generation of unique inbox
  int  last_removed;
  Logger* _logger;
  std::vector<Subscription> subs;

  std::string server_ip;
  std::string server_port;
  Receiver    receiver;               //!< Receiver for incoming tcp data

  OnConnectCallback on_connect_cb = [](Peanats*){};
};

PEANATS_NAMESPACE_END