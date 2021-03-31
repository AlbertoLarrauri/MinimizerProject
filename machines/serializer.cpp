//
// Created by llarrauriborroto on 05/03/2021.
//

#include <fstream>
#include <regex>
#include "machines.h"
#include "serializer.h"

using namespace SBCMin;


void SBCMin::serializeDFSM(const DFSM &dfsm, std::string name, std::filesystem::path dir) {
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directory(dir);
    }
    name = name + ".txt";
    std::filesystem::path file_dir = dir / name;
    std::ofstream ostream(file_dir, std::ios_base::trunc);
    serializeDFSM(dfsm, ostream);
    ostream.close();
}


void SBCMin::serializeDFSM(const DFSM &dfsm, std::ostream &ostream) {
    ostream << "i. " << dfsm.numberOfInputs() << "\n";
    ostream << "o. " << dfsm.numberOfOutputs() << "\n";
    ostream << "n. " << dfsm.size() << "\n";

    for (int state = 0; state < dfsm.size(); ++state) {
        for (int i = 0; i < dfsm.numberOfInputs(); ++i) {
            ostream << state << " / " << i << " --> " << dfsm.getOut(state, i)
                    << " / " << dfsm.getSucc(state, i) << "\n";
        }
    }


}


DFSM SBCMin::deserializeDFSM(const std::filesystem::path &file_dir) {
    if (!file_dir.has_filename()) "No filename provided \n";
    std::ifstream istream(file_dir, std::ios::out);
    if (!istream) throw "Unable to open file \n";
    std::string str = std::string(std::istreambuf_iterator<char>(istream), std::istreambuf_iterator<char>());
    return deserializeDFSM(str);


}

DFSM SBCMin::deserializeDFSM(const std::string &str) {
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
    SBCMin::DFSM A(inputs, outputs);
    A.addStates(size);
    {
        std::regex regex(R"((\d+)\s*/\s*(\d+)\s*-->\s*(\d+)\s*/\s*(\d+))");
        std::sregex_iterator rit(str.begin(), str.end(), regex);
        std::sregex_iterator rend;
        while (rit != rend) {
            std::smatch match = *rit;
//            std::cout << match.str() << "\n";
            int state = std::stoi(match[1]);
            int in = std::stoi(match[2]);
            int out = std::stoi(match[3]);
            int succ = std::stoi(match[4]);
            A.setTrans(state, in, out, succ);
            ++rit;
        }
    }
    return A;
}

std::pair<DFSM, DFSM> SBCMin::deserializeCascade(const std::filesystem::path &file_dir) {
    if (!file_dir.has_filename()) "No filename provided \n";
    std::ifstream istream(file_dir, std::ios::out);
    if (!istream) throw "Unable to open file \n";
    std::string str = std::string(std::istreambuf_iterator<char>(istream), std::istreambuf_iterator<char>());
    DFSM driver(0, 0);
    {
        std::ostringstream driver_ostream;
        std::regex driver_regex(R"(Driver\:([\S\s]*)Driven\:)");
        std::smatch match;
        assert(std::regex_search(str, match, driver_regex));
        std::string driver_str = match[1];
        driver = deserializeDFSM(driver_str);
    }

    DFSM driven(0, 0);
    {
        std::ostringstream driven_ostream;
        std::regex driven_regex(R"(Driven:([\S\s]*))");
        std::smatch match;
        assert(std::regex_search(str, match, driven_regex));
        std::string driven_str = match[1];
        driven = deserializeDFSM(driven_str);
    }

    return std::make_pair(driver, driven);
}

void SBCMin::serializeCascade(const DFSM &driver, const DFSM &driven, std::string name, std::filesystem::path dir) {
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directory(dir);
    }
    name = name + ".txt";
    std::filesystem::path file_dir = dir / name;
    std::ofstream ostream(file_dir, std::ios_base::trunc);
    ostream << "Driver: \n";
    serializeDFSM(driver, ostream);
    ostream << "Driven: \n";
    serializeDFSM(driven, ostream);
    ostream.close();
}







