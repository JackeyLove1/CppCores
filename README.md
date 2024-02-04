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

### openmp
`reference: https://edu.itp.phys.ethz.ch/hs12/programming_techniques/openmpi.pdf`
```shell
wget https://download.open-mpi.org/release/open-mpi/v5.0/openmpi-5.0.1.tar.gz
tar -xvzf openmpi-5.0.1.tar.gz
cd openmpi
./configure --prefix=$HOME/opt/openmpi
make all
make install
# echo "export PATH=\$PATH:\$HOME/opt/openmpi/bin" >> $HOME/.bashrc
# echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$HOME/opt/openmpi/lib" >> $HOME/.bashrc
-fopenmp
```
```cmake
cmake_minimum_required(VERSION 3.1)
project(your_project_name)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_executable(your_target your_source_file.c)
```
# Reference
## GPT-4

## toft
https://github.com/chen3feng/toft

## leveldb
https://github.com/google/leveldb
