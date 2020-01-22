cmake_minimum_required(VERSION 3.13)
project(playlist)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2 -std=c++17")

set(SOURCE_FILES
        playlist_example.cc
        playlist.h
        playlist.cc
        player_mode.h
        player_mode.cc
        playable.h
        playable.cc
        opener.h
        opener.cc
        lib_playlist.h
        player_exception.h
        playable_exception.h)

add_executable(playlist ${SOURCE_FILES})