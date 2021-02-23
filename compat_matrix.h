//
// Created by lazhares on 22/02/2021.
//

#ifndef PMIN_COMPAT_MATRIX_H
#define PMIN_COMPAT_MATRIX_H


#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>

class CompatMatrix {
private:
    int size;
    std::vector<bool> impl;
    std::vector<std::pair<int,int>> pairs;

    bool is_clique_needed;
    std::vector<int> incompat_scores;




    inline size_t toID(int state1, int state2){
        if (state1>=state2){
            return (state1*(state1+1))/2 + state2;
        }
        return (state2*(state2+1))/2 + state1;
    }

public:

    CompatMatrix(int _size, bool _is_clique_needed= true):
    size(_size),
    impl((_size*(_size+1))/2),
    is_clique_needed(_is_clique_needed)
    {
        if(is_clique_needed) incompat_scores.resize(_size);
    }

    bool inline areIncompatible(int state1, int state2){
        return impl[toID(state1,state2)];
    }

    void setIncompatible(int state1, int state2){

        if(areIncompatible(state1,state2)) return;

        if (is_clique_needed){
            incompat_scores[state1]++;
            incompat_scores[state2]++;
        }

        impl[toID(state1,state2)]= true;


        if (state1>=state2){
            pairs.emplace_back(state1,state2);
        } else {
            pairs.emplace_back(state2,state1);
        }
    }

    std::vector<int> computeLargeClique(){
        //TODO: this should be an exception
        if(!is_clique_needed) throw "Clique generation is disabled";

        std::vector<int> big_clique;
        if (pairs.empty()) {
            big_clique.push_back(0);
            return big_clique;
        }

        auto comparator = [this](int state1, int state2) -> bool {
            return (incompat_scores[state1] < incompat_scores[state2]);
        };


        std::vector<int > ordered_states(size);
        std::iota(ordered_states.begin(), ordered_states.end(), 0);
        std::sort(ordered_states.begin(), ordered_states.end(), comparator);
        big_clique.push_back(ordered_states[0]);

        for (int stateID = 1; stateID < size; ++stateID) {
            int const &state1 = ordered_states[stateID];
            bool valid = true;
            for (int state2:big_clique) {
                if (!areIncompatible(state1, state2)) {
                    valid = false;
                    break;
                }
            }
            if (valid) big_clique.push_back(state1);
        }
        return big_clique;
    }

    inline std::vector<std::pair<int, int>>& getPairs(){
        return pairs;
    }

    void check(){
        for (auto pair:pairs) {
            if(!areIncompatible(pair.first,pair.second)){
                std::cout<<"Bugazo\n";
            }

        }
    }

};


#endif //PMIN_COMPAT_MATRIX_H
