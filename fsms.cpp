//
// Created by llarrauriborroto on 13/01/2021.
//

#include "fsms.h"
#include <iostream>

namespace machines{

    void DFSM::print() {
        std::cout<<"State / Input --> Output / State \n";
        std::cout<<"---------------------------------\n";
        for(unsigned state = 0; state < size ; ++state) {
            for (unsigned i = 0; i < in_alphabet_size ; ++i) {
                std::cout<<state<<" / "<<i<<" --> "<<out(state,i)<<" / "<<succ(state,i)<<"\n";
            }
        }
    }

    void OFSM::print() {
        std::cout<<"State / Input --> Output / States \n";
        std::cout<<"---------------------------------\n";
        for(unsigned state = 0; state < size ; ++state) {
            for (unsigned i = 0; i < in_alphabet_size ; ++i) {
                std::cout << state << " / " << i << " --> ";
                if(!hasTransition(state,i)) {
                    std::cout<<"*\n";
                } else{
                    std::vector<unsigned>& targets=succs(state,i);
                    std::cout<<out(state, i) << " / [ "<< targets[0];

                     for (unsigned j=1; j<targets.size(); j++) {
                         std::cout << ", " << targets[j];
                     }
                     std::cout<<" ] \n";
                }
            }
        }

    }

}

