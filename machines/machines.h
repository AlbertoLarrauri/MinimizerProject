//
// Created by llarrauriborroto on 13/01/2021.
//

#ifndef PMIN_MACHINES_H
#define PMIN_MACHINES_H

#include<iostream>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cassert>
#include <memory>


namespace SBCMin
{
//static char wrong_size_1[1 + sizeof(std::size_t) - sizeof(std::vector<size_t>::size_type)];
//
//static char wrong_size_2[1 - sizeof(std::size_t) + sizeof(std::vector<size_t>::size_type)];






    class FSM{
    protected:
        int size;
        int in_alphabet_size;
        int out_alphabet_size;

        //CONSTRUCTORS

        FSM(int in_size, int out_size):in_alphabet_size(in_size), out_alphabet_size(out_size),size(0){};


        virtual void reset(int in_size=0, int out_size=0){
            in_alphabet_size=in_size;
            out_alphabet_size=out_size;
            size=0;
        }

    public:
        inline int getSize() const{return size;}
        inline int numberOfInputs() const{return in_alphabet_size;}
        inline int numberOfOutputs() const{return out_alphabet_size;}



    };

    class OFA;
    class DFSM;






    DFSM& cascadeDFSM(const DFSM& A, const DFSM& B);
    void makeRandomDFSM(int size, DFSM& A);


    class DFSM: public virtual FSM{
    private:

        class DTrans{
        public:
            int out;
            int next;
        };

        // Transition data:

        typedef std::vector<DTrans> IMPL;

        IMPL impl;

        // Helper function to get transition index;

        inline size_t SItoID(const int state, const int in) const{
            return size_t(in_alphabet_size) * size_t(state) + size_t(in);
        }



    public:

        DFSM(int in_size, int out_size): FSM(in_size,out_size){
        }

        DFSM(const DFSM& other)=default;
        DFSM& operator=(const DFSM& other)=default;

        DFSM(DFSM&& other) noexcept:
        //Fist copies FSM parameters: size, input alphabet size, and output alphabet size
                FSM(other),
                impl(std::move(other.impl))
        {
            other.reset();
        }

        DFSM& operator=(DFSM&& other){
            FSM::operator=(other);
            impl=std::move(other.impl);
            other.reset();
            return *this;
        }

        void reset(int in_size=0, int out_size=0) override{
            FSM::reset(in_size,out_size);
            impl.clear();
        }



        inline void addStates(int amount=1){
            size+=amount;
            impl.resize(size*in_alphabet_size);
        }


        inline const int& getOut(int state, int in) const{
            return impl.at(SItoID(state, in)).out;
        }

        inline const int& getSucc(int state, int in)const{
            return impl.at(SItoID(state, in)).next;
        }

        inline void setOut(int state, int in, int  out){
            impl.at(SItoID(state, in)).out=out;
        }

        inline void setSucc(int state, int in, int next){
            impl.at(SItoID(state, in)).next=next;
        }


        void print();

    };

    class OFA: public virtual FSM{

    private:

        class NTrans{
        public:
            int out;
            std::vector<int> successors;

        };


        // Transition data. We keep track of sources and targets of each transition.

        typedef std::vector<std::unordered_map<size_t, std::vector<int>>> SOURCE_IMPL;
        typedef std::vector<std::optional<NTrans>> IMPL;
        IMPL impl;
        SOURCE_IMPL source_data;

        // Helper functions to get indices in the data structures

        inline size_t SItoID(const int state, const int in) const{
            return size_t(in_alphabet_size) * size_t(state) + size_t(in);
        }



        inline size_t IOtoID(const int i, const int o) const{
            return size_t(in_alphabet_size)*size_t(o)+ size_t(i);
        }


    public:

        OFA(int in_size, int out_size): FSM(in_size, out_size){
        }

        OFA(const OFA& other)=default;

        OFA& operator=(const OFA& other)=default;

        OFA(OFA&& other):
                FSM(other),
                impl(std::move(other.impl)),
                source_data(std::move(other.source_data))
        {
            other.reset();
        }

        OFA& operator=(OFA&& other){
            FSM::operator=(other);
            impl=std::move(other.impl);
            source_data=std::move(other.source_data);
            other.reset();
            return *this;
        }



        void reset(int in_size=0, int out_size=0){
            FSM::reset(in_size,out_size);
            impl.clear();
            source_data.clear();
        }



        inline bool hasTransition(const int state, const int in) const {
            return impl[SItoID(state, in)].has_value();
        }



        inline const int & out(int state, int in) const {
            assert(impl[SItoID(state, in)].has_value());
            return impl[SItoID(state, in)]->out;

        }

        void setTransition(int state, int in, int out){
            impl[SItoID(state, in)].emplace();
            impl[SItoID(state, in)]->out=out;
        }

        void addSucc(int state, int in, int succ){
            assert(impl[SItoID(state, in)]);
            impl[SItoID(state, in)]->successors.push_back(succ);
            source_data[succ][IOtoID(in,out(state,in))].emplace_back(state);
        }

        inline const std::vector<int> &
        succs(const int state, const int in) const {
            assert(impl[SItoID(state, in)]);
            return (impl[SItoID(state, in)]->successors);
        }

        inline void addStates(int amount=1){
            size+=amount;
            impl.resize(size*in_alphabet_size);
            source_data.resize(size*in_alphabet_size*out_alphabet_size);
        }

        inline bool hasSources(int state, int in, int out){
            return source_data[state].count(IOtoID(in, out));
        }




        inline const std::vector<int>& sources(int state, int in, int out){
            assert(hasSources(state, in, out));
            return source_data[state][IOtoID(in, out)];
        }

        inline const std::unordered_map<size_t, std::vector<int>>& sourceData(int state){
            return source_data[state];
        }


        void print();
    };



}





#endif //PMIN_MACHINES_H
