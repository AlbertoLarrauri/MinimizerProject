//
// Created by llarrauriborroto on 05/03/2021.
//

#include <fstream>
#include <regex>
#include "machines.h"
#include "serializer.h"

using namespace SBCMin;

void SBCMin::serializeDFSM(DFSM &dfsm, int ID, std::filesystem::path dir) {
    if(!std::filesystem::exists(dir)){
        std::filesystem::create_directory(dir);
    }
    std::string name = "dfsm" + std::to_string(ID) + ".txt";
    std::filesystem::path file_dir = dir / name;
    std::cout<<file_dir<<"\n";


    std::ofstream file(file_dir, std::ios::trunc);
    if(!file){
        std::cout<<"No file!!";
    }

    file<<"i. "<<dfsm.numberOfInputs()<<"\n";
    file<<"o. "<<dfsm.numberOfOutputs()<<"\n";
    file<<"n. "<<dfsm.getSize()<<"\n";

    for (int state = 0; state < dfsm.getSize(); ++state) {
        for (int i = 0; i < dfsm.numberOfInputs(); ++i) {
            file << state << " / " << i << " --> " << dfsm.getOut(state, i)
                    << " / " << dfsm.getSucc(state, i) << "\n";
        }
    }

    file.close();
}

DFSM SBCMin::deserializeDFSM(std::filesystem::path file_dir) {
    assert(file_dir.has_filename());
    std::cout << file_dir.extension() << "\n";
    std::ifstream file(file_dir, std::ios::out);
    assert(file);

    std::string str = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    int inputs;
    {
        std::regex regex(R"(i\.\s*(\d+))");
        std::smatch match;
        assert(std::regex_search(str, match, regex));
        inputs = std::stoi(match[1]);
    }
    int outputs;
    {
        std::regex regex(R"(o\.\s*(\d+))");
        std::smatch match;
        assert(std::regex_search(str, match, regex));
        outputs = std::stoi(match[1]);

    }
    int size;
    {
        std::regex regex(R"(n\.\s*(\d+))");
        std::smatch match;
        assert(std::regex_search(str, match, regex));
        size = std::stoi(match[1]);
    }
    SBCMin::DFSM A(inputs,outputs);
    A.addStates(size);
    {
        std::regex regex(R"((\d+)\s*/\s*(\d+)\s*-->\s*(\d+)\s*/\s*(\d+))");
        std::sregex_iterator rit ( str.begin(), str.end(), regex );
        std::sregex_iterator rend;
        while(rit!=rend){
            std::smatch match=*rit;
            std::cout<<match.str()<<"\n";
            int state=std::stoi(match[1]);
            int in=std::stoi(match[2]);
            int out=std::stoi(match[3]);
            int succ=std::stoi(match[4]);
            A.setTrans(state, in, out, succ);
            ++rit;
        }
    }

    return A;

}