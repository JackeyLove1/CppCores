#pragma once

#include <iostream>
#include <map>
#include <string>
#include <functional>

/**
 * @brief read process info from /proc, only support linux
 */

void ProcessMemUsage(double* vm_usage, double* resident_set);

struct HostStat {
    int64_t total_memory_bytes_;
    int64_t cpu_cores_;
    int64_t cpu_hz_;
};

void GetHostStat(HostStat* stat);

struct ProcessStat {
    int64_t virtual_memory_bytes_;
    int64_t physical_memory_bytes_;
    int64_t total_jiffies_;
    std::map<std::string, int64_t> threadpool_jiffies_;
};

void GetProcessStat(const std::function<std::string(std::string)>* threadpool_classfier,
                    ProcessStat* stat);

int PipeExecute(const char* command, std::ostream* output);