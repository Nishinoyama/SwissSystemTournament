cmake_minimum_required(VERSION 3.16)
project(SwissSystemTournament)

set(CMAKE_CXX_STANDARD 14)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(SwissSystemTournament main.cpp Player.cpp Player.h SwissSystemTournament.cpp SwissSystemTournament.h MatchResult.cpp MatchResult.h picojson.h)
