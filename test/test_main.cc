#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <iostream>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::AllowCommandLineReparsing();
    if (!gflags::ParseCommandLineFlags(&argc, &argv, true)) {
        std::cerr << "Failed to parse command line" << "\n";
        return -1;
    }
    auto result = RUN_ALL_TESTS();
    if (result == 0) {
        std::cout << "Succeed to Run all tests..." << "\n";
    }
    return result;
}