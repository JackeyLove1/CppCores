# README

## Install Package

### openssl

```shell
sudo apt-get install openssl -y
```

```cmake
find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(your_exec ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})
```

### boost

```shell
sudo apt-get install libboost-dev -y
# install boost 1.82
https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz

tar -xvzf boost_1_82_0.tar.gz
./bootstrap.sh --prefix=/usr/local
sudo ./b2 install

# used in cmake
find_package(Boost 1.82.0 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS}) 
add_executable(your_executable_file your_source_file.cpp)
target_link_libraries(your_executable_file ${Boost_LIBRARIES})
```

### json

```shell
apt-get install libhiredis-dev

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})
include_directories(${JSONCPP_INCLUDE_DIRS})
add_executable(your_executable_file your_source_file.cpp)
target_link_libraries(your_executable_file ${JSONCPP_LIBRARIES})
```

### zlib
```shell
sudo apt-get install zlib1g-dev
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
add_executable(MyProject main.cpp)
target_link_libraries(MyProject ${ZLIB_LIBRARIES})
```

# Reference
## toft
https://github.com/chen3feng/toft


