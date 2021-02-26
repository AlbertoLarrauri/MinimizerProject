#include <iostream>
#include<vector>
#include<list>
#include<memory>
#include<chrono>
#include<optional>
#include "machines/machines.h"
#include "machines/builders.h"
#include "minimizers/ofa_minimizer.h"
//#include "machines/minimizers/driven_fsm_minimizer.h"

#include "cryptominisat5/cryptominisat.h"
#include "machines/builders.h"
#include "machines/machines.h"


int main() {


    SBCMin::DFSM A(2, 3);
    SBCMin::DFSM B(3, 2);


    makeRandomDFSM(8, A);
    makeRandomDFSM(20, B);

    SBCMin::OFA ofa = SBCMin::buildOFA(A, B);
    SBCMin::OFAMinimizer minimizer(ofa);
    minimizer.setCNFBuilder(SBCMin::OFAMinimizer::BASIC_INCREMENTAL);
    minimizer.run();
    SBCMin::DFSM B2 = minimizer.getResult();

    SBCMin::DFSM Comp1=SBCMin::buildCascadeDFSM(A,B);
    SBCMin::DFSM Comp2=SBCMin::buildCascadeDFSM(A,B2);

    std::cout<<(true==SBCMin::areEquivalent(Comp1,Comp2))<<"\n";
    std::cout<< true<<"\n";






////    minimizer.printResult();
//



    return 0;


}

