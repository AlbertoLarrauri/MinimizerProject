//
// Created by llarrauriborroto on 13/01/2021.
//

#include "fsms.h"
#include<assert.h>

namespace machines{
    unsigned int OFSM::getOutput(const unsigned int state, const unsigned int in) const {
        return impl[SItoIDX(state,in)].out;
    }

    const std::vector<unsigned int> * OFSM::peakSuccessors(const unsigned int state, const unsigned int in) const {
        return impl[SItoIDX(state,in)].successors.get();
    }

    void OFSM::addState() {
        impl.resize(SItoIDX(size,in_alphabet_size));
        size++;
    }

    void OFSM::setTransition(unsigned int state, unsigned int in, NTrans& trans) {
        assert(trans.out!=DONT_CARE || trans.successors== nullptr);
        impl[SItoIDX(state,in)]=std::move(trans);
    }



}

