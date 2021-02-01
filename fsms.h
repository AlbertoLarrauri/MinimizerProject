//
// Created by llarrauriborroto on 13/01/2021.
//

#ifndef PMIN_FSMS_H
#define PMIN_FSMS_H



#include<vector>
#include<unordered_set>
#include<unordered_map>
#include<climits>
#include<memory>

namespace machines
{
//static char wrong_size_1[1 + sizeof(std::size_t) - sizeof(std::vector<size_t>::size_type)];
//
//static char wrong_size_2[1 - sizeof(std::size_t) + sizeof(std::vector<size_t>::size_type)];









class FSM{
protected:
    unsigned size;
    const unsigned in_alphabet_size;
    const unsigned out_alphabet_size;

public:
    unsigned getSize() const{return size;}
    unsigned numberOfInputs() const{return in_alphabet_size;}
    unsigned numberOfOutputs() const{return out_alphabet_size;}

};

class OFSM;
class NTrans;
class DTrans;
class DFSM;



class DTrans{
    public:
        unsigned out;
        unsigned next;
};

class NTrans{
        public:
        unsigned out;
        std::unique_ptr<std::vector<unsigned>> successors;

        NTrans(NTrans&& trans){
            out=trans.out;
            successors=std::move(trans.successors);
        }

        NTrans& operator=(NTrans&& other)
        {
            out=other.out;
            successors=std::move(other.successors);
            return *this;
        }

    };







DFSM& cascadeDFSM(const DFSM& A, const DFSM& B);

class DFSM: public virtual FSM{

private:

    typedef std::vector<DTrans>::size_type IDX;
    inline IDX SItoIDX(const unsigned state, const unsigned in) const{
        return IDX(in_alphabet_size)*IDX(state)+ IDX(in);
    }

    std::vector<DTrans> impl;

public:

   inline unsigned getOutput(unsigned state, unsigned in)const{
       return impl[SItoIDX(state,in)].out;
   }

   inline unsigned getSuccessor(unsigned state, unsigned in){
       return impl[SItoIDX(state,in)].next;
   }



};

class OFSM:  public virtual FSM{

private:


    typedef std::vector<NTrans>::size_type IDX;
    const unsigned DONT_CARE=out_alphabet_size;
    std::vector<NTrans> impl;


    inline IDX SItoIDX(const unsigned state, const unsigned in) const{
        return IDX(in_alphabet_size)*IDX(state)+ IDX(in);
    }

    inline size_t IOtoSIZE_T(const unsigned state, const unsigned in) const{
        return size_t(in_alphabet_size)*size_t(state)+ size_t(in);
    }


public:

    inline unsigned getOutput(const unsigned state, const unsigned in) const;
    inline const std::vector<unsigned>*
        peakSuccessors(const unsigned state, const unsigned in) const;

    void addState();
    void setTransition(unsigned int state, unsigned int in, NTrans& trans);
};



}





#endif //PMIN_FSMS_H
