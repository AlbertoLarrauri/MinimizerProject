#include <iostream>
#include "machines/machines.h"
#include "machines/builders.h"
#include "minimizers/hopcroft.h"
#include "minimizers/ofa_minimizer.h"
#include "minimizers/OFAMinimizers/basic_incremental.h"
#include "minimizers/OFAMinimizers/assumption_based.h"
#include "minimizers/OFAMinimizers/w_symmetry_breaking.h"


int main() {


    SBCMin::DFSM A(2, 4);
    SBCMin::DFSM B(4, 2);




//    A.addStates(5);
//    A.setOut(0,0,0);
//    A.setOut(0,1,1);
//
//    A.setSucc(0,0,2);
//    A.setSucc(0,1,1);
//
//    A.setOut(1,0,1);
//    A.setOut(1,1,1);
//
//    A.setSucc(1,0,1);
//    A.setSucc(1,1,1);
//
//    A.setOut(2,0,0);
//    A.setOut(2,1,0);
//
//    A.setSucc(2,0,3);
//    A.setSucc(2,1,3);
//
//    A.setOut(3,0,1);
//    A.setOut(3,1,1);
//
//    A.setSucc(3,0,4);
//    A.setSucc(3,1,4);
//
//
//    A.setOut(4,0,0);
//    A.setOut(4,1,0);
//
//    A.setSucc(4,0,2);
//    A.setSucc(4,1,2);
//
//
//    B.addStates(3);
//
//    B.setOut(0,0,0);
//    B.setOut(1,0,1);
//    B.setOut(2,0,1);
//
//    B.setSucc(0,0,1);
//    B.setSucc(1,0,2);
//    B.setSucc(2,0,0);
//
//    B.setOut(0,1,1);
//    B.setOut(1,1,1);
//    B.setOut(2,1,1);
//
//    B.setSucc(0,1,1);
//    B.setSucc(1,1,2);
//    B.setSucc(2,1,0);

    makeRandomDFSM(20, A);

    makeRandomDFSM(50, B);

//    B.addStates(5);
//    B.setTrans(0,0,0,3);
//    B.setTrans(0,1,1,1);
//    B.setTrans(1,0,0,3);
//    B.setTrans(1,1,0,2);
//    B.setTrans(2,0,0,4);
//    B.setTrans(2,1,0,2);
//    B.setTrans(3,0,0,1);
//    B.setTrans(3,1,0,3);
//    B.setTrans(4,0,0,0);
//    B.setTrans(4,1,1,1);






    SBCMin::OFA ofa= SBCMin::buildOFA(A,B);
    ofa=SBCMin::OFAHopcroft::minimize(ofa);


    SBCMin::OFAMinimizers::WSymmetryBreaking exact_minimizer;

//    exact_minimizer.setCNFBuilder(SBCMin::OFAMinimizer::BASIC_INCREMENTAL);
    bool solved=exact_minimizer.run(ofa,B.getSize());
    std::cout<<"Solved: "<<solved<<"\n";
//    exact_minimizer.printVars();
//    ofa.print();

    SBCMin::DFSM B2 = exact_minimizer.getResult();


    SBCMin::DFSM Comp1=SBCMin::buildCascadeDFSM(A,B);
    SBCMin::DFSM exactComp2=SBCMin::buildCascadeDFSM(A,B2);

    std::cout<<"Is result valid: "<<(SBCMin::areEquivalent(Comp1,exactComp2)?"True":"False")<<"\n";







////    minimizer.printResult();
//



    return 0;


}

