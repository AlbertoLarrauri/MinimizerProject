//
// Created by llarrauriborroto on 13/01/2021.
//

#include "fsms.h"

#include<random>
#include<vector>
#include<optional>


namespace machines{

    void DFSM::print() {
        std::cout<<"State / Input --> Output / State \n";
        std::cout<<"---------------------------------\n";
        for(int state = 0; state < size ; ++state) {
            for (int i = 0; i < in_alphabet_size ; ++i) {
                std::cout<<state<<" / "<<i<<" --> "<<getOut(state,i)<<" / "<<getSucc(state,i)<<"\n";
            }
        }
    }

    void OFSM::print() {
        std::cout<<"State / Input --> Output / States \n";
        std::cout<<"---------------------------------\n";
        for(int state = 0; state < size ; ++state) {
            for (int i = 0; i < in_alphabet_size ; ++i) {
                std::cout << state << " / " << i << " --> ";
                if(!hasTransition(state,i)) {
                    std::cout<<"*\n";
                } else{
                    const std::vector<int >& targets=succs(state,i);
                    std::cout<<out(state, i) << " / [ "<< targets[0];

                     for (int j=1; j<targets.size(); ++j) {
                         std::cout << ", " << targets[j];
                     }
                     std::cout<<" ] \n";
                }
            }
        }
    }

    void makeRandomDFSM(int size, DFSM& A){
        if(A.getSize()) std::cout<<"The argument machine has already been built.\n";
        A.addStates(size);
        std::default_random_engine defEngine(time(nullptr));
        std::uniform_int_distribution<int > output_generator(0,A.numberOfOutputs()-1);
        std::uniform_int_distribution<int > succ_generator(0,size-1);
        for (int state = 0; state < size; ++state) {
            for (int i = 0; i < A.numberOfInputs(); ++i) {
                A.setOut(state,i, output_generator(defEngine));
                A.setSucc(state,i,succ_generator(defEngine));
            }
        }
    }
}

