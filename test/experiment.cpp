//
// Created by llarrauriborroto on 25/03/2021.
//

#include <fstream>
#include <future>
#include "experiment.h"
#include "../machines/builders.h"
#include "../machines/serializer.h"
#include "../minimizers/OFAMinimizers/w_state_priority.h"
#include <unistd.h>
#include <ctime>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/mman.h>

enum {
    TIMEOUT = 600,
    NS_PER_SECOND = 1000000000,
};

namespace {
    struct Logger {
        std::ostream &ostream;

        Logger(std::ostream &_ostream) :
                ostream(_ostream) {}


        template<typename T>
        void log(const std::string &property, const T &value) {
            ostream << "\"" << property << "\":" << value << ",\n";
        };
    };
}

template<class T>
BenchmarkStats<T> benchmark(std::function<T(void)> func, rlim_t cpu_time) {
    BenchmarkStats<T> stats;
    T *m_result = static_cast<T *>(mmap(nullptr, sizeof(T), PROT_READ | PROT_WRITE | PROT_EXEC,
                                        MAP_ANONYMOUS | MAP_SHARED, -1, 0));
    pid_t id = fork();
    if (id == 0) {
        rlimit limit = {cpu_time, RLIM_INFINITY};
        setrlimit(RLIMIT_CPU, &limit);
        *m_result = func();
        exit(0);
    }
    int status = 0;
    rusage resources;
    wait4(id, &status, 0, &resources);
    if (WIFSIGNALED(status)) {
        stats.err = WTERMSIG(status);
        return stats;
    }
    stats.result = std::make_unique<T>(*m_result);

    stats.duration = double(resources.ru_utime.tv_sec + resources.ru_stime.tv_sec)
                     + double(resources.ru_utime.tv_usec + resources.ru_stime.tv_usec) / 1000000;
    munmap(m_result, sizeof(T));
    return stats;
}

template<>
BenchmarkStats<void> benchmark(std::function<void(void)> func, rlim_t cpu_time) {
    BenchmarkStats<void> stats;
    pid_t id = fork();
    if (id == 0) {
        rlimit limit = {cpu_time, RLIM_INFINITY};
        setrlimit(RLIMIT_CPU, &limit);
        func();
        exit(0);
    }
    int status = 0;
    rusage resources;
    wait4(id, &status, 0, &resources);
    if (WIFSIGNALED(status)) {
        stats.err = WTERMSIG(status);
        return stats;
    }

    stats.duration = double(resources.ru_utime.tv_sec + resources.ru_stime.tv_sec)
                     + double(resources.ru_utime.tv_usec + resources.ru_stime.tv_usec) / 1000000;


    return stats;
}


void SBCMin::minimizationExperiment(const SBCMin::DFSM &driver, const SBCMin::DFSM &driven,
                                    std::filesystem::path dir) {
    namespace fs = std::filesystem;

    auto parent_path = dir.parent_path();
    if (!fs::exists(parent_path)) {
        std::filesystem::create_directory(parent_path);
    }
//
//    name = name + ".txt";
//    std::filesystem::path file_dir = dir / name;
    std::ofstream ostream(dir, std::ios_base::trunc);

    Logger logger(ostream);

    logger.log("driver_size", driver.size());
    logger.log("driver_inputs", driver.numberOfInputs());
    logger.log("driver_outputs", driver.numberOfOutputs());

    logger.log("driven_size", driven.size());
    logger.log("driven_inputs", driven.numberOfInputs());
    logger.log("driven_outputs", driven.numberOfOutputs());

    ///Time OFA creation
    {
        auto createOFA = [&]() {
            volatile OFA ofa = buildOFA(driver, driven);
        };

        double duration = 0.0;

        for (int i = 0; i < 3; ++i) {
            auto stats = benchmark<void>(createOFA);
            if (stats.err != 0) {
                logger.log("ofa_err", stats.err);
            }
            duration += stats.duration;
        }

        duration = duration / 3;
        logger.log("ofa_building_time", duration);
    }

    OFA ofa = buildOFA(driver, driven);
    logger.log("ofa_size", ofa.size());
    logger.log("ofa_transitions", ofa.numberOfTransitions());

    ///Time OFA minimization

    {
        auto minimizeOFA = [&]() -> double {
            OFAMinimizers::WStatePriority minimizer;
            volatile bool success = minimizer.run(ofa, driven.size());
            volatile DFSM result = minimizer.getResult();
            return minimizer.SATTime();
        };


        double duration = 0.0;
        double sat_duration = 0.0;
        bool timeout = false;

        for (int i = 0; i < 3; ++i) {
            auto stats = benchmark<double>(minimizeOFA, 300);
            if (stats.err != 0) {
                timeout = true;
                break;
            } else {
                duration += stats.duration;
                sat_duration += *(stats.result);
            }
        }

        if (timeout) {
            logger.log("ofa_minimization_time", "timeout");
        } else {
            duration = duration / 3;
            sat_duration = sat_duration / 3;
            logger.log("ofa_minimization_time", duration);
            logger.log("sat_time", sat_duration);
        }
    }
}

void SBCMin::experimentCampaign(std::filesystem::path dir, std::filesystem::path result_dir) {
    namespace fs = std::filesystem;
    if (!fs::exists(dir)) {
        std::cout << "Experiment directory does not exist";
        return;
    }

    for (auto &entry: fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        auto file_dir = entry.path();
        auto filename = file_dir.stem().string();
        auto[A, B]=deserializeCascade(file_dir);
        auto result_name = filename + "result" + ".txt";
        auto result_path = result_dir / result_name;
        minimizationExperiment(A, B, result_path);
    }

}
