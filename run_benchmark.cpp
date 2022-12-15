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

    std::cout<<"Starting experiment..\n";
    auto [H,T]=SBCMin::deserializeCascade(cascade_path);

    auto ofa= SBCMin::buildOFA(H,T);

    SBCMin::OFAMinimizers::AssumptionBased minimizer;

    minimizer.run(ofa, T.size());
    auto result_size= minimizer.getResult().size();



    auto finish_time = clock.now();
    long double duration = std::chrono::duration_cast<std::chrono::microseconds>(finish_time - start_time).count();
    duration = duration / 1000;
    {
        std::ofstream ostream(log_path, std::ios_base::app);
        ostream<<"\"result_size\":"<<result_size<<", \n";
        ostream << "\"execution_time\":" << duration << ",\n";
    }

    return 0;




    //// Benchmark generation

//    int curr_benchmark=0;
//    for(int i=1; i<5; ++i){
//        for(int j=0; j<30; ++j) {
//            auto H = SBCMin::makeRandomDFSM(10, 4, 4);
//            auto T = SBCMin::makeRandomDFSM(10 * i, 4, 4);
//            std::string name = "benchmark" + std::to_string(curr_benchmark);
//            SBCMin::serializeCascade(H, T, name);
//            ++curr_benchmark;
//        }
//    }
//
//    for(int i=1; i<5; ++i){
//        for(int j=0; j<30; ++j) {
//            auto T = SBCMin::makeRandomDFSM(10, 4, 4);
//            auto H = SBCMin::makeRandomDFSM(10 * i, 4, 4);
//            std::string name = "benchmark" + std::to_string(curr_benchmark);
//            SBCMin::serializeCascade(H, T, name);
//            ++curr_benchmark;
//        }
//    }
//
//    return 0;

//
//    SBCMin::makeRandomDFSM()
//
//    SBCMin::NFA A(2);
//    A.addStates(3);
//    A.addSucc(0,0,2);
//    A.addSucc(0,1,1);
//    A.addSucc(0,1,2);
//    A.addSucc(1,1,2);
//    A.addSucc(2,1,0);
//
//    auto B=SBCMin::makeDeterministic(A);
//
//    std::cout<<A.isDeterministic()<<"\n"<<B.isDeterministic()<<"\n";
//
//    std::cout<<B.size()<<"\n";
//
//    B.print();
//
//    std::cout<<"\n";
//
//    B=SBCMin::minimizeDFA(B);
//
//    B.print();

//    auto driver= SBCMin::makeRandomDFSM(4,4,5);
//    auto driven = SBCMin::makeRandomDFSM(4,4,30);
//
//    SBCMin::DFSM driver(2,2);
//    driver.addStates(3);
//
//    driver.setTrans(0,0,0,1);
//    driver.setTrans(0,1,1,2);
//
//    driver.setTrans(1,0,0,1);
//    driver.setTrans(1,1,0,1);
//
//    driver.setTrans(2,0,1,2);
//    driver.setTrans(2,1,1,2);
//
//
//    SBCMin::DFSM driven(2,2);
//
//    driven.addStates(5*7);
//
//    for(uint32_t n1=0; n1<7; ++n1){
//        for(uint32_t n2=0; n2<5; ++n2){
//            auto curr= n1*5 + n2;
//            auto succ1 = (curr+5) % (5*7);
//
//            driven.setTrans(curr, 0, (n1+1)%7==0,  succ1);
//
//            auto succ2 = (curr+1) % (5*7);
//
//            driven.setTrans(curr, 1, (n2+1)%5==0,  succ2);
//
//        }
//    }
//    driven.print();



//    std::cout<<"Driver: \n";
//    driver.print();

//    std::cout<<"DFA: \n";
//
//    auto A= SBCMin::getOutputNFA(driver, driven.numberOfInputs());
//    A= SBCMin::makeDeterministic(A);
//    A= SBCMin::minimizeDFA(A);
//
//    std::cout<<"size: "<<A.size()<<"\n";

//    A.print();

//    {
//        std::cout<<" Heuristic \n";
//        SBCMin::OFAMinimizers::AssumptionBased minimizer;
//        auto ofa= SBCMin::buildHeuristicOFA(driver,driven);
//        minimizer.run(ofa, driven.size());
//        auto result= minimizer.getResult();
//
//        std::cout<<" Size "<< result.size()<< "\n";
//
//
//    }
//
//
//    {
//        std::cout<<" Proposed \n";
//        SBCMin::OFAMinimizers::AssumptionBased minimizer;
//        auto ofa= SBCMin::buildOFAV2(driver,driven);
//        minimizer.run(ofa, driven.size());
//        auto result= minimizer.getResult();
//
//        std::cout<<" Size "<< result.size()<< "\n";
//
//    }
//
//    {
//        std::cout<<" K-N \n";
//        SBCMin::OFAMinimizers::AssumptionBased minimizer;
//        auto ofa= SBCMin::buildOFAKimNewborn(driver,driven);
//        minimizer.run(ofa, driven.size());
//        auto result= minimizer.getResult();
//
//        std::cout<<" Size "<< result.size()<< "\n";
//
//    }
//



//    auto H = deserializeDFSM(std::filesystem::path("../head.txt"));
//    auto T = deserializeDFSM(std::filesystem::path("../tail.txt"));
//    std::vector<int> state_map;
//    auto ofa = buildOFA(H,T,state_map);
//
//    auto matrix=CompatMatrix::generateCompatMatrix(ofa);
//
//    auto ID = [&](int state1, int state2) {
//        return T.size() * state1 + state2;
//    };
//
//    std::cout<<"2,0 : "<<state_map[ID(0,2)]<<"\n";
//    std::cout<<"9,0 : "<<state_map[ID(0,9)]<<"\n";
//
//    for(int a= 0; a<H.size(); ++a){
//        auto p1=state_map[ID(a,2)];
//        auto p2=state_map[ID(a,9)];
//        if(p1==0|| p2==0) continue;
//
//        std::cout<<" 2 and 9 are compatible at "<<a<<" :"<<
//        !matrix.areIncompatible(p1,p2)<<"\n";
//
//
//    }

//    OFAMinimizers::AssumptionBased minimizer;
//    minimizer.run(ofa,H.size()-1);
//    auto result= minimizer.getResult();
//    serializeDFSM(result, std::cout);
    //  experimentCampaign();

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

