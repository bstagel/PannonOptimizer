#include <globals.h>

#include <string>
#include <iostream>
#include <fstream>

#include <dirent.h>

#include <utils/numerical.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexparameterhandler.h>

static const std::string solutionsFilename = "solutions.txt";
static const double solutionTolerance = 1e-6;

struct ParameterValue {
    std::string name;
    std::string value;
    std::string type;
};

static bool error = false;

void fillSolutionMap(std::map<std::string, double> solutionMap) {
    std::string line;
    std::string delimiter = "\t";

    std::ifstream solutionFile;
    solutionFile.open(solutionsFilename, std::ios::in);
    if (solutionFile.is_open()) {
        while ( getline (solutionFile,line) ) {
            std::cout << "Line read: " << line << "\n";

            size_t pos = 0;
            std::string problem;
            double solution;
            pos = line.find(delimiter);
            problem = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
            pos = line.find(delimiter);
            solution = atof(line.substr(0, pos).c_str());
            solutionMap[problem] = solution;
        }
        solutionFile.close();
    } else {
        std::cout << "Unable to open file";
    }
    std::cout<<"map: ";
    for(std::map<std::string, double>::const_iterator it = solutionMap.begin();
        it != solutionMap.end(); ++it)
    {
        std::cout << it->first << " = " << it->second<< "\n";
    }

}

void printHelp() {
    std::cout << "Usage: NewPanOptParameterGenerator [OPTION] \n" <<
                 "Generate parameter file for the NewPanOpt solver. \n"<<
                 "  -i, --input     \t The input file. \n"<<
                 "\n"<<
                 "  -i, --input     \t The input file. \n"<<
                 "  -r, --range     \t Specifies a parameter range NAME.\n"<<
                 "  -p, --parameter \t Specifies a parameter value NAME TYPE VALUE.\n"<<
                 "  -h, --help      \t Displays this help.\n"<<
                 "\n";
}


void printInvalidOptionError(char** argv, int index) {
    std::cout << argv[0]<< ": invalid option: `" << argv[index]<<"`\n"<<
                 "Try `"<< argv[0] <<" --help` for more information.\n";
    error = true;
}

void printInvalidOperandError(char** argv, int opindex, int argindex) {
    std::cout << argv[0]<< ": invalid operand: `" << argv[argindex]<<"` for option: `"<< argv[opindex] <<"`\n"<<
                 "Try `"<< argv[0] <<" --help` for more information.\n";
    error = true;
}

void printMissingOperandError(char** argv) {
    std::cout << argv[0]<< ": missing operand \n"<<
                 "Try `"<< argv[0] <<" --help` for more information.\n";
    error = true;
}

int main(int argc, char** argv) {
    setbuf(stdout, 0);


    std::vector<ParameterValue> values;
    std::vector<std::string> ranges;

    std::string filename;

    std::map<std::string, double> solutionMap;

    if(argc < 2){
        printHelp();
    } else if(argc > 1){
        for(int i=1; i<argc; i++){
            std::string arg(argv[i]);
//            std::cout << "arg: "<< arg << "\n";
            if(arg.compare("-h") == 0 || arg.compare("--help") == 0){
                printHelp();
            } else if(arg.compare("-i") == 0 || arg.compare("--input") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    filename = argv[i+1];
                    i++;
                }
            } else if(arg.compare("-p") == 0 || arg.compare("--parameter") == 0){
                if(argc < i+4 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    ParameterValue value;
                    value.name = argv[i+1];
                    value.type = argv[i+2];
                    value.value = argv[i+3];
                    values.push_back(value);
                    i+=3;
                }
            } else if(arg.compare("-r") == 0 || arg.compare("--range") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    ranges.push_back(argv[i+1]);
                    i++;
                }
            } else {
                printInvalidOptionError(argv, i);
                break;
            }
        }
    }

    if(error){
        return EXIT_FAILURE;
    }

    fillSolutionMap(solutionMap);


    return EXIT_SUCCESS;
}

