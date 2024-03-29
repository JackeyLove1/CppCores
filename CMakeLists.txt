cmake_minimum_required(VERSION 3.10)
project("CppCores")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall")

message("ProjectDir:${PROJECT_SOURCE_DIR}")
message("ENV: '$ENV{CXXFLAGS}'")
message("Version: ${CMAKE_CXX_STANDARD} ")

# level db
find_path(LEVELDB_INCLUDE_PATH NAMES leveldb/db.h)
find_library(LEVELDB_LIB NAMES leveldb)
if ((NOT LEVELDB_INCLUDE_PATH) OR (NOT LEVELDB_LIB))
    message(FATAL_ERROR "Fail to find leveldb")
endif ()
include_directories(${LEVELDB_INCLUDE_PATH})
#include_directories(
#        ${PROJECT_SOURCE_DIR}/src/
#        ${PROJECT_SOURCE_DIR}/third/leveldb/include
#)
#option(LEVELDB_BUILD_TESTS "Enable leveldb testing and parse tools" OFF)
#add_subdirectory("third/leveldb")

find_package(Boost 1.82.0 REQUIRED
        COMPONENTS system filesystem thread)


# glog
find_package(glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIR})

# gtest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# brpc static link
# gflags
find_package(gflags REQUIRED)
#include(FindThreads)
#include(FindProtobuf)
find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})
#set(BRPC_PATH "${PROJECT_SOURCE_DIR}/third/brpc/build/output")
## message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
## find_path(BRPC_PATH NAMES bvar/bvar.h PATHS ${PROJECT_SOURCE_DIR}/third/brpc/src)
#if (NOT BRPC_PATH)
#    message(FATAL_ERROR "BRPC path is not found")
#endif ()
#message("BRPC PATH: ${BRPC_PATH}")
#include_directories("${BRPC_PATH}/include")
#add_library(brpc STATIC IMPORTED)
#set_target_properties(brpc PROPERTIES
#        IMPORTED_LOCATION ${BRPC_PATH}/lib/libbrpc.a
#        INTERFACE_INCLUDE_DIRECTORIES ${BRPC_PATH}/include)

# boost
include_directories(${Boost_INCLUDE_DIRS})

# absl
#add_subdirectory("third/abseil-cpp")
#include_directories("third/abseil-cpp")
#include_directories("third/abseil-cpp/algorithm")
#include_directories("third/abseil-cpp/base")
#include_directories("third/abseil-cpp/container")
#include_directories("third/abseil-cpp/hash")
#include_directories("third/abseil-cpp/strings")
#include_directories("third/abseil-cpp/types")
#include_directories("third/abseil-cpp/memory")
#set(absl_libs absl::strings
#        absl::flat_hash_map
#        absl::base
#        absl::memory
#        absl::flags
#        absl::synchronization
#        absl::numeric
#        absl::algorithm
#        absl::utility
#        absl::numeric
#        absl::time)

# JSON
#find_library(JSONCPP_LIB jsoncpp)
#include_directories(${JSONCPP_INCLUDE_DIR})
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

#zlib
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

find_library(YAMLCPP yaml-cpp)

set(libs
        ${LEVELDB_LIB}
        ${Boost_LIBRARIES}
        ${GTEST_LIBRARIES}
        ${PROTOBUF_LIBRARIES}
        ${OPENSSL_CRYPTO_LIBRARY}
        ${OPENSSL_SSL_LIBRARY}
        ${JSONCPP_LIBRARIES}
        ${ZLIB_LIBRARIES}
        yaml-cpp
        gflags
        dl
        glog::glog
        ev)
find_package(Protobuf REQUIRED)
if(PROTOBUF_FOUND)
    message (STATUS "protobuf found: ${PROTOBUF_VERSION}")
else()
    message (FATAL_ERROR "Cannot find Protobuf")
endif()
include_directories(${Protobuf_INCLUDE_DIRS})
# Now we can use protobuf_generate
# PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS HelloWorld.proto)
message(STATUS "Protobuf_INCLUDE_DIRS: ${Protobuf_INCLUDE_DIRS}")

# openmp support
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/ds")
include_directories("${PROJECT_SOURCE_DIR}/src/concurrent")
include_directories("${PROJECT_SOURCE_DIR}/src/coroutine")
include_directories("${PROJECT_SOURCE_DIR}/src/meta")
include_directories("${PROJECT_SOURCE_DIR}/src/fiber")
# include_directories("${PROJECT_SOURCE_DIR}/src/utils")

file(GLOB SRCS "src/*.cc" "src/*.h" "src/ds/*" "src/utils/*" "src/concurrent/*" "src/coroutine/*" "src/meta/*" "src/fiber/*")

file(GLOB WORK_TESTS "src/test/*.cc" "src/test/*.h")
add_executable(test_main ${WORK_TESTS} ${SRCS})
target_link_libraries(test_main ${libs})

add_executable(main main.cc)
target_link_libraries(main ${libs})
