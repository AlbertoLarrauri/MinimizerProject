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

using namespace SBCMin;

int main() {
  experimentCampaign();

//    int benchmark_count = 0;
//
//    for (int i = 2; i <= 4; ++i) {
//        for (int j = 0; j < 20; ++j) {
//            auto A = makeRandomDFSM(4, 4, 6 * i);
//            auto B = makeRandomDFSM(4, 4, 6 * i);
//            std::string name = "cascade" + std::to_string(benchmark_count);
//            serializeCascade(A, B, name, "./benchmarks");
//            ++benchmark_count;
//        }
//    }
//
//    for (int i = 6; i <= 8; i += 2) {
//        for (int j = 0; j < 10; ++j) {
//            auto A = makeRandomDFSM(4, 4, 6 * i);
//            auto B = makeRandomDFSM(4, 4, 6 * i);
//            std::string name = "cascade" + std::to_string(benchmark_count);
//            serializeCascade(A, B, name, "./benchmarks");
//            ++benchmark_count;
//        }
//    }
//
//
//    for (int i = 10; i <= 10; i += 2) {
//        for (int j = 0; j < 5; ++j) {
//            auto A = makeRandomDFSM(4, 4, 6 * i);
//            auto B = makeRandomDFSM(4, 4, 6 * i);
//            std::string name = "cascade" + std::to_string(benchmark_count);
//            serializeCascade(A, B, name, "./benchmarks");
//            ++benchmark_count;
//        }
//    }
//
//
//    /// Variying sizes
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(4, 4, 18);
//        auto B = makeRandomDFSM(4, 4, 32);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(4, 4, 12);
//        auto B = makeRandomDFSM(4, 4, 48);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(4, 4, 8);
//        auto B = makeRandomDFSM(4, 4, 72);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//
//    for (int j = 0; j < 20; ++j) {
//        auto B = makeRandomDFSM(4, 4, 18);
//        auto A = makeRandomDFSM(4, 4, 32);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//    for (int j = 0; j < 20; ++j) {
//        auto B = makeRandomDFSM(4, 4, 12);
//        auto A = makeRandomDFSM(4, 4, 48);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//    for (int j = 0; j < 20; ++j) {
//        auto B = makeRandomDFSM(4, 4, 8);
//        auto A = makeRandomDFSM(4, 4, 72);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//
//
//    /// Varying output
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(4, 4, 24);
//        auto B = makeRandomDFSM(4, 8, 24);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(4, 4, 24);
//        auto B = makeRandomDFSM(4, 12, 24);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(4, 4, 24);
//        auto B = makeRandomDFSM(4, 2, 24);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//
//    /// Varying input
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(8, 4, 24);
//        auto B = makeRandomDFSM(4, 4, 24);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(12, 4, 24);
//        auto B = makeRandomDFSM(4, 4, 24);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(2, 4, 24);
//        auto B = makeRandomDFSM(4, 4, 24);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//
//    /// Varying connection
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(4, 8, 24);
//        auto B = makeRandomDFSM(8, 4, 24);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(4, 12, 24);
//        auto B = makeRandomDFSM(12, 4, 24);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }
//
//    for (int j = 0; j < 20; ++j) {
//        auto A = makeRandomDFSM(4, 2, 24);
//        auto B = makeRandomDFSM(2, 4, 24);
//        std::string name = "cascade" + std::to_string(benchmark_count);
//        serializeCascade(A, B, name, "./benchmarks");
//        ++benchmark_count;
//    }


}

