//
//  custom_strstream.hpp
//  SIFT
//
//  Created by VADL on 1/12/22.
//  Copyright © 2022 VADL. All rights reserved.
//
// Based on /usr/include/c++/8/backward/strstream on the Raspberry Pi.
// This file is a partial replacement for the <strstream> header of the C++ standard library since clang++ on the Raspberry Pi's Nix doesn't have the header..

#ifndef custom_strstream_hpp
#define custom_strstream_hpp

#include <ios>
#include <ostream>

// From https://codeviewer.zeldamods.org/uking/include/x86_64-linux-gnu/c++/10/bits/c++config.h.html //
#ifndef _GLIBCXX_CONST
# define _GLIBCXX_CONST __attribute__ ((__const__))
#endif

#ifndef _GLIBCXX_PURE
# define _GLIBCXX_PURE __attribute__ ((__pure__))
#endif
// //

// Class strstreambuf, a streambuf class that manages an array of char.
// Note that this class is not a template.
class strstreambuf : public std::basic_streambuf<char, std::char_traits<char> >
{
public:
  // Types.
  typedef std::char_traits<char>              _Traits;
  typedef std::basic_streambuf<char, _Traits> _Base;
    
    // https://code.woboq.org/gcc/libstdc++-v3/include/std/streambuf.html#std::basic_streambuf::int_type
    typedef typename traits_type::int_type                 int_type;

public:
  // Constructor, destructor
  explicit strstreambuf(std::streamsize __initial_capacity = 0);
  strstreambuf(void* (*__alloc)(size_t), void (*__free)(void*));

  strstreambuf(char* __get, std::streamsize __n, char* __put = 0) throw ();
  strstreambuf(signed char* __get, std::streamsize __n, signed char* __put = 0) throw ();
  strstreambuf(unsigned char* __get, std::streamsize __n, unsigned char* __put=0) throw ();

  strstreambuf(const char* __get, std::streamsize __n) throw ();
  strstreambuf(const signed char* __get, std::streamsize __n) throw ();
  strstreambuf(const unsigned char* __get, std::streamsize __n) throw ();

  virtual ~strstreambuf();

public:
  void freeze(bool = true) throw ();
  char* str() throw ();
  _GLIBCXX_PURE int pcount() const throw ();

    // https://code.woboq.org/gcc/libstdc++-v3/include/std/streambuf.html
    void
    __safe_pbump(std::streamsize __n) { _M_out_cur += __n; }
    
protected:
  virtual int_type overflow(int_type __c  = _Traits::eof());
  virtual int_type pbackfail(int_type __c = _Traits::eof());
  virtual int_type underflow();
  virtual _Base* setbuf(char* __buf, std::streamsize __n);
  virtual pos_type seekoff(off_type __off, std::ios_base::seekdir __dir,
                           std::ios_base::openmode __mode
                           = std::ios_base::in | std::ios_base::out);
  virtual pos_type seekpos(pos_type __pos, std::ios_base::openmode __mode
                           = std::ios_base::in | std::ios_base::out);
    
    // https://code.woboq.org/gcc/libstdc++-v3/include/std/streambuf.html
    char_type*                 _M_out_cur;    ///< Current put area.

private:
  strstreambuf&
  operator=(const strstreambuf&) = delete; // Not yet implemented

  strstreambuf(const strstreambuf&) = delete; // Not yet implemented

  // Dynamic allocation, possibly using _M_alloc_fun and _M_free_fun.
  char* _M_alloc(size_t);
  void  _M_free(char*);

  // Helper function used in constructors.
  void _M_setup(char* __get, char* __put, std::streamsize __n) throw ();

  private:
  // Data members.
  void* (*_M_alloc_fun)(size_t);
  void  (*_M_free_fun)(void*);

  bool _M_dynamic  : 1;
  bool _M_frozen   : 1;
  bool _M_constant : 1;
};


// Class ostrstream
class ostrstream : public std::basic_ostream<char>
{
public:
  ostrstream() = delete; // Not yet implemented
  ostrstream(char*, int, ios_base::openmode = ios_base::out);
  virtual ~ostrstream();

  _GLIBCXX_CONST strstreambuf* rdbuf() const throw ();
  void freeze(bool = true) throw();
  char* str() throw ();
  _GLIBCXX_PURE int pcount() const throw ();

private:
  strstreambuf _M_buf;
};

#endif /* custom_strstream_hpp */
