#include <iostream>
#include<vector>
#include<list>
#include<memory>
#include<chrono>
#include<optional>
//#include "fsmclass.h"




class A{
public:
   long long a;
    A(long long _a):a(_a){
    }
    ~A(){
        std::cout<<"..bye";
    }

};


int main() {
    auto time0=std::chrono::high_resolution_clock::now();
    std::list<int> list1={};
    std::vector<int> vect={};
     vect.reserve(100000);
    std::cout<<vect.capacity()<<"\n";
    for (int i = 0; i < 100000; ++i) {
        list1.push_back(i);
//        vect.push_back(i);
    }
   vect.insert(vect.end(),list1.begin(),list1.end());


    auto time1=std::chrono::high_resolution_clock::now();
    auto delay=std::chrono::duration_cast<std::chrono::microseconds>(time1-time0);
    std::cout<<delay.count();
//    opt=std::make_unique<A>(2);
//    opt=std::make_unique<A>(3);
//    std::cout<<(opt.value()->a);







    return 0;
}
