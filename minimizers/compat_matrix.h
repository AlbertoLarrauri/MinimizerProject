//
// Created by lazhares on 22/02/2021.
//

#ifndef PMIN_COMPAT_MATRIX_H
#define PMIN_COMPAT_MATRIX_H

#include "../machines/machines.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>

namespace SBCMin {

    class CompatMatrix {
    protected:
        int size;
        std::vector<bool> impl;
        std::vector<std::pair<int, int>> pairs;

        std::vector<int> incompat_scores;
        std::vector<int> big_clique;
        std::vector<int> ordering;

        size_t toID(int state1, int state2) const;

        void computeLargeClique(bool ordering_req);


        CompatMatrix()=default;


    public:




        bool inline areIncompatible(int state1, int state2) const {
            return impl[toID(state1, state2)];
        }


        void setIncompatible(int state1, int state2);

        inline const std::vector<std::pair<int, int>> & getPairs() const {
            return pairs;
        }




        inline const std::vector<int> & getClique() const {
            return big_clique;
        }



        inline const std::vector<int> & getOrdering() const {
            return ordering;
        }


        static CompatMatrix generateCompatMatrix(const OFA& ofa, bool clique_req=true, bool ordering_req=true);

    };


}

#endif //PMIN_COMPAT_MATRIX_H
