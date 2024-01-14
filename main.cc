#include <iostream>
#include <gflags/gflags.h>
#include <glog/logging.h>

int main(int argc, char **argv) {
    gflags::AllowCommandLineReparsing();
    if (!gflags::ParseCommandLineFlags(&argc, &argv, true)) {
        std::cerr << "Failed to parse command line" << "\n";
        return -1;
    }
    std::cout << "Hello, World!" << "\n";
    return 0;
}