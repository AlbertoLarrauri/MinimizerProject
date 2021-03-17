//
// Created by lazhares on 18/02/2021.
//

#ifndef PMIN_BUILDERS_H
#define PMIN_BUILDERS_H


#include <memory>
#include <unordered_set>
#include <random>
#include "machines.h"

namespace SBCMin {

    OFA buildOFA(const DFSM &driver, const DFSM &driven);

    OFA buildHeuristicOFA(const DFSM &driver, const DFSM &driven);

    DFSM buildCascadeDFSM(const DFSM &driver, const DFSM &driven);

    int getStateCoverage(const DFSM &driver, const DFSM &driven);

    DFSM makeRandomDFSM(int in_size, int out_size, int size, int padding=0);

    class ModularDFSMBuilder{
    private:
        int in_size=0;
        int out_size=0;
        int size=0;
        int number_of_bags=1;
        double extra_edge_probability=0;
        int padding=1;

    public:

        inline ModularDFSMBuilder(int _in_size, int _out_size, int _size):
        in_size(_in_size),
        out_size(_out_size),
        size(_size){}

        inline void setNumberOfInputs(int _number){
            in_size=_number;
        }

        inline void setNumberOfOutputs(int _number){
            out_size=_number;
        }
        void setSize(int _number){
            size=_number;
        }
        void setNumberOfBags(int _number){
            number_of_bags=_number;
        }

        void setPadding(int _number){
            padding=_number;
        }
        void setEdgeProbability(float _number){
            extra_edge_probability=_number;
        }

        DFSM getDFSM();

    };

}


#endif //PMIN_BUILDERS_H
