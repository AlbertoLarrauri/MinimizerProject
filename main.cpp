#include <iostream>
#include "machines/machines.h"
#include "machines/builders.h"
#include "minimizers/hopcroft.h"
#include "minimizers/ofa_minimizer.h"
#include "minimizers/OFAMinimizers/assumption_based.h"
#include "minimizers/OFAMinimizers/w_symmetry_breaking.h"
#include "machines/serializer.h"

int main() {

    SBCMin::DFSM A(2,4);
//    {
//        auto builder = SBCMin::ModularDFSMBuilder(4, 8, 20);
//        builder.setNumberOfBags(2);
//        builder.setPadding(2);
//        builder.setEdgeProbability(2.0/(4*20.0));
//
//        A = builder.getDFSM();
//    }
    A=SBCMin::makeRandomDFSM(2,4,20);

    A=SBCMin::DFSMHopcroft::minimize(A);
    std::cout<<"Driver size: "<<A.getSize()<<"\n";

    SBCMin::DFSM B(4,2);

//    {
//        auto builder = SBCMin::ModularDFSMBuilder(8, 4, 80);
//        builder.setPadding(2);
//        builder.setNumberOfBags(6);
//        builder.setEdgeProbability(6.0/(8*80.0));
//        B = builder.getDFSM();
//    }
    B=SBCMin::makeRandomDFSM(4,2,40);

    B=SBCMin::DFSMHopcroft::minimize(B);
    std::cout<<"Driven size: "<<B.getSize()<<"\n";

//    std::cout<<"Coverage: "<<SBCMin::getStateCoverage(A,B)<<"\n";


    SBCMin::OFA ofa = SBCMin::buildOFA(A, B);
    SBCMin::OFA ofa_min=SBCMin::OFAHopcroft::minimize(ofa);

    std::cout<<"OFA size: "<<ofa.getSize()<<"\n Minimized OFA size: "<<ofa_min.getSize()<<"\n";


    SBCMin::OFA heuristic_ofa = SBCMin::buildHeuristicOFA(A, B);

    std::chrono::high_resolution_clock clock;

    for(int i=0; i<4; ++i) {

        {
            auto begin = clock.now();

            SBCMin::OFAMinimizers::AssumptionBased minimizer;
            minimizer.run(ofa, B.getSize());
            auto end = clock.now();
            std::cout<< "No preprocessing:\n";
            std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                      << " ms.\n";
            SBCMin::DFSM B2 = minimizer.getResult();
            SBCMin::DFSM Comp1 = SBCMin::buildCascadeDFSM(A, B);
            SBCMin::DFSM exactComp2 = SBCMin::buildCascadeDFSM(A, B2);
            assert(SBCMin::areEquivalent(Comp1, exactComp2));

        }

        {
            auto begin = clock.now();

            SBCMin::OFAMinimizers::AssumptionBased minimizer;
            minimizer.run(ofa_min, B.getSize());
            auto end = clock.now();
            std::cout<< "With preprocessing:\n";
            std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                      << " ms.\n";
            SBCMin::DFSM B2 = minimizer.getResult();
            SBCMin::DFSM Comp1 = SBCMin::buildCascadeDFSM(A, B);
            SBCMin::DFSM exactComp2 = SBCMin::buildCascadeDFSM(A, B2);
            assert(SBCMin::areEquivalent(Comp1, exactComp2));

        }

    }
//
//





    /// Sanity Checks








////    minimizer.printResult();
//



    return 0;


}

