//
// Created by llarrauriborroto on 23/02/2021.
//

#ifndef PMIN_OFA_MINIMIZER_H
#define PMIN_OFA_MINIMIZER_H

#include "../machines/machines.h"
//#include "compat_matrix.h"

namespace SBCMin {

    class CompatMatrix;

    namespace OFA_CNFBuilders {
        class CNFBuilder;
    };

    class OFAMinimizer {

    private:

        using CNFBuilder=OFA_CNFBuilders::CNFBuilder;

        OFA &ofa;

        std::unique_ptr<CompatMatrix> compat_matrix;
        std::unique_ptr<CNFBuilder> cnf_builder;

        std::unique_ptr<DFSM> result;
        std::vector<int> big_clique;

        void generateMatrix();

    public:


        OFAMinimizer(OFA &_ofa) : ofa(_ofa) {
        }

        DFSM& minimize();

    };
}


#endif //PMIN_OFA_MINIMIZER_H
