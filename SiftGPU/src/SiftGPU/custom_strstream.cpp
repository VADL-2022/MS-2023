//
//  custom_strstream.cpp
//  SIFT
//
//  Created by VADL on 1/12/22.
//  Copyright © 2022 VADL. All rights reserved.
//

#include "custom_strstream.hpp"

#include <cstring>
#include <climits>

// https://code.woboq.org/gcc/libstdc++-v3/include/std/streambuf.html#49
#define _IsUnused __attribute__ ((__unused__))

// https://en.cppreference.com/w/cpp/io/strstreambuf/strstreambuf
strstreambuf::strstreambuf(char* get, std::streamsize n, char* put) throw() :
basic_streambuf<char, std::char_traits<char> >(),
_M_out_cur(0)
{
    if (get)
    {
      size_t N = n > 0 ? size_t(n) : n == 0 ? strlen(get) : size_t(INT_MAX);
      if (put)
        {
          setg(get, get, put);
          setp(put, put + N);
        }
      else
        setg(get, get, get + N);
    }
}

strstreambuf::~strstreambuf() {}

// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/src/c%2B%2B98/strstream.cc
std::basic_streambuf<char, std::char_traits<char> >*
strstreambuf::setbuf(char*, std::streamsize)
{ return this; }

// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/src/c%2B%2B98/strstream.cc
strstreambuf::pos_type
strstreambuf::seekoff(off_type off, std::ios_base::seekdir dir,
          std::ios_base::openmode mode)
{
  using namespace std;
  
  bool do_get = false;
  bool do_put = false;

  if ((mode & (ios_base::in | ios_base::out))
  == (ios_base::in | ios_base::out) &&
  (dir == ios_base::beg || dir == ios_base::end))
    do_get = do_put = true;
  else if (mode & ios_base::in)
    do_get = true;
  else if (mode & ios_base::out)
    do_put = true;

  // !gptr() is here because, according to D.7.1 paragraph 4, the seekable
  // area is undefined if there is no get area.
  if ((!do_get && !do_put) || (do_put && !pptr()) || !gptr())
    return pos_type(off_type(-1));

  char* seeklow  = eback();
  char* seekhigh = epptr() ? epptr() : egptr();

  off_type newoff;
  switch (dir)
    {
    case ios_base::beg:
  newoff = 0;
  break;
    case ios_base::end:
  newoff = seekhigh - seeklow;
  break;
    case ios_base::cur:
  newoff = do_put ? pptr() - seeklow : gptr() - seeklow;
  break;
    default:
  return pos_type(off_type(-1));
    }

  off += newoff;
  if (off < 0 || off > seekhigh - seeklow)
    return pos_type(off_type(-1));

  if (do_put)
    {
  if (seeklow + off < pbase())
    {
      setp(seeklow, epptr());
      __safe_pbump(off);
    }
  else
    {
      setp(pbase(), epptr());
      __safe_pbump(off - (pbase() - seeklow));
    }
    }
  if (do_get)
    {
  if (off <= egptr() - seeklow)
    setg(seeklow, seeklow + off, egptr());
  else if (off <= pptr() - seeklow)
    setg(seeklow, seeklow + off, pptr());
  else
    setg(seeklow, seeklow + off, epptr());
    }
  return pos_type(newoff);
}

// https://code.woboq.org/gcc/libstdc++-v3/include/std/streambuf.html
/**
 *  @brief  Alters the stream positions.
 *
 *  Each derived class provides its own appropriate behavior.
 *  @note  Base class version does nothing, returns a @c pos_type
 *         that represents an invalid stream position.
*/
strstreambuf::pos_type
strstreambuf::seekpos(pos_type,
        std::ios_base::openmode /*__mode*/)
{ return pos_type(off_type(-1)); }

// https://code.woboq.org/gcc/libstdc++-v3/include/std/streambuf.html
/**
 *  @brief  Consumes data from the buffer; writes to the
 *          controlled sequence.
 *  @param  __c  An additional character to consume.
 *  @return  eof() to indicate failure, something else (usually
 *           @a __c, or not_eof())
 *
 *  Informally, this function is called when the output buffer
 *  is full (or does not exist, as buffering need not actually
 *  be done).  If a buffer exists, it is @a consumed, with
 *  <em>some effect</em> on the controlled sequence.
 *  (Typically, the buffer is written out to the sequence
 *  verbatim.)  In either case, the character @a c is also
 *  written out, if @a __c is not @c eof().
 *
 *  For a formal definition of this function, see a good text
 *  such as Langer & Kreft, or [27.5.2.4.5]/3-7.
 *
 *  A functioning output streambuf can be created by overriding only
 *  this function (no buffer area will be used).
 *
 *  @note  Base class version does nothing, returns eof().
*/
strstreambuf::int_type
strstreambuf::overflow(int_type __c _IsUnused  /*= traits_type::eof()*/)
{ return traits_type::eof(); }

// https://code.woboq.org/gcc/libstdc++-v3/include/std/streambuf.html
// [27.5.2.4.4] putback
/**
 *  @brief  Tries to back up the input sequence.
 *  @param  __c  The character to be inserted back into the sequence.
 *  @return  eof() on failure, <em>some other value</em> on success
 *  @post  The constraints of @c gptr(), @c eback(), and @c pptr()
 *         are the same as for @c underflow().
 *
 *  @note  Base class version does nothing, returns eof().
*/
strstreambuf::int_type
strstreambuf::pbackfail(int_type __c _IsUnused  /*= traits_type::eof()*/)
{ return traits_type::eof(); }

// https://code.woboq.org/gcc/libstdc++-v3/include/std/streambuf.html
/**
 *  @brief  Fetches more data from the controlled sequence.
 *  @return  The first character from the <em>pending sequence</em>.
 *
 *  Informally, this function is called when the input buffer is
 *  exhausted (or does not exist, as buffering need not actually be
 *  done).  If a buffer exists, it is @a refilled.  In either case, the
 *  next available character is returned, or @c traits::eof() to
 *  indicate a null pending sequence.
 *
 *  For a formal definition of the pending sequence, see a good text
 *  such as Langer & Kreft, or [27.5.2.4.3]/7-14.
 *
 *  A functioning input streambuf can be created by overriding only
 *  this function (no buffer area will be used).  For an example, see
 *  https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html
 *
 *  @note  Base class version does nothing, returns eof().
*/
strstreambuf::int_type
strstreambuf::underflow()
{ return traits_type::eof(); }



//ostrstream::ostrstream() {
//    // Dynamically allocates memory
//    // Not yet implemented
//    throw std::logic_error{"Dynamically allocated ostrstream not yet implemented."};
//}

// https://en.cppreference.com/w/cpp/io/ostrstream/ostrstream

// https://code.woboq.org/gcc/libstdc++-v3/src/c++98/strstream.cc.html
ostrstream::ostrstream(char* s, int n, ios_base::openmode mode)
: std::basic_ios<char>(), std::basic_ostream<char>(0),
  _M_buf(s, n, mode & ios_base::app ? s + strlen(s) : s)
{ std::basic_ios<char>::init(&_M_buf); }

ostrstream::~ostrstream() {
    // Nothing to do since this memory is managed by the user of this class when constructed with a char* argument ( https://en.cppreference.com/w/cpp/io/ostrstream/~ostrstream ) and we didn't implement the default ctor
}
