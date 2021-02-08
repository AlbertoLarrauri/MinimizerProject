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


    makeRandomDFSM(25,A);
    makeRandomDFSM(40,B);

    std::cout<<"Driver machine table: \n";
    A.print();
    std::cout<<"\nDriven machine table: \n";
    B.print();
    minimizers::DrivenFSMMinimizer minimizer(A,B);
    minimizer.buildOFSM();
    OFSM O=minimizer.getOFSM();
//  std::cout<<"\nObservation machine table: \n";
//   O.print();

    minimizer.buildCMatrix();


    minimizer.processCMatrix();
//   minimizer.printIncompatibles();
//   minimizer.printBigClique();
    minimizer.solve();
//    minimizer.printResult();




    return 0;


}

//    CMSat::SATSolver solver;
//    std::vector<CMSat::Lit> clause;
//
//    //Let's use 4 threads
//    solver.set_num_threads(4);
//
//    //We need 3 variables. They will be: 0,1,2
//    //Variable numbers are always trivially increasing
//    solver.new_vars(3);
//
//    //add "1 0"
//    clause.emplace_back(0, false);
//    solver.add_clause(clause);
//
//    //add "-2 0"
//    clause.clear();
//    clause.push_back(CMSat::Lit(1, true));
//    solver.add_clause(clause);
//
//    //add "-1 2 3 0"
//    clause.clear();
//    clause.push_back(CMSat::Lit(0, true));
//    clause.push_back(CMSat::Lit(1, false));
//    clause.push_back(CMSat::Lit(2, false));
//    solver.add_clause(clause);
//
//    CMSat::lbool ret = solver.solve();
//    assert(ret == CMSat::l_True);
//    std::cout
//            << "Solution is: "
//            << solver.get_model()[0]
//            << ", " << solver.get_model()[1]
//            << ", " << solver.get_model()[2]
//            << std::endl;
//
//    //assumes 3 = FALSE, no solutions left
//    std::vector<CMSat::Lit> assumptions;
//    assumptions.emplace_back(2, true);
//    ret = solver.solve(&assumptions);
//    assert(ret ==CMSat::l_False);
//
//    //without assumptions we still have a solution
//    ret = solver.solve();
//    assert(ret == CMSat::l_True);
//
//    //add "-3 0"
//    //No solutions left, UNSATISFIABLE returned
//    clause.clear();
//    clause.emplace_back(2, true);
//    solver.add_clause(clause);
//    ret = solver.solve();
//    assert(ret == CMSat::l_False);



//    A.addStates(5);
//    B.addStates(5);
//    A.out(0,0)=0;
//    A.out(0,1)=0;
//    A.out(1,0)=1;
//    A.out(1,1)=0;
//    A.out(2,0)=1;
//    A.out(2,1)=1;
//    A.out(3,0)=1;
//    A.out(3,1)=1;
//    A.out(4,0)=0;
//    A.out(4,1)=0;
//
//    A.succ(0,0)=0;
//    A.succ(0,1)=4;
//    A.succ(1,0)=1;
//    A.succ(1,1)=0;
//    A.succ(2,0)=1;
//    A.succ(2,1)=0;
//    A.succ(3,0)=0;
//    A.succ(3,1)=0;
//    A.succ(4,0)=3;
//    A.succ(4,1)=0;
//
//
//
//    B.out(0,0)=0;
//    B.out(0,1)=1;
//    B.out(1,0)=3;
//    B.out(1,1)=0;
//    B.out(2,0)=2;
//    B.out(2,1)=3;
//    B.out(3,0)=3;
//    B.out(3,1)=3;
//    B.out(4,0)=0;
//    B.out(4,1)=0;
//
//    B.succ(0,0)=0;
//    B.succ(0,1)=4;
//    B.succ(1,0)=1;
//    B.succ(1,1)=0;
//    B.succ(2,0)=1;
//    B.succ(2,1)=0;
//    B.succ(3,0)=0;
//    B.succ(3,1)=0;
//    B.succ(4,0)=3;
//    B.succ(4,1)=0;


//    auto time0=std::chrono::high_resolution_clock::now();
//    std::list<int> list1={};
//    std::vector<int> vect={};
//     vect.reserve(100000);
//    std::cout<<vect.capacity()<<"\n";
//    for (int i = 0; i < 100000; ++i) {
//        list1.push_back(i);
////        vect.push_back(i);
//    }
//   vect.insert(vect.end(),list1.begin(),list1.end());
//
//
//    auto time1=std::chrono::high_resolution_clock::now();
//    auto delay=std::chrono::duration_cast<std::chrono::microseconds>(time1-time0);
//    std::cout<<delay.count();
////    opt=std::make_unique<A>(2);
////    opt=std::make_unique<A>(3);
////    std::cout<<(opt.value()->a);
//





