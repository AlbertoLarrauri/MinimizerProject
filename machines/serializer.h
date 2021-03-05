//
// Created by llarrauriborroto on 05/03/2021.
//

#ifndef PMIN_SERIALIZER_H
#define PMIN_SERIALIZER_H

#include<filesystem>

namespace SBCMin{

    void serializeDFSM(DFSM& dfsm, int ID=0, std::filesystem::path dir="./benchmarks/");
    DFSM deserializeDFSM(std::filesystem::path file_dir);









}


#endif //PMIN_SERIALIZER_H
