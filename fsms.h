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
#include <cassert>

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

    typedef std::vector<DTrans>::size_type ID;
    inline ID SItoID(const unsigned state, const unsigned in) const{
        return ID(in_alphabet_size) * ID(state) + ID(in);
    }

    std::vector<DTrans> impl;

public:
//TODO: Check for overflows
    inline void addStates(unsigned amount=1){
        size+=amount;
        impl.resize(size*in_alphabet_size);
    }

   inline unsigned& out(unsigned state, unsigned in){
       return impl[SItoID(state, in)].out;
   }

   inline unsigned& succ(unsigned state, unsigned in){
       return impl[SItoID(state, in)].next;
   }

   DFSM(unsigned in_size, unsigned out_size): FSM(in_size,out_size){}

   void print();





};

class OFSM:  public virtual FSM{

private:


    typedef std::vector<std::optional<NTrans>>::size_type ID;
    typedef std::vector<std::optional<std::vector<unsigned>>>::size_type ID2;
    std::vector<std::optional<NTrans>> impl;
    std::vector<std::optional<std::vector<unsigned>>> source_data;

    inline ID SItoID(const unsigned state, const unsigned in) const{
        return ID(in_alphabet_size) * ID(state) + ID(in);
    }


    inline ID2 SIOtoID(const unsigned state, const unsigned in, const unsigned out) const{
        return ID2(out_alphabet_size)*
            (ID2(in_alphabet_size)*ID2(state) + ID2(in))
                + ID2(out);
    }

//    inline size_t IOtoSIZE_T(const unsigned state, const unsigned in) const{
//        return size_t(in_alphabet_size)*size_t(state)+ size_t(in);
//    }


public:

    inline bool hasTransition(const unsigned state, const unsigned in){
        return impl[SItoID(state, in)].has_value();
    }



    inline const unsigned& out(unsigned state, unsigned in){
        assert(impl[SItoID(state, in)].has_value());
        return impl[SItoID(state, in)]->out;

    }

    void setTransition(unsigned state, unsigned in, unsigned out){
        impl[SItoID(state, in)].emplace();
        impl[SItoID(state, in)]->out=out;
    }

    void addSucc(unsigned state, unsigned in, unsigned succ){
        assert(impl[SItoID(state, in)]);
        impl[SItoID(state, in)]->successors.push_back(succ);

        auto& sources=source_data[SIOtoID(succ,in,out(state,in))];
        if(!sources) sources.emplace();
        sources->push_back(state);
    }

    inline const std::vector<unsigned>&
        succs(const unsigned state, const unsigned in){
        assert(impl[SItoID(state, in)]);
        return impl[SItoID(state, in)]->successors;
    }

    inline void addStates(unsigned amount=1){
        size+=amount;
        impl.resize(size*in_alphabet_size);
        source_data.resize(size*in_alphabet_size*out_alphabet_size);
    }

    OFSM(unsigned in_size, unsigned out_size): FSM(in_size,out_size){}


    inline bool hasSources(unsigned state, unsigned in, unsigned out){
        return source_data[SIOtoID(state, in, out)].has_value();
    }




    inline const std::vector<unsigned>& sources(unsigned state, unsigned in, unsigned out){
        assert(source_data[SIOtoID(state, in, out)]);
        return *source_data[SIOtoID(state, in, out)];
    }


    void print();
};



}





#endif //PMIN_FSMS_H
