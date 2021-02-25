//
// Created by lazhares on 22/02/2021.
//

#ifndef PMIN_COMPAT_MATRIX_H
#define PMIN_COMPAT_MATRIX_H

#include "ofa_minimizer.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>

namespace SBCMin {

    class CompatMatrix {
    private:
        int size;
        std::vector<bool> impl;
        std::vector<std::pair<int, int>> pairs;

        bool is_clique_needed;
        std::vector<int> incompat_scores;
        std::vector<int> big_clique;


        size_t toID(int state1, int state2);

        void computeLargeClique();

    public:

        CompatMatrix(OFA &ofa, bool _is_clique_needed = true);

        std::vector<int> &getClique();

        bool areIncompatible(int state1, int state2);

        void setIncompatible(int state1, int state2);

        std::vector<std::pair<int, int>> &getPairs();


    };
}

#endif //PMIN_COMPAT_MATRIX_H
