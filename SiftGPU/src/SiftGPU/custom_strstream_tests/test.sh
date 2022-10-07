set -e

clang++ test.cpp ../custom_strstream.cpp -g3
lldb -o run a.out
