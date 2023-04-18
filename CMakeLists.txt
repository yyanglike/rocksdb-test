cmake_minimum_required(VERSION 3.24)

project(db)

# Set the C++ compiler to gcc
# set(CMAKE_CXX_COMPILER /usr/bin/gcc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-error=unused-but-set-variable -Wno-deprecated-copy -Wno-deprecated")
include(FetchContent)

#FetchContent_Declare(
#  rocksdb
#  GIT_REPOSITORY https://github.com/facebook/rocksdb.git
#  GIT_TAG        v6.22.1
#)
#
#FetchContent_MakeAvailable(rocksdb)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.10.2
)

FetchContent_MakeAvailable(nlohmann_json)

# find_package(rocksdb)

find_package(gflags REQUIRED)
include_directories(${GFLAGS_INCLUDE_DIRS} /Users/yangyi/company/c++/rocksdb/include )

add_executable(db rocksdb_.cpp)
target_link_libraries(db ${GFLAGS_LIBRARIES} /Users/yangyi/company/c++/rocksdb/librocksdb.a)


