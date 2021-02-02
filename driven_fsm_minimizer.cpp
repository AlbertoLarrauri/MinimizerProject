//
// Created by llarrauriborroto on 28/01/2021.
//

#include "driven_fsm_minimizer.h"

using namespace machines;
namespace minimizers{
    OFSM &buildOFSM(DFSM& driver, DFSM& driven){
        assert(driver.numberOfOutputs()==driven.numberOfInputs());
        OFSM obs_machine=*new OFSM(driver.numberOfInputs(),driven.numberOfOutputs());
        if(driver.getSize()==0 || driver.getSize()==0) return obs_machine;
        unsigned inputs=driver.numberOfInputs();
        unsigned size2=driven.getSize();
        auto ID=[size2](unsigned state1, unsigned state2){
            return size2*state1 + state2;
        };
        std::unordered_map<unsigned,unsigned> state_map;
        std::unordered_set<unsigned> unexplored;
        unsigned max_state=0;
        unexplored.insert(0);
        state_map[ID(0,0)]=0;
        obs_machine.addStates(1);

        while(!unexplored.empty()){
            unsigned pair=*unexplored.begin();

            unsigned state12=state_map[pair];

            unsigned state1=pair/size2;
            unsigned state2=pair%size2;

            unsigned unexplored_succs=0;

            for (unsigned i = 0; i < inputs ; ++i) {
                unsigned o=driver.out(state1,i);
                unsigned t=driven.out(state2,o);
                unsigned next1=driver.succ(state1,i);
                unsigned next2=driven.succ(state2,o);
                unsigned next12;

                if(!state_map.count(ID(next1,next2))){
                    unexplored_succs++;
                    state_map[ID(next1,next2)]=++max_state;
                    next12=max_state;
                    unexplored.insert(next12);
                } else next12=state_map[ID(next1,next2)];


                if(!obs_machine.hasTransition(state_map[pair],o)){
                    obs_machine.out(state12,o)=t;
                }
                obs_machine.succs(state12,o).push_back(next12);
            }
            unexplored.erase(pair);
            obs_machine.addStates(unexplored_succs);

        }


        return obs_machine;
    }



}


