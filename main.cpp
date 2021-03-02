#include <iostream>
#include "machines/machines.h"
#include "machines/builders.h"
#include "minimizers/hopcroft.h"
#include "minimizers/ofa_minimizer.h"


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

    makeRandomDFSM(10, A);

    makeRandomDFSM(40, B);

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







    SBCMin::DFSM B2 = SBCMin::DFSMHopcroft::minimize(B);

    std::cout << "Is hopcroft result valid: " << (SBCMin::areEquivalent(B, B2) ? "True" : "False") << "\n";
    std::cout << B2.getSize() << "\n";
    std::cout << B.getSize() << "\n";



    SBCMin::OFA ofa= SBCMin::buildHeuristicOFA(A,B);
//    std::cout<<"Driver table: \n";
//    A.print();
//
//    std::cout<<"\n OFA table: \n";
//    ofa.print();

//    std::cout<<"Heuristic minimization: \n";
//
//    SBCMin::OFAMinimizer minimizer(ofa);
//    minimizer.setCNFBuilder(SBCMin::OFAMinimizer::BASIC_INCREMENTAL);
//    minimizer.run();
//    SBCMin::DFSM B3= minimizer.getResult();
//

//    SBCMin::DFSM Comp2=SBCMin::buildCascadeDFSM(A,B3);
//



    std::cout<<"\n Exact minimization: \n";

    SBCMin::OFA exact_ofa= SBCMin::buildOFA(A,B);
    SBCMin::OFA mini_ofa = SBCMin::OFAHopcroft::minimize(exact_ofa);

    std::cout<<" OFA minimization: "<<exact_ofa.getSize()<<", "<<mini_ofa.getSize()<<"\n";

    SBCMin::OFAMinimizer exact_minimizer(mini_ofa);
    exact_minimizer.setCNFBuilder(SBCMin::OFAMinimizer::BASIC_INCREMENTAL);
    exact_minimizer.run();
    SBCMin::DFSM exactB2 = exact_minimizer.getResult();
    SBCMin::DFSM Comp1=SBCMin::buildCascadeDFSM(A,B);
    SBCMin::DFSM exactComp2=SBCMin::buildCascadeDFSM(A,B2);

    std::cout<<"Is result valid: "<<(SBCMin::areEquivalent(Comp1,exactComp2)?"True":"False")<<"\n";







////    minimizer.printResult();
//



    return 0;


}

