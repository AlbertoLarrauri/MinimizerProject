//
// Created by lazhares on 05/03/2021.
//

#include "w_symmetry_breaking.h"

using namespace SBCMin::OFAMinimizers;


void WSymmetryBreaking::init() {
    solver = std::make_unique<CMSat::SATSolver>();
    lower_bound = compat_matrix().getClique().size();
    current_size = lower_bound;
    generateVars();

    buildPartialSolutionClauses();
    if (!initial_in_partial_solution) buildCoveringClauses();

    buildFrameClauses();
    buildIncompatibilityClauses();
    buildSuccessorClauses();
    buildSizeClauses();
    if (lower_bound < upper_bound) buildSymmetryBreakingClauses();
}


void WSymmetryBreaking::generateVars() {
    resize(upper_bound);

    for (int set = 0; set < upper_bound; ++set) {
        for (int state = 0; state < ofa().getSize(); ++state) {
            stateSetVar(state, set) = number_of_vars++;
        }

        for (int set2 = 0; set2 < upper_bound; ++set2) {
            for (int in = 0; in < ofa().numberOfInputs(); ++in) {
                setSetVar(set, set2, in) = number_of_vars++;
                if (set2 >= lower_bound) usedVar(set, set2, in) = number_of_vars++;
            }
        }
    }

    for (int size = lower_bound; size <= upper_bound; ++size) {
        sizeVar(size) = number_of_vars++;
    }


    solver->new_vars(number_of_vars);

    return;

}


void WSymmetryBreaking::buildCoveringClauses() {
    int partial_solution_size = compat_matrix().getClique().size();
    std::vector<CMSat::Lit> clause;
    clause.reserve(upper_bound);
    for (int set = 0; set < upper_bound; ++set) {
        clause.emplace_back(stateSetVar(0, set), false);
    }
    solver->add_clause(clause);
}


void WSymmetryBreaking::buildSymmetryBreakingClauses() {

    {
        std::vector<CMSat::Lit> clause;
        clause.reserve(2);
        clause.emplace_back(setSetVar(0, lower_bound, 0), false);
        clause.emplace_back(usedVar(0, lower_bound, 0), true);
        solver->add_clause(clause);
    }



    for (int set = lower_bound+1; set < upper_bound; ++set) {
        std::vector<CMSat::Lit> clause;
        clause.reserve(1);
        clause.emplace_back(usedVar(0, set, 0), true);
        solver->add_clause(clause);
    }


    for (int set1 = 0; set1 < upper_bound; ++set1) {
        for (int in = 0; in < ofa().numberOfInputs(); ++in) {
            for (int set2 = lower_bound; set2 < upper_bound; ++set2) {
                {
                    std::vector<CMSat::Lit> clause;
                    clause.reserve(2);
                    clause.emplace_back(setSetVar(set1, set2, in), true);
                    clause.emplace_back(usedVar(set1, set2, in), false);
                    solver->add_clause(clause);
                }


                if (in + set1 > 0) {

                    auto[prev_set, prev_in]=prev(set1, in);

                    if (set2 > lower_bound) {
                        std::vector<CMSat::Lit> clause;
                        clause.reserve(2);
                        clause.emplace_back(setSetVar(set1, set2, in), true);
                        clause.emplace_back(usedVar(prev_set, set2 - 1, prev_in), false);
                        solver->add_clause(clause);
                    }

                    {
                        std::vector<CMSat::Lit> clause;
                        clause.reserve(3);
                        clause.emplace_back(usedVar(prev_set, set2, prev_in), false);
                        clause.emplace_back(usedVar(set1, set2, in), true);
                        clause.emplace_back(setSetVar(set1, set2, in), false);
                        solver->add_clause(clause);
                    }
                    {
                        std::vector<CMSat::Lit> clause;
                        clause.reserve(2);
                        clause.emplace_back(usedVar(prev_set, set2, prev_in), true);
                        clause.emplace_back(usedVar(set1, set2, in), false);
                        solver->add_clause(clause);

                    }
                }
            }
        }
    }

    for (int set = lower_bound; set < upper_bound; ++set) {
        {

            std::vector<CMSat::Lit> clause;
            clause.reserve(2);
            clause.emplace_back(sizeVar(set + 1), false);
            clause.emplace_back(usedVar(upper_bound - 1, set, ofa().numberOfInputs() - 1), true);
            solver->add_clause(clause);
        }

        {

            std::vector<CMSat::Lit> clause;
            clause.reserve(2);
            clause.emplace_back(sizeVar(set + 1), true);
            clause.emplace_back(usedVar(upper_bound - 1, set, ofa().numberOfInputs() - 1), false);
            solver->add_clause(clause);
        }


    }

}

bool WSymmetryBreaking::query(int size) {
    current_size=size;
    std::vector<CMSat::Lit> assumptions(0);
    assumptions.emplace_back(sizeVar(size), false);
    if(size<upper_bound){
        assumptions.emplace_back(sizeVar(size+1),true);
    }
    return trySolve(assumptions);
}



