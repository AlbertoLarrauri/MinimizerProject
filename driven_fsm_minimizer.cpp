//
// Created by llarrauriborroto on 28/01/2021.
//

#include <deque>
#include "driven_fsm_minimizer.h"

using namespace machines;
namespace minimizers{
    void DrivenFSMMinimizer::buildOFSM() {
        if(driver.getSize()==0 || driver.getSize()==0){
            ofsm_init=true;
            return;
        }
        unsigned inputs=driver.numberOfInputs();
        unsigned size1=driver.getSize();
        unsigned size2=driven.getSize();
        auto ID=[size2](unsigned state1, unsigned state2){
            return size2*state1 + state2;
        };
//        std::vector<bool> added(size1*size2);
//        std::vector<bool> explored(size1*size2);
//        std::vector<unsigned> state_map;
//        std::deque<unsigned> unexplored;
        std::unordered_map<unsigned,unsigned> state_map;
        std::unordered_set<unsigned> unexplored(driven.getSize()*driver.getSize());
        unsigned max_state=0;
        unexplored.insert(0);
        state_map[ID(0,0)]=0;
        obs_fsm.addStates(1);
        while(!unexplored.empty()){
            unsigned pair=*unexplored.begin();

            unsigned state12=state_map[pair];

            unsigned state1=pair/size2;
            unsigned state2=pair%size2;

            unsigned unexplored_succs=0;

            for (unsigned i = 0; i < inputs ; ++i) {
                unsigned o = driver.out(state1, i);
                unsigned t = driven.out(state2, o);
                unsigned next1 = driver.succ(state1, i);
                unsigned next2 = driven.succ(state2, o);
                unsigned next12;
                unsigned next_pair = ID(next1, next2);

                if (!state_map.count(next_pair)) {
                    obs_fsm.addStates();
                    state_map[next_pair] = ++max_state;
                    next12 = max_state;
                    unexplored.insert(next_pair);
                } else next12 = state_map[next_pair];


                if (!obs_fsm.hasTransition(state_map[pair], o)) {
                    obs_fsm.setTransition(state12, o, t);
                }
                if (!obs_fsm.hasSources(next12, o, t) ||
                    obs_fsm.sources(next12, o, t).back() != state12) {
                    obs_fsm.addSucc(state12, o, next12);
                }
            }

            unexplored.erase(pair);
            obs_fsm.addStates(unexplored_succs);

        }

        ofsm_init=true;
        return;
    }



    void DrivenFSMMinimizer::buildCMatrix() {
        if(!ofsm_init){
            std::cout<<" Observation FSM not built yet. \n";
            return;
        }
        const unsigned& size=obs_fsm.getSize();
        const unsigned& inputs=obs_fsm.numberOfInputs();
        compat_matrix.clear();
        compat_matrix.resize((size*(size+3))/2);
        std::deque<std::pair<unsigned ,unsigned>> modified;
        for(unsigned state1 = 0; state1 < size;  ++state1){
        for(unsigned state2 = 0; state2 <= state1 ; ++state2){
            size_t current_pair=toCMatrixID(state1,state2);
            if(compat_matrix[current_pair]) continue;
            for(unsigned i = 0; i<=inputs; ++i){
                if(obs_fsm.hasTransition(state1,i) &&
                        obs_fsm.hasTransition(state2,i) &&
                        obs_fsm.out(state1,i)!=obs_fsm.out(state2,i) ) {
                    compat_matrix[current_pair]= true;
                    modified.push_back({state1,state2});

                }

                while(!modified.empty()){

                    auto mstate1=modified.front().first;
                    auto mstate2=modified.front().second;
                    modified.pop_front();
                    auto mpair=toCMatrixID(mstate1,mstate2);

                    if(compat_matrix[mpair]) continue;



                }


            }






        }
        }

    }



}


