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


int main() {


    SBCMin::DFSM A(2, 2);
    SBCMin::DFSM B(2, 2);


    makeRandomDFSM(8, A);
    makeRandomDFSM(20, B);

    SBCMin::OFA ofa = SBCMin::buildOFA(A, B);
    SBCMin::OFAMinimizer minimizer(ofa);
    minimizer.run();
    SBCMin::DFSM B2 = minimizer.getResult();






////    minimizer.printResult();
//



    return 0;


}

