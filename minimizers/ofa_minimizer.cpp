//
// Created by llarrauriborroto on 23/02/2021.
//

#include "compat_matrix.h"
#include "CNFBuilders/basic_incremental.h"
#include "ofa_minimizer.h"
#include "CNFBuilders/basic_non_incremental.h"

using namespace SBCMin;


void OFACNFBuilder::run() {
    init();
    while (!trySolve()) {
        step();
    }
    computeSolution();
}

void OFAMinimizer::run() {
    compat_matrix_ptr=std::make_unique<CompatMatrix>(ofa, clique_needed);

    switch (builder_type) {
        case BASIC_INCREMENTAL:
            cnf_builder_ptr=std::make_unique<OFACNFBuilders::BasicIncremental>(ofa, *compat_matrix_ptr);
        case BASIC_NON_INCREMENTAL:
            cnf_builder_ptr=std::make_unique<OFACNFBuilders::BasicNonIncremental>(ofa, *compat_matrix_ptr);
    }

    cnf_builder_ptr->run();
    result=&(cnf_builder_ptr->getResult());

}
