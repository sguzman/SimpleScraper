cmake_minimum_required(VERSION 3.10)
project(simplescraper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_MACOSX_RPATH 1)

add_executable(scrape src/main.cpp)

add_subdirectory(cpr)
include_directories(${CPR_INCLUDE_DIRS})
target_link_libraries(scrape ${CPR_LIBRARIES})

link_directories(/usr/local/lib)

target_link_libraries(scrape cpp_redis)
target_link_libraries(scrape gq)
target_link_libraries(scrape brotlienc)
target_link_libraries(scrape brotlidec)
