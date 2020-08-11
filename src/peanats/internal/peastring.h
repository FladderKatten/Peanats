#pragma once
#include "peanats/internal/globals.h"

#include <string>
#include <cstring>
#include <string_view>


PEANATS_NAMESPACE_BEGIN

// ---======================================================================---
// [ Peastring - Internal class for viewing strings ]
// ---======================================================================---
class Peastring
{
public:
  // --------------------------------------------------------------------------
  // [ Constructors ]
  // --------------------------------------------------------------------------
  explicit Peastring(char* s, size_t len)
    : _ptr(s),
      _len(len) {}

  Peastring(const char* s, size_t len)
    : Peastring(const_cast<char*>(s), len) {}

  Peastring(const char* s)
    : Peastring(const_cast<char*>(s), std::strlen(s)) {}

  Peastring()
    : _ptr(0),
      _len(0) {}

  // get the length of the string
  inline const size_t length() const { return _len; }

  // get whether the string is empty
  inline const bool empty() const { return _len == 0; }

  // get whether this string is equal to another string
  const bool compare(const Peastring& other) const {
    return _len == other._len && std::memcmp(this->_ptr, other._ptr, _len) == 0;
  }

  // --------------------------------------------------------------------------
  // [ Access ]
  // --------------------------------------------------------------------------

  //! get the raw pointer to the string
  inline char* data() { return _ptr; }
  //! get the raw pointer to the string
  inline const char* data() const { return _ptr; }
  //! get the character located at 'pos'
  inline char& at(size_t pos) { return _ptr[pos]; }

  // --------------------------------------------------------------------------
  // [ Operator overloads ]
  // --------------------------------------------------------------------------

  //! don't convert to const char because that messes up the nulltemination convert to const char*
  //operator const char* () const { return _ptr; }
  operator const std::string () const { return std::string(_ptr,_len); }

  //! get the character at a position
  char& operator [] (size_t pos) { return at(pos); }

  const bool operator == (const Peastring& other) const { return compare(other) == true; }
  const bool operator != (const Peastring& other) const { return compare(other) == false; }
  const bool operator == (const char* other) const { return compare(Peastring(other, std::strlen(other))) == true; }
  const bool operator != (const char* other) const { return compare(Peastring(other, std::strlen(other))) == false; }

  //std::string operator + (const char* s) { return std::string(*this) + s; }
  std::string operator + (const Peastring& s) { return std::string(*this) + s.data(); }


  // --------------------------------------------------------------------------
  // [ Members ]
  // --------------------------------------------------------------------------
  char* _ptr;
  size_t _len;
};

PEANATS_NAMESPACE_END

// --------------------------------------------------------------------------
// [ Globals ]
// --------------------------------------------------------------------------

#include <ostream>

inline std::ostream& operator<<(std::ostream& os, const peanats::Peastring& s) {

  for (size_t i = 0; i < s._len; i++)
    os << s._ptr[i];

  return os;
}

inline std::string& operator+(std::string& s, const peanats::Peastring& ps)
{
  for (size_t i = 0; i < ps._len; i++)
    s.push_back(ps._ptr[i]);

  return s;
}