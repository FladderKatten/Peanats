#pragma once

#include "peanats/globals.h"
#include "peanats/debugutil.h"

#include <vector>
#include <inttypes.h>

PEANATS_NAMESPACE_BEGIN


class NatsBuffer :
  protected std::vector<uint8_t>
{
public:
  using vector::vector;
  using vector::data;
  using vector::size;
  using vector::capacity;

  const auto index() { return _index; }
  void   set_index(const size_t index) { _index = index; }

  //! Returns the number of bytes remaining 
  auto remaining() { return capacity() - index(); }


  // [ Members ]
  size_t _index;
};


PEANATS_NAMESPACE_END