#include <boost/filesystem.hpp>

inline std::string GetCurrentPath(){
    return boost::filesystem::current_path().string();
}

inline bool Exists(const boost::filesystem::path& path){
    return boost::filesystem::exists(path);
}

inline bool IsDirectory(const boost::filesystem::path& path){
    return boost::filesystem::is_directory(path);
}

inline bool IsFile(const boost::filesystem::path& path){
    return boost::filesystem::is_regular_file(path);
}

inline bool RemoveFile(const boost::filesystem::path& path){
    return boost::filesystem::remove(path);
}

inline void RenameFile(const boost::filesystem::path& old_path, const boost::filesystem::path& new_path){
    boost::filesystem::rename(old_path, new_path);
}

inline bool CreateDirectory(const boost::filesystem::path& path){
    return boost::filesystem::create_directory(path);
}

inline bool RemoveDirectory(const boost::filesystem::path& path){
    return boost::filesystem::remove_all(path);
}
