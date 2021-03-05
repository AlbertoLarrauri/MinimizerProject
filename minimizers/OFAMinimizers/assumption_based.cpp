//
// Created by lazhares on 03/03/2021.
//

#include "assumption_based.h"

using namespace SBCMin;
using namespace SBCMin::OFAMinimizers;


void AssumptionBased::generateVars() {
    resize(upper_bound);

    for (int set = 0; set < upper_bound; ++set) {
        for (int state = 0; state < ofa().getSize(); ++state) {
            stateSetVar(state, set) = number_of_vars++;
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

    return;

}

void AssumptionBased::init() {
    solver = std::make_unique<CMSat::SATSolver>();
    lower_bound = compat_matrix().getClique().size();
    current_size = lower_bound;
    generateVars();

    if (current_size < upper_bound) assumptions.emplace_back(sizeVar(current_size + 1), true);

    buildPartialSolutionClauses();
    if (!initial_in_partial_solution) buildCoveringClauses();
    buildFrameClauses();
    buildIncompatibilityClauses();
    buildSuccessorClauses();
    buildSizeClauses();

}

void AssumptionBased::buildPartialSolutionClauses() {
    const std::vector<int> clique = compat_matrix().getClique();
    for (int i = 0; i < clique.size(); ++i) {
        std::vector<CMSat::Lit> clause;
        clause.reserve(1);
        clause.emplace_back(stateSetVar(clique[i], i), false);
        solver->add_clause(clause);
        if (clique[i] == 0) initial_in_partial_solution = true;
    }
}

void AssumptionBased::buildCoveringClauses() {
    int partial_solution_size = compat_matrix().getClique().size();
    int initial_cover_size = std::min(partial_solution_size + 1, upper_bound);
    std::vector<CMSat::Lit> clause;
    clause.reserve(initial_cover_size);
    for (int set = 0; set < initial_cover_size; ++set) {
        clause.emplace_back(stateSetVar(0, set), false);
    }
    solver->add_clause(clause);
}

void AssumptionBased::buildFrameClauses() {
    for (int set = 0; set < upper_bound; ++set) {
        for (int in = 0; in < ofa().numberOfInputs(); ++in) {
            {
                std::vector<CMSat::Lit> clause;
                clause.reserve(upper_bound);
                for (int set2 = 0; set2 < upper_bound; ++set2) {
                    clause.emplace_back(setSetVar(set, set2, in), false);
                }
                solver->add_clause(clause);
            }
            for (int set2 = 1; set2 < upper_bound; ++set2){
                for(int set3=0; set3<set2; ++set3){
                    std::vector<CMSat::Lit> clause;
                    clause.reserve(2);
                    clause.emplace_back(setSetVar(set, set2, in), true);
                    clause.emplace_back(setSetVar(set, set3, in), true);
                }
            }
        }
    }
}

void AssumptionBased::buildIncompatibilityClauses() {
    for (int set = 0; set < upper_bound; ++set) {
        for (auto[state1, state2]:compat_matrix().getPairs()) {
            std::vector<CMSat::Lit> clause;
            clause.reserve(2);
            clause.emplace_back(stateSetVar(state1, set), true);
            clause.emplace_back(stateSetVar(state2, set), true);
            solver->add_clause(clause);
        }
    }
}

void AssumptionBased::buildSuccessorClauses() {
    for (int state = 0; state < ofa().getSize(); ++state) {
        for (int in = 0; in < ofa().numberOfInputs(); ++in) {
            if (!ofa().hasTransition(state, in)) continue;
            for (int succ:ofa().getSuccs(state, in)) {
                for (int set1 = 0; set1 < upper_bound; ++set1) {
                    for (int set2 = 0; set2 < upper_bound; ++set2) {
                        std::vector<CMSat::Lit> clause;
                        clause.reserve(3);
                        clause.emplace_back(setSetVar(set1, set2, in), true);
                        clause.emplace_back(stateSetVar(state, set1), true);
                        clause.emplace_back(stateSetVar(succ, set2), false);
                        solver->add_clause(clause);
                    }
                }
            }
        }
    }
}

void AssumptionBased::buildSizeClauses() {
    for (int size = lower_bound; size < upper_bound; ++size) {

        std::vector<CMSat::Lit> clause;
        clause.reserve(2);
        clause.emplace_back(sizeVar(size), false);
        clause.emplace_back(sizeVar(size + 1), true);
        solver->add_clause(clause);


    }

    for (int size = lower_bound; size <= upper_bound; ++size) {
        for (int state = 0; state < ofa().getSize(); ++state) {
            std::vector<CMSat::Lit> clause;
            clause.reserve(2);
            clause.emplace_back(stateSetVar(state, size - 1), true);
            clause.emplace_back(sizeVar(size), false);
            solver->add_clause(clause);
        }
        for (int set2 = 0; set2 < upper_bound; ++set2) {
            for (int in = 0; in < ofa().numberOfInputs(); ++in) {
                std::vector<CMSat::Lit> clause;
                clause.reserve(2);
                clause.emplace_back(setSetVar(set2, size - 1, in), true);
                clause.emplace_back(sizeVar(size), false);
                solver->add_clause(clause);
            }
        }
    }
}

bool AssumptionBased::step() {
    if (current_size == upper_bound) return false;
    ++current_size;
    assumptions.resize(0);
    if (current_size < upper_bound) {
        assumptions.emplace_back(sizeVar(current_size + 1), true);
    }
    return true;
}

