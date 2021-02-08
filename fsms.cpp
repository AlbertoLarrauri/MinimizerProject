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
                    const std::vector<unsigned>& targets=succs(state,i);
                    std::cout<<out(state, i) << " / [ "<< targets[0];

                     for (unsigned j=1; j<targets.size(); ++j) {
                         std::cout << ", " << targets[j];
                     }
                     std::cout<<" ] \n";
                }
            }
        }

//        std::cout<<"State / <-- Input,Output / States \n";
//        std::cout<<"---------------------------------\n";
//        for(unsigned state = 0; state < size ; ++state) {
//            for(auto pair:sourceData(state)){
//                size_t io=pair.first;
//                unsigned o=io/in_alphabet_size;
//                unsigned i=io%in_alphabet_size;
//                auto sources=pair.second;
//                std::cout << state << " <-- " << i <<", "<<o<< " / ";
//
//                std::cout<<" [ "<< sources[0];
//                    for (unsigned j=1; j<sources.size(); ++j) {
//                        std::cout << ", " << sources[j];
//                    }
//                    std::cout<<" ] \n";
//
//            }
//        }
//
//
//

    }

    void makeRandomDFSM(unsigned size, DFSM& A){
        if(A.getSize()) std::cout<<"The argument machine has already been built.\n";
        A.addStates(size);
        std::default_random_engine defEngine(time(nullptr));
        std::uniform_int_distribution<unsigned> output_generator(0,A.numberOfOutputs()-1);
        std::uniform_int_distribution<unsigned> succ_generator(0,size-1);
        for (unsigned state = 0; state < size; ++state) {
            for (unsigned i = 0; i < A.numberOfInputs(); ++i) {
                A.out(state,i)=output_generator(defEngine);
                A.succ(state,i)=succ_generator(defEngine);
            }
        }

    }


}

