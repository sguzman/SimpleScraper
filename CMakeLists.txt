cmake_minimum_required(VERSION 3.10)
project(simplescraper)

set(CMAKE_CXX_STANDARD 17)
set(OPENSSL_INCLUDE_DIR /usr/local/opt/openssl/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_MACOSX_RPATH 1)

add_subdirectory(cpr)
add_subdirectory(brotli)

include_directories(${CPR_INCLUDE_DIRS})
include_directories(${BROTLI_INCLUDE_DIRS})

add_executable(scrape src/main.cpp)

target_link_libraries(scrape ${CPR_LIBRARIES})
target_link_libraries(scrape ${BROTLI_LIBRARIES})

link_directories(/usr/local/lib)
target_link_libraries(scrape gq)
