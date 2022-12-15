//
// Created by llarrauriborroto on 10/02/2022.
//

#include <iostream>
#include <sys/resource.h>
#include "machines/machines.h"
#include "machines/builders.h"
#include "minimizers/hopcroft.h"
#include "minimizers/ofa_minimizer.h"
#include "minimizers/OFAMinimizers/assumption_based.h"
#include "minimizers/OFAMinimizers/w_set_priority.h"
#include "minimizers/OFAMinimizers/w_state_priority.h"
#include "machines/serializer.h"
#include "minimizers/monotone_range_searchers.h"
#include "test/experiment.h"
#include <algorithm>
#include <fstream>

//using namespace SBCMin;

int main(int argc, char **argv) {

    std::string cascade_path;
    std::string log_path;

    std::setprecision(6);
    struct rlimit memlimit;
    memlimit.rlim_cur = 4 * long(1073741824);
    memlimit.rlim_max = 4 * long(1073741824);
    setrlimit(RLIMIT_DATA, &memlimit);

    auto clock = std::chrono::high_resolution_clock();

    auto start_time = clock.now();

    {
        int opt;
        while ((opt = getopt(argc, argv, ":c:l:")) != -1) {
            switch (opt) {
                case 'c':
                    cascade_path = optarg;
                    break;
                case 'l':
                    log_path = optarg;
                    break;
                case '?':
                    std::cout << "Unknown option \n";
                    return 0;

                case ':':
                    std::cout << "Option needs an argument \n";
                    return 0;

            }

        }
    }

    if (cascade_path.empty()) {
        std::cout << "Cascade path required";
        return 0;
    }

    if (log_path.empty()) {
        std::cout << " Log path required \n";
        return 0;
    }

    std::cout << "Starting experiment..\n";
    auto[H, T]=SBCMin::deserializeCascade(cascade_path);

    auto A=SBCMin::getOutputNFA(H,4);
    std::cout<<"SIZE"<<A.size()<<" \n";
    A= SBCMin::makeDeterministic(A);
    A=SBCMin::minimizeDFA(A);


    auto ofa = SBCMin::buildOFA(A, T);

    SBCMin::OFAMinimizers::AssumptionBased minimizer;

    minimizer.run(ofa, T.size());
    auto result_size = minimizer.getResult().size();


    auto finish_time = clock.now();
    long double duration = std::chrono::duration_cast<std::chrono::microseconds>(finish_time - start_time).count();
    duration = duration / 1000;
    {
        std::ofstream ostream(log_path, std::ios_base::app);
        ostream << "\"result_size\":" << result_size << ", \n";
        ostream << "\"execution_time\":" << duration << ",\n";
    }

    return 0;
}