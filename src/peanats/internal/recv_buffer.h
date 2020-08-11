#pragma once
#include <peanats/internal/globals.h>

#include <vector>
#include <inttypes.h>
#include <assert.h>

PEANATS_NAMESPACE_BEGIN


// [RecvBuffer]
// Helper class for managing a copy free and allocation
// free receive buffer
class RecvBuffer
{
public:
  // ctor, 1k default 
  RecvBuffer(size_t capacity = 1024)
    : _data(new char[capacity]),
      _cursor(_data),
      _capacity(capacity),
      _size(0) {}

  // dtor
  ~RecvBuffer() {
    if (_capacity)
      delete[] _data;
  }

  // gets a pointer to the start of the buffer, not an iterator
  inline char* data() const { return _data; }

  //! returns the current size of the buffer, i.e. size()
  inline const size_t have() { return _size; }

  //! gets how much remaining space until the buffer is full
  inline const size_t want() { return _capacity - _size; }

  //! clears the buffer by setting it's size to 0
  void clear() { _size = 0; _cursor = _data; }

  //! sets the size
  void push_back(const char c) {
    PEANATS_ASSERT(_size < _capacity, "buffer overflow");
    _data[_size++] = c;
  }

  // [ Members ]
  char* _data;
  char* _cursor;
  size_t _capacity;
  size_t _size;
};



PEANATS_NAMESPACE_END