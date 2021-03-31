//
// Created by llarrauriborroto on 25/03/2021.
//

#ifndef PMIN_EXPERIMENTS_H
#define PMIN_EXPERIMENTS_H

#include <filesystem>
#include "../machines/machines.h"
#include "../minimizers/OFAMinimizers/assumption_based.h"
#include <sys/resource.h>


namespace SBCMin {
    void minimizationExperiment(const DFSM &driver, const DFSM &driven,
                                std::filesystem::path dir = "./benchmarks/result.txt"
//                                std::string name = "result0",
//                                std::filesystem::path dir = "./benchmarks/"
    );

    void experimentCampaign(std::filesystem::path dir = "./benchmarks/",
                            std::filesystem::path result_dir = "./benchmarks/results");

};


template<class T>
struct BenchmarkStats {
    std::unique_ptr<T> result = nullptr;
    int err = 0;
    double duration = 0;
};

template<>
struct BenchmarkStats<void> {
    int err = 0;
    double duration = 0;
};

template<class T>
BenchmarkStats<T> benchmark(std::function<T(void)> func, rlim_t cpu_time = RLIM_INFINITY);

template<>
BenchmarkStats<void> benchmark(std::function<void(void)> func, rlim_t cpu_time);

#endif //PMIN_EXPERIMENTS_H
