#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <dirent.h>
#include <unistd.h>

#include "process_util.h"
#include "defer.h"

struct ThreadStat {
    std::string thread_name_;
    int64_t virtual_memory_bytes_{0};
    int64_t physical_memory_bytes_{0};
    int64_t jiffies_{0};
};

static void GetThreadStat(const std::string &stat_file, ThreadStat *stat) {
    std::ifstream stat_stream{stat_file, std::ios::in};

    std::string pid;
    std::string comm;
    std::string state, ppid, pgrp, session, tty_nr;
    std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    int64_t utime = 0, stime = 0;
    std::string cutime, cstime, priority, nice;
    std::string O, itrealvalue, starttime;

    uint64_t vsize;
    int64_t rss;
    try {
        stat_stream >> pid >> comm >> state >> ppid >>
                    pgrp >> session >> tty_nr >> tpgid >> flags >>
                    minflt >> cminflt >> majflt >> cmajflt >> utime >>
                    stime >> cutime >> cstime >> priority >>
                    nice >> O >> itrealvalue >> starttime >> vsize >> rss;
    } catch (...) {
    }

    stat_stream.close();

    if (!comm.empty()) {
        int left = 0;
        if (comm[0] == '(') {
            ++left;
        }
        int right = comm.size() - 1;
        if (comm[right] == ')') {
            --right;
        }
        if (left <= right) {
            stat->thread_name_ = comm.substr(left, right - left + 1);
        } else {
            stat->thread_name_ = comm;
        }
    }

    static int64_t page_size = sysconf(_SC_PAGE_SIZE);
    stat->virtual_memory_bytes_ = vsize;
    stat->physical_memory_bytes_ = rss * page_size;
    stat->jiffies_ = utime + stime;
}

void ProcessMemUsage(double *vm_usage, double *resident_set) {
    ThreadStat stat;
    GetThreadStat("/proc/self/stat", &stat);
    *vm_usage = stat.virtual_memory_bytes_ / 1024.0;
    *resident_set = stat.physical_memory_bytes_ / 1024.0;
}

static int64_t GetCpuCores() {
#if defined(_SC_NPROCESSORS_ONLN)
    return sysconf(_SC_NPROCESSORS_ONLN);
#else
    std::shared_ptr<FILE> fp(fopen("/proc/stat", "r"), [](FILE* f) { fclose(f); });
    if (!fp) {
        std::cerr << "[HardWare Metric] open /proc/stat failed."
                  << " " << safe_strerror(errno) << std::endl;
        return -1;
    }
    static const size_t kLineMaxLen = 256;  // enough in here
    std::unique_ptr<char[]> aline(new char[kLineMaxLen]);
    if (!aline) {
        std::cerr << "[HardWare Metric] malloc failed." << std::endl;
        return -1;
    }
    static const size_t kHeaderMaxLen = 10;
    char header[kHeaderMaxLen];
    uint32_t i = 0;
    size_t len = 0;
    char* line_ptr = aline.get();
    getline(&line_ptr, &len, fp.get());  // drop the first line
    while (getline(&line_ptr, &len, fp.get())) {
        i++;
        sscanf(line_ptr, "%s", header);
        if (!strncmp(header, "intr", kHeaderMaxLen)) {
            break;
        }
    }
    return std::max(i - 1, 1);
#endif
}

static int64_t GetTotalMemory() {
    std::ifstream mem_info("/proc/meminfo", std::ios::in);
    if (!mem_info.is_open()) {
        std::cerr << "open /proc/meminfo failed" << std::endl;
        return -1;
    }

    std::string line;
    std::string prefix = "MemTotal:";
    while (getline(mem_info, line)) {
        if (line.size() > prefix.size() &&
            memcmp(line.c_str(), prefix.c_str(), prefix.size()) == 0) {
            std::istringstream s(line);
            std::string p;
            s >> p;
            int64_t total_kb;
            s >> total_kb;
            return total_kb * 1024;
        }
    }
    return -1;
}

void GetHostStat(HostStat *stat) {
    stat->cpu_hz_ = sysconf(_SC_CLK_TCK);
    stat->cpu_cores_ = GetCpuCores();
    stat->total_memory_bytes_ = GetTotalMemory();
}

static void GetThreadJiffies(const std::function<std::string(std::string)> *classfier,
                             std::map<std::string, int64_t> *output) {
    DIR *dp;
    struct dirent *ent;
    output->clear();

    dp = opendir("/proc/self/task");
    if (dp == nullptr) {
        std::cerr << "failed to open dir /pro/self/task" << "\n";
        return;
    }
    DEFER([dp] { closedir(dp); });
    for (errno = 0; (ent = readdir(dp)) != nullptr; errno = 0) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        if (ent->d_type == DT_DIR) {
            ThreadStat stat;
            std::string stat_path = "/proc/self/task/" + std::string(ent->d_name) + "/stat";
            GetThreadStat(stat_path, &stat);
            std::string thread_group = (*classfier)(stat.thread_name_);
            (*output)[thread_group] += stat.jiffies_;
        } else {
            fprintf(stderr, "skip wield file /proc/self/task/%s of type %d\n", ent->d_name,
                    ent->d_type);
        }
    }
    if (errno != 0) {
        fprintf(stderr, "read from /proc/self/task got error: %d\n", errno);
    }
}

void GetProcessStat(const std::function<std::string(std::string)> *threadpool_classfier,
                    ProcessStat *stat) {
    stat->virtual_memory_bytes_ = 0;
    stat->physical_memory_bytes_ = 0;
    stat->threadpool_jiffies_.clear();

    ThreadStat t_stat;
    GetThreadStat("/proc/self/stat", &t_stat);
    stat->virtual_memory_bytes_ = t_stat.virtual_memory_bytes_;
    stat->physical_memory_bytes_ = t_stat.physical_memory_bytes_;
    stat->total_jiffies_ = t_stat.jiffies_;

    if (threadpool_classfier != nullptr) {
        GetThreadJiffies(threadpool_classfier, &stat->threadpool_jiffies_);
    }
}

int PipeExecute(const char *command, std::ostream *output) {
    std::array<char, 256> buffer;
    int ret = 0;
    {
        std::shared_ptr<FILE> command_pipe(popen(command, "r"),
                                           [&ret](FILE *p) { ret = pclose(p); });
        while (!feof(command_pipe.get())) {
            if (fgets(buffer.data(), 256, command_pipe.get()) != NULL) (*output) << buffer.data();
        }
    }
    return ret;
}