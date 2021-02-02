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
#include<optional>

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
    inline unsigned getSize() const{return size;}
    inline unsigned numberOfInputs() const{return in_alphabet_size;}
    inline unsigned numberOfOutputs() const{return out_alphabet_size;}
    FSM(unsigned in_size, unsigned out_size):in_alphabet_size(in_size), out_alphabet_size(out_size),size(0){};
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
        std::vector<unsigned> successors;

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
//TODO: Check for overflows
    inline void addStates(unsigned amount=1){
        size+=amount;
        impl.resize(size*in_alphabet_size);
    }

   inline unsigned& out(unsigned state, unsigned in){
       return impl[SItoIDX(state,in)].out;
   }

   inline unsigned& succ(unsigned state, unsigned in){
       return impl[SItoIDX(state,in)].next;
   }

   DFSM(unsigned in_size, unsigned out_size): FSM(in_size,out_size){}

   void print();





};

class OFSM:  public virtual FSM{

private:


    typedef std::vector<std::optional<NTrans>>::size_type IDX;
    std::vector<std::optional<NTrans>> impl;


    inline IDX SItoIDX(const unsigned state, const unsigned in) const{
        return IDX(in_alphabet_size)*IDX(state)+ IDX(in);
    }

    inline size_t IOtoSIZE_T(const unsigned state, const unsigned in) const{
        return size_t(in_alphabet_size)*size_t(state)+ size_t(in);
    }


public:

    inline bool hasTransition(const unsigned state, const unsigned in){
        return impl[SItoIDX(state,in)].has_value();
    }

    inline unsigned& out(const unsigned state, const unsigned in){
        if(!impl[SItoIDX(state,in)]){
            impl[SItoIDX(state,in)].emplace();
        }
        return impl[SItoIDX(state,in)]->out;

    }

    inline std::vector<unsigned>&
        succs(const unsigned state, const unsigned in){
        if(!impl[SItoIDX(state,in)]){
            impl[SItoIDX(state,in)].emplace();
        }
        return impl[SItoIDX(state,in)]->successors;

    }

    inline void addStates(unsigned amount=1){
        size+=amount;
        impl.resize(size*in_alphabet_size);
    }

    OFSM(unsigned in_size, unsigned out_size): FSM(in_size,out_size){}

    void print();
};



}





#endif //PMIN_FSMS_H
