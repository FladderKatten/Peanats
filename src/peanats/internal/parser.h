#pragma once
#include "peanats/internal/globals.h"
#include "peanats/internal/logger.h"
#include "peanats/internal/message.h"
#include "peanats/internal/token.h"
#include "peanats/internal/recv_buffer.h"

#include <inttypes.h>
#include <vector>
#include <array>

PEANATS_NAMESPACE_BEGIN

// [ Forward decl ]
class Peanats;

/*
*/
class Parser
{
public:
  // --------------------------------------------------------------------------
  // [ Constructor / Destructor ]
  // --------------------------------------------------------------------------
  Parser(Peanats* peanats)
    : peanats(peanats),
//      tokens(),
      remaining_payload(0),
      cs(0),
      top(0)
  {
    clear_state();
  }
      
  ~Parser() {}

  // --------------------------------------------------------------------------
  // [ Virtuals ]
  // --------------------------------------------------------------------------
protected:
  virtual void log( const std::string&) = 0;
  virtual void logn(const std::string&) = 0;

  virtual void on_parser_ok() {};
  virtual void on_parser_ping() {};
  virtual void on_parser_pong() {};
  virtual void on_parser_msg(Message& m) {};
  virtual void on_parser_err(Peastring&) {};
  virtual void on_parser_info(Peastring&, Peastring&) {};

  // --------------------------------------------------------------------------
  // [ Api ]
  // --------------------------------------------------------------------------
public:
  //! Parse a 'RecvBuffer' and clear consumed content when done
  inline void ragel_parser(RecvBuffer&);
  inline const int first_final_state();

  //! Reset the state of the parser
  inline void clear_state();


  //! calls the ragel parser and does adjustments to the buffer
  //! afterwards
  inline void parse(RecvBuffer& buf)
  {
    ragel_parser(buf);

    // if we've finished without any started tokesm we've consumed the
    // whole buffer and can reset it
    if (tokens.empty()) {
      debug_logn("Finished in state without any tokens " + std::to_string(cs) + " and consumed the buffer");
      buf.clear();
      return;
    }

    // Otherwise we may need to shift the buffer, but if we still have space
    // to recevice data, we'll wait until really full
    if (buf.want()) {
      return;
    }

    // if the first token is not already at the start of the buffer
    // we move the data that needs saving to the front of the buffer
    if (tokens.front()._ptr != buf.data()) {
      PEANATS_ASSERT(!tokens.empty(), "missing tokens");

      auto from = tokens.front()._ptr;
      auto dest = buf.data();
      auto distance = from - dest;
      auto size = buf._cursor - from;

      // move the data to the front of the buffer
      std::memmove(dest, from, size);

      // adjust the token pointers. Compare less or equal because
      // a token might be started but not finished
      for (size_t i = 0; i <= tokens.size(); i++)
        tokens[i]._ptr -= distance;

      // reset the cursor
      buf._cursor -= distance;
      buf._size = size;

      return;
    }

    // Otherwise we have a packet that is too large
    logn("parser_error: buffer is too small to handle the packet");
    clear_state();
    buf.clear();
    return;
  }

  // --------------------------------------------------------------------------
  // [ Members ]
  // --------------------------------------------------------------------------

  // Ragel stuff
  size_t p_index;
  int cs;
  int stack[4], top = 0;
  //ServerINFO server_info;
  TokenList tokens;

  size_t remaining_payload = 0;
  Peanats* peanats;
};

PEANATS_NAMESPACE_END

#include <peanats/internal/autogen/ragel_parser.h>


