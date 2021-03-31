//
// Created by llarrauriborroto on 23/03/2021.
//


#include "w_state_priority.h"

using namespace SBCMin::OFAMinimizers;


void WStatePriority::init() {
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


void WStatePriority::generateVars() {
    resize(upper_bound);

    for (int set = 0; set < upper_bound; ++set) {
        for (int state = 0; state < ofa().size(); ++state) {
            stateSetVar(state, set) = number_of_vars++;
            usedVar(state,set)=number_of_vars++;
        }

        for (int set2 = 0; set2 < upper_bound; ++set2) {
            for (int in = 0; in < ofa().numberOfInputs(); ++in) {
                setSetVar(set, set2, in) = number_of_vars++;
            }
        }
    }

    for (int size = lower_bound; size <= upper_bound; ++size) {
        sizeVar(size) = number_of_vars++;
    }


    solver->new_vars(number_of_vars);

}


void WStatePriority::buildCoveringClauses() {
    std::vector<CMSat::Lit> clause;
    clause.reserve(upper_bound);
    for (int set = 0; set < upper_bound; ++set) {
        clause.emplace_back(stateSetVar(0, set), false);
    }
    solver->add_clause(clause);
}


void WStatePriority::buildSymmetryBreakingClauses() {

    const std::vector<int>& ordering=compat_matrix().getOrdering();

    for(int set=lower_bound; set<upper_bound; ++set){
        std::vector<CMSat::Lit> clause;
        clause.reserve(upper_bound-lower_bound);
        clause.emplace_back(stateSetVar(ordering[0], set), false);
        clause.emplace_back(usedVar(ordering[0], set), true);
        solver->add_clause(clause);
    }

    for (auto stateID = 0; stateID < ordering.size(); ++stateID) {
        for (int in = 0; in < ofa().numberOfInputs(); ++in) {
            int state=ordering[stateID];
            for (int set = lower_bound; set < upper_bound; ++set) {
                {
                    std::vector<CMSat::Lit> clause;
                    clause.reserve(2);
                    clause.emplace_back(stateSetVar(state, set), true);
                    clause.emplace_back(usedVar(state, set), false);
                    solver->add_clause(clause);
                }

                if (set > lower_bound) {
                    std::vector<CMSat::Lit> clause;
                    clause.reserve(2);
                    clause.emplace_back(stateSetVar(state, set), true);
                    clause.emplace_back(usedVar(state, set - 1), false);
                    solver->add_clause(clause);
                }

                if (stateID > 0) {
                    int prev_state=ordering[stateID-1];
                    {
                        std::vector<CMSat::Lit> clause;
                        clause.reserve(3);
                        clause.emplace_back(usedVar(prev_state, set), false);
                        clause.emplace_back(usedVar(state, set), true);
                        clause.emplace_back(stateSetVar(state, set), false);
                        solver->add_clause(clause);
                    }
                    {
                        std::vector<CMSat::Lit> clause;
                        clause.reserve(2);
                        clause.emplace_back(usedVar(prev_state, set), true);
                        clause.emplace_back(usedVar(state, set), false);
                        solver->add_clause(clause);

                    }
                }
            }
        }
    }
    for (int set = lower_bound; set < upper_bound; ++set) {
        int last_state=ordering.back();
        {
            std::vector<CMSat::Lit> clause;
            clause.reserve(2);
            clause.emplace_back(sizeVar(set + 1), false);
            clause.emplace_back(usedVar(last_state, set), true);
            solver->add_clause(clause);
        }

//        {
//
//            std::vector<CMSat::Lit> clause;
//            clause.reserve(2);
//            clause.emplace_back(sizeVar(set + 1), true);
//            clause.emplace_back(usedVar(last_state, set), false);
//            solver->add_clause(clause);
//        }
    }
}

bool WStatePriority::query(int size) {
    current_size=size;
    std::vector<CMSat::Lit> assumptions(0);
    if(size<upper_bound){
        assumptions.emplace_back(sizeVar(size+1),true);
    }
    return trySolve(assumptions);
}



