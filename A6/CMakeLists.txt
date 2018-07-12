cmake_minimum_required(VERSION 3.9)
project(CSpielwiese C)

set(CMAKE_C_STANDARD 99)


add_executable(CSpielwiese dedup.c dict.c dict.h)