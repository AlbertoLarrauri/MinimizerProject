#include <iostream>
#include "machines/machines.h"
#include "machines/builders.h"
#include "minimizers/hopcroft.h"
#include "minimizers/ofa_minimizer.h"
#include "minimizers/OFAMinimizers/assumption_based.h"
#include "minimizers/OFAMinimizers/w_symmetry_breaking.h"
#include "machines/serializer.h"
#include "minimizers/monotone_range_searchers.h"

int main() {

    SBCMin::DFSM A = SBCMin::makeRandomDFSM(2, 4, 20);
    SBCMin::DFSM B = SBCMin::makeRandomDFSM(4, 2, 30);
    SBCMin::OFA ofa = SBCMin::buildOFA(A, B);




    {
        SBCMin::OFAMinimizers::AssumptionBased minimizer;
        minimizer.setStrategy(SBCMin::BasicBinaryMRS());
        std::chrono::high_resolution_clock clock;
        auto start=clock.now();
        minimizer.run(ofa, B.getSize());
        auto end=clock.now();
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        std::cout<<"Duration: "<<duration<<" \n";
    }
    {
        SBCMin::OFAMinimizers::AssumptionBased minimizer;
        std::chrono::high_resolution_clock clock;
        auto start=clock.now();
        minimizer.run(ofa, B.getSize());
        auto end=clock.now();
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        std::cout<<"Duration: "<<duration<<" \n";
    }

    {
        SBCMin::OFAMinimizers::WSymmetryBreaking minimizer;
        minimizer.setStrategy(SBCMin::BasicBinaryMRS());
        std::chrono::high_resolution_clock clock;
        auto start=clock.now();
        minimizer.run(ofa, B.getSize());
        auto end=clock.now();
        auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        std::cout<<"Duration: "<<duration<<" \n";
    }


//





    /// Sanity Checks








////    minimizer.printResult();
//



    return 0;


}

