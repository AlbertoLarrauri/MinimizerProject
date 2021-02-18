//
// Created by llarrauriborroto on 13/01/2021.
//

#ifndef PMIN_FSMS_H
#define PMIN_FSMS_H

#include<iostream>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cassert>
#include <memory>


namespace machines
{
//static char wrong_size_1[1 + sizeof(std::size_t) - sizeof(std::vector<size_t>::size_type)];
//
//static char wrong_size_2[1 - sizeof(std::size_t) + sizeof(std::vector<size_t>::size_type)];









class FSM{
protected:
    unsigned size;
    unsigned in_alphabet_size;
    unsigned out_alphabet_size;

public:
    void reset(unsigned in_size=0, unsigned out_size=0){
        in_alphabet_size=in_size;
        out_alphabet_size=out_size;
        size=0;
    }
    inline unsigned getSize() const{return size;}
    inline unsigned numberOfInputs() const{return in_alphabet_size;}
    inline unsigned numberOfOutputs() const{return out_alphabet_size;}
    FSM(unsigned in_size, unsigned out_size):in_alphabet_size(in_size), out_alphabet_size(out_size),size(0){};

    FSM(FSM&& other){
        in_alphabet_size=other.in_alphabet_size;
        out_alphabet_size=other.out_alphabet_size;
        size=other.size;
        other.reset();
    }
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
void makeRandomDFSM(unsigned size, DFSM& A);


class DFSM: public virtual FSM{
private:
    inline size_t SItoID(const unsigned state, const unsigned in) const{
        return size_t(in_alphabet_size) * size_t(state) + size_t(in);
    }

    typedef std::vector<DTrans> IMPL;

    std::unique_ptr<IMPL> impl;

public:
//TODO: Check for overflows
    inline void addStates(unsigned amount=1){
        size+=amount;
        impl->resize(size*in_alphabet_size);
    }

   inline const unsigned& getOut(unsigned state, unsigned in) const{
       return impl->at(SItoID(state, in)).out;
   }

   inline const unsigned& getSucc(unsigned state, unsigned in)const{
       return impl->at(SItoID(state, in)).next;
   }

    inline void setOut(unsigned state, unsigned in, unsigned  out){
        impl->at(SItoID(state, in)).out=out;
    }

    inline void setSucc(unsigned state, unsigned in, unsigned next){
        impl->at(SItoID(state, in)).next=next;
    }

    void reset(unsigned in_size, unsigned out_size){
        DFSM::reset(in_size,out_size);
        impl=std::make_unique<IMPL>();
    }


    DFSM(unsigned in_size, unsigned out_size): FSM(in_size,out_size){}

    DFSM(DFSM&& other): FSM(std::move(other)){
        impl=std::move(other.impl);
    }

   void print();





};

class OFSM:  public virtual FSM{

private:

    typedef std::vector<std::unordered_map<size_t, std::vector<unsigned>>> SOURCE_IMPL;
    typedef std::vector<std::optional<NTrans>> IMPL;
    std::unique_ptr<IMPL> impl;
    std::unique_ptr<SOURCE_IMPL> source_data;


    inline size_t SItoID(const unsigned state, const unsigned in) const{
        return size_t(in_alphabet_size) * size_t(state) + size_t(in);
    }



    inline size_t IOtoID(const unsigned i, const unsigned o) const{
        return size_t(in_alphabet_size)*size_t(o)+ size_t(i);
    }


public:

    OFSM(unsigned in_size, unsigned out_size): FSM(in_size,out_size){}

    OFSM(OFSM&& other):FSM(std::move(other)){
        impl=std::move(other.impl);
        source_data=std::move(other.source_data);
    }

    void reset(unsigned in_size, unsigned out_size){
        FSM::reset(in_size,out_size);
        impl=std::make_unique<IMPL>();
        source_data=std::make_unique<SOURCE_IMPL>();
    }



    inline bool hasTransition(const unsigned state, const unsigned in){
        return (*impl)[SItoID(state, in)].has_value();
    }



    inline const unsigned& out(unsigned state, unsigned in){
        assert((*impl)[SItoID(state, in)].has_value());
        return (*impl)[SItoID(state, in)]->out;

    }

    void setTransition(unsigned state, unsigned in, unsigned out){
        (*impl)[SItoID(state, in)].emplace();
        (*impl)[SItoID(state, in)]->out=out;
    }

    void addSucc(unsigned state, unsigned in, unsigned succ){
        assert((*impl)[SItoID(state, in)]);
        (*impl)[SItoID(state, in)]->successors.push_back(succ);
        (*source_data)[succ][IOtoID(in,out(state,in))].emplace_back(state);
    }

    inline const std::vector<unsigned>&
        succs(const unsigned state, const unsigned in){
        assert((*impl)[SItoID(state, in)]);
        return (*impl)[SItoID(state, in)]->successors;
    }

    inline void addStates(unsigned amount=1){
        size+=amount;
        (*impl).resize(size*in_alphabet_size);
        (*source_data).resize(size*in_alphabet_size*out_alphabet_size);
    }




    inline bool hasSources(unsigned state, unsigned in, unsigned out){
        return (*source_data)[state].count(IOtoID(in, out));
    }




    inline const std::vector<unsigned>& sources(unsigned state, unsigned in, unsigned out){
        assert(hasSources(state, in, out));
        return (*source_data)[state][IOtoID(in, out)];
    }

    inline const std::unordered_map<size_t, std::vector<unsigned>>& sourceData(unsigned state){
        return (*source_data)[state];
    }


    void print();
};



}





#endif //PMIN_FSMS_H
