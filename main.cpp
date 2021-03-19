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

    CMSat::SATSolver solver;
    solver.new_vars(4);
    std::vector<CMSat::Lit> clause;
    clause.emplace_back(0, true);
    clause.emplace_back(1, true);
    clause.emplace_back(2, true);
    solver.add_clause(clause);
    clause.clear();
    clause.emplace_back(0, false);
    clause.emplace_back(1, true);
    clause.emplace_back(2, true);
    solver.add_clause(clause);
    clause.clear();
    clause.emplace_back(1, false);
    clause.emplace_back(2, false);
    clause.emplace_back(0,true);
    std::cout<<solver.solve(&clause)<<"\n";
    std::cout<<solver.get_conflict()<<"\n";
    std::cout<<clause<<"\n";
    auto vars=std::vector<unsigned>({0});
    solver.set_sampling_vars(&vars);
    std::cout<<solver.solve({},true)<<" Model: ";
    for(auto a:solver.get_model()){
        std::cout<<a<<"\n";
    }

    for(auto a:vars){
        std::cout<<a<<"\n";
    }
    std::cout<<solver.get_zero_assigned_lits()<<"\n";
//





    /// Sanity Checks








////    minimizer.printResult();
//



    return 0;


}

