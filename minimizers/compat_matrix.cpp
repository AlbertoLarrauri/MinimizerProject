//
// Created by lazhares on 22/02/2021.
//

#include <deque>
#include "compat_matrix.h"

using namespace SBCMin;

inline size_t CompatMatrix::toID(int state1, int state2) const {
    if (state1 >= state2) {
        return (state1 * (state1 + 1)) / 2 + state2;
    }
    return (state2 * (state2 + 1)) / 2 + state1;
}


void CompatMatrix::setIncompatible(int state1, int state2) {
    if (areIncompatible(state1, state2)) return;
    incompat_scores[state1]++;
    incompat_scores[state2]++;

    impl[toID(state1, state2)] = true;


    if (state1 >= state2) {
        pairs.emplace_back(state1, state2);
    } else {
        pairs.emplace_back(state2, state1);
    }
}

CompatMatrix CompatMatrix::generateCompatMatrix(const OFA &ofa, bool clique_req, bool ordering_req) {
    CompatMatrix matrix;
    int &size = matrix.size;
    std::vector<bool> &impl = matrix.impl;
    std::vector<int> &big_clique = matrix.big_clique;
    std::vector<int> &ordering = matrix.ordering;

    size = ofa.size();
    impl.resize((size * (size + 1)) / 2);
    matrix.incompat_scores.resize(size);

    const int inputs = ofa.numberOfInputs();

    std::deque<std::pair<int, int>> modified;

    for (int state1 = 1; state1 < size; ++state1) {
        for (int state2 = 0; state2 < state1; ++state2) {
            if (matrix.areIncompatible(state1, state2)) continue;

            for (int i = 0; i < inputs; ++i) {
                if (ofa.hasTransition(state1, i) &&
                    ofa.hasTransition(state2, i) &&
                    (ofa.getOut(state1, i) != ofa.getOut(state2, i))) {
                    matrix.setIncompatible(state1, state2);
                    modified.emplace_back(state1, state2);

                    while (!modified.empty()) {
                        auto mstate1 = modified.front().first;
                        auto mstate2 = modified.front().second;
                        modified.pop_front();
                        auto &sources1 = ofa.sourceData(mstate1);
                        auto &sources2 = ofa.sourceData(mstate2);
                        for (auto pair:sources1) {
                            if (sources2.count(pair.first)) {
                                auto parents1 = pair.second;
                                auto parents2 = sources2.at(pair.first);
                                for (auto parent1:parents1) {
                                    for (auto parent2:parents2) {
                                        if (matrix.areIncompatible(parent1, parent2)) continue;
                                        matrix.setIncompatible(parent1, parent2);
                                        modified.emplace_back(parent1, parent2);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (!clique_req) {
        big_clique.push_back(0);
        return matrix;
    }

    matrix.computeLargeClique(ordering_req);
    return matrix;
}


void CompatMatrix::computeLargeClique(bool ordering_req) {
    ordering.clear();
    big_clique.clear();
    if (pairs.empty()) {
        big_clique.push_back(0);
        return;
    }

    auto comparator = [&](int state1, int state2) -> bool {
        return (incompat_scores[state1] < incompat_scores[state2]);
    };


    std::vector<int> ordered_states(size);
    std::iota(ordered_states.begin(), ordered_states.end(), 0);
    std::sort(ordered_states.begin(), ordered_states.end(), comparator);
    big_clique.push_back(ordered_states.back());
    ordered_states.pop_back();
    auto stateID = ordered_states.size();
    while (stateID != 0) {
        --stateID;
        int const &state1 = ordered_states[stateID];
        bool valid = true;
        for (int state2:big_clique) {
            if (!areIncompatible(state1, state2)) {
                valid = false;
                break;
            }
        }
        if (valid) {
            big_clique.push_back(state1);

            ordered_states.erase(ordered_states.begin() + stateID);
        }
    }

    if (!ordering_req) return;

    ordering.reserve(size);
    ordering.insert(ordering.end(), big_clique.begin(), big_clique.end());
    ordering.insert(ordering.end(), ordered_states.rbegin(), ordered_states.rend());
}






