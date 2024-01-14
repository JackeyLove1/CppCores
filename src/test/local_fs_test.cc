#include <glog/logging.h>
#include <gtest/gtest.h>
#include "local_fs.h"

class LocalFileSystemTest : public ::testing::Test {
public:
    void SetUp() {}

    void TearDown() {}

public:
    const std::string dir_{"local_fs_test"};
};

TEST_F(LocalFileSystemTest, CreateDir) {
    auto current_path = GetCurrentPath();
    LOG(INFO) << "Current Path: " << current_path;
    boost::filesystem::path path(current_path);
    auto target_path = path / dir_;
    bool result = CreateDirectory(target_path);
    ASSERT_TRUE(result);
    LOG(INFO) << "Succeed to create dir: " << target_path;
    bool is_existed = Exists(target_path);
    ASSERT_TRUE(is_existed);
    result = RemoveDirectory(target_path);
    ASSERT_TRUE(result);
    LOG(INFO) << "Succeed to remove dir: " << target_path;
}