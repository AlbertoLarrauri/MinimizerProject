//
// Created by llarrauriborroto on 23/02/2021.
//

#include "compat_matrix.h"
#include "cnf_builders.h"
#include "ofa_minimizer.h"

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

    }

    cnf_builder_ptr->run();
    result=&(cnf_builder_ptr->getResult());

}
