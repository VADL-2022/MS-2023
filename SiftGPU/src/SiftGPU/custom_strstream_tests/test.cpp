#include "../custom_strstream.hpp"

#include <iostream>
#include <iomanip> // std::setprecision()

#include <cmath>

int main() {
  char buf[1024];
  ostrstream s(buf, 1024);
  s << "hi\0";
  std::cout << buf << std::endl;

  ostrstream s2(buf, 1024);
  s2<<std::setprecision(8);
  const long double pi = std::acos(-1.L); // https://en.cppreference.com/w/cpp/io/manip/setprecision
  s2 << "hi" << 112312 << " " << 1.0 << " " << 3.1415 << " " << pi << " " << "testing" << std::endl << std::endl << '\0';
  std::cout << buf << std::endl;

  return 0;
}
