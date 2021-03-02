//
// Created by llarrauriborroto on 13/01/2021.
//

#include "machines.h"


#include<vector>
#include <unordered_set>


using namespace SBCMin;

void DFSM::print() {
    std::cout << "State / Input --> Output / State \n";
    std::cout << "---------------------------------\n";
    for (int state = 0; state < size; ++state) {
        for (int i = 0; i < in_alphabet_size; ++i) {
            std::cout << state << " / " << i << " --> " << getOut(state, i) << " / " << getSucc(state, i) << "\n";
        }
    }
}

void OFA::print() {
    std::cout << "State / Input --> Output / States \n";
    std::cout << "---------------------------------\n";
    for (int state = 0; state < size; ++state) {
        for (int i = 0; i < in_alphabet_size; ++i) {
            std::cout << state << " / " << i << " --> ";
            if (!hasTransition(state, i)) {
                std::cout << "*\n";
            } else {
                const std::vector<int> &targets = succs(state, i);
                std::cout << out(state, i) << " / [ " << targets[0];

                for (int j = 1; j < targets.size(); ++j) {
                    std::cout << ", " << targets[j];
                }
                std::cout << " ] \n";
            }
        }
    }
}


bool SBCMin::areEquivalent(const DFSM &A, const DFSM &B) {
    if (&A == &B) return true;
    if (A.numberOfInputs() != B.numberOfInputs()) return false;
    if (A.getSize() == 0 && B.getSize() == 0) return true;
    if (A.getSize() == 0 || B.getSize() == 0) return false;

    int inputs = A.numberOfInputs();
    int size1 = A.getSize();
    int size2 = B.getSize();
    auto ID = [size2](int state1, int state2) {
        return size2 * state1 + state2;
    };

    std::unordered_set<int> visited_states;
    std::vector<int> unexplored;
    unexplored.reserve(size1*size2);
    unexplored.push_back(0);
    visited_states.insert(0);

    while (!unexplored.empty()) {
        int pair = unexplored.back();
        unexplored.pop_back();

        int state1 = pair / size2;
        int state2 = pair % size2;


        for (int i = 0; i < inputs; ++i) {
            int o = A.getOut(state1, i);
            int t = B.getOut(state2, i);
            if (o != t) return false;
            int next1 = A.getSucc(state1, i);
            int next2 = B.getSucc(state2, i);
            int next_pair = ID(next1, next2);

            if (!visited_states.count(next_pair)) {
                visited_states.insert(next_pair);
                unexplored.push_back(next_pair);
            }

        }
    }

    return true;
}


