cmake_minimum_required(VERSION 3.13)
project(2018114002_CShell)

set(CMAKE_C_STANDARD 99)

add_executable(2018114002_CShell ../2018114002_assgn2/cshell.c 2018114002_assgn2/jobs.h)
target_link_libraries(2018114002_CShell -lreadline)