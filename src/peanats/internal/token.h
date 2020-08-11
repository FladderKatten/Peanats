#pragma once
#include <peanats/internal/globals.h>
#include <peanats/internal/peastring.h>
#include <peanats/internal/recv_buffer.h>
#include <assert.h>

PEANATS_NAMESPACE_BEGIN

//
// Token class for marking where nats tokes starts and finishes
//
class Token
  : public Peastring
{
public:
  void start(char* location) { _ptr = location; }
  void  stop(char* location) { _len = location - _ptr; }
};


//! Class for keeping track of nats tokens
class TokenList
{
  friend class RecvBuffer;
public:
  TokenList()
    : _size(0),
      _tokens() {}

  void clear()        { _tokens[_size = 0]._ptr = 0; }
  void start(char* p) { _tokens[_size].start(p);  }
  void  stop(char* p) { _tokens[_size++].stop(p); }

  size_t size() { return _size; }

  //! access the front token
  inline Token& front() { return _tokens[0]; }

  //! access the last token
  inline Token& back() {
    PEANATS_ASSERT(_size, "access to token when list is empty");
    return _tokens[_size-1];
  }

  // gets whether any tokens have been stored or started
  inline bool empty() { return front()._ptr == nullptr; }

  // gets all the tokens as a merged string. This is mostly used to log
  // packets without using allocations to build strings
  std::string to_full_string() {
    auto ptr = front()._ptr;
    size_t len = (back()._ptr + back()._len) - ptr;
    return { ptr, len };
  }


  Token& operator[](size_t n) { return _tokens[n]; }

  // [ Members ]
  size_t _size;          //!< current number of tokens
  Token  _tokens[10];    //!< 10 tokens will be enough for all packets
};


PEANATS_NAMESPACE_END
