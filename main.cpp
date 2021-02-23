#include <iostream>
#include<vector>
#include<list>
#include<memory>
#include<chrono>
#include<optional>
#include "fsms.h"
#include "driven_fsm_minimizer.h"

#include "cryptominisat5/cryptominisat.h"



int main()
{


    DFSM A(2,2);
    DFSM B(2,2);


    makeRandomDFSM(8,A);
    makeRandomDFSM(20,B);

    std::cout<<"Driver machine table: \n";
    A.print();
    std::cout<<"\nDriven machine table: \n";
    B.print();
    minimizers::DrivenFSMMinimizer minimizer(A,B);
    minimizer.buildOFSM();
//    const OFSM& O=minimizer.getOFSM();
//  std::cout<<"\nObservation machine table: \n";
//   O.print();

    minimizer.buildCMatrix();
//    minimizer.check();

////   minimizer.printIncompatibles();
////   minimizer.printBigClique();
    minimizer.solve();
////    minimizer.printResult();
//



    return 0;




