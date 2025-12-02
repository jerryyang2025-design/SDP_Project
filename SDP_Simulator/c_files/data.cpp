#include <fstream>
#include <string>
#include "header_files/data.h"
#include "header_files/utils.h"

void Files::loadFile(char fileName[30]) {
    std::ifstream file;

    file.open(fileName);

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> testline = split(line,' ');
        if (testline[0] == "v") {

        }
        if (testline[0] == "f") {
            
        }
    }

    file.close();
}

void Files::loadStage() {
    
}