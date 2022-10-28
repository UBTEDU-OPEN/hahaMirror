cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

project(hahaMirror)

set(
    CXX_FLAGS
    -g
    -DCHECK_PTHREAD_RETURN_VALUE
    -D_FILE_OFFSET_BITS=64
    -Wall
    -Wextra
#    -Werror
#   -Wconversion
    -Wno-unused-parameter
    -Wno-unused-function
#    -Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    -Wshadow
    -Wwrite-strings
    -march=native
    -fPIC
    -rdynamic
)

# 上述的 -std=c++17 没有生效，所以添加这一句
set(CMAKE_CXX_STANDARD 14)

if(CMAKE_BUILD_BITS EQUAL 32)
  list(APPEND CXX_FLAGS "-m32")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(APPEND CXX_FLAGS "-Wno-null-dereference")
  list(APPEND CXX_FLAGS "-Wno-sign-conversion")
  list(APPEND CXX_FLAGS "-Wno-unused-local-typedef")
  list(APPEND CXX_FLAGS "-Wthread-safety")
  list(REMOVE_ITEM CXX_FLAGS "-rdynamic")
endif()
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


find_package(OpenCV 4.3.0 REQUIRED)
message("main opencv dir: " ${OpenCV_INCLUDE_DIRS})
set(BOOST_LIBRARYDIR /usr/lib/x86_64-linux-gnu)
find_package(Boost REQUIRED system filesystem)

include_directories(
    3rdParty/muduo/include
    3rdParty/spdlog/include
    3rdParty/googletest/include   
    3rdParty/jsonrpccpp/include
    3rdParty/websocketpp/include
    3rdParty/
    main_haha/include/common/
    ${CMAKE_SOURCE_DIR}
)

link_directories(3rdParty/muduo/lib)
link_directories(3rdParty/spdlog/lib)
link_directories(3rdParty/jsonrpccpp/lib)
link_directories(3rdParty/googletest/lib)
link_directories(3rdParty/snowplow/lib)
link_directories(3rdParty/zip/lib)
link_directories(3rdParty/libarchive/lib)


add_subdirectory(common)
# add_subdirectory(main_haha)
add_subdirectory(src)
