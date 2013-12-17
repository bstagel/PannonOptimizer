#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include <dirent.h>

#if defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
#ifndef WIN32
#define WIN32
#endif
#endif

#define LOWER_LOGICAL "2"
#define UPPER_LOGICAL "3"
#define CRASH "4"

enum ALGORITHM {
    PRIMAL, //Choice 1 in Maros' application
    DUAL    //Choice 2 in Maros' application
};

const std::string STARTING_BASIS = LOWER_LOGICAL;

const std::string DEFAULT_OUTPUT_DIR = "output\\";
const std::string DEFAULT_BINARY = "m503d.exe";

bool fileExists(std::string filename) {
  std::ifstream ifile(filename);
  return ifile;
}


bool dirExists(std::string dirname) {
    DIR *dir;
    if ((dir = opendir (dirname.c_str())) != NULL) {
        closedir (dir);
        return true;
    } else {
        return false;
    }
}

void printHelp() {
    std::cout << "Usage: MarosTester [OPTION] \n" <<
                 "Solve [FILE] with the dual simplex method. \n"<<
                 "\n"<<
                 "   -a, --algorithm \t Specifies the solution algorithm (`primal` or `dual`(default)) .\n"<<
                 "   -b, --binary    \t Specifies the binary application path (`m503d.exe`(default)) .\n"<<
                 "  -fl, --file-list \t Solve all the MPS files listed in text file.\n"<<
                 "   -p, --path      \t Specifies a path to the mps files.(`.\\`(default))\n"<<
                 "   -o, --output-dir\t Specifies the output directiry (`output\\`(default))\n"<<
                 "   -h, --help      \t Displays this help.\n"<<
                 "\n";
}

void printInvalidOptionError(char** argv, int index) {
    std::cout << argv[0]<< ": invalid option: `" << argv[index]<<"`\n"<<
                 "Try `"<< argv[0] <<" --help` for more information.\n";
}

void printInvalidOperandError(char** argv, int opindex, int argindex) {
    std::cout << argv[0]<< ": invalid operand: `" << argv[argindex]<<"` for option: `"<< argv[opindex] <<"`\n"<<
                 "Try `"<< argv[0] <<" --help` for more information.\n";
}

void printMissingOperandError(char** argv) {
    std::cout << argv[0]<< ": missing operand \n"<<
                 "Try `"<< argv[0] <<" --help` for more information.\n";
}

int main (int argc, char** argv) {

    std::cerr << "\nWelcome to MarosTester!\n\n";
    std::cerr << "This program can test many mps files automatically one after another using Maros' Fortran program.";
    std::cerr << "\nSupported platforms: Windows\n";
    std::cerr << "Each test case will generate an output file *.MPS_P_Result.txt for the primal.\n"
                 "Each test case will generate an output file *.MPS_D_Result.txt for the dual.\n\n";

#ifdef WIN32
    if (argc < 2) {
        printHelp();
    } else if(argc > 1){
        std::string program = DEFAULT_BINARY;
        std::string fileListPath = "";
        std::string path = ".\\";
        std::string outputDir = DEFAULT_OUTPUT_DIR;
        ALGORITHM algorithm = DUAL;

        for(int i=1; i<argc; i++){
            std::string arg(argv[i]);
            if(arg.compare("-h") == 0 || arg.compare("--help") == 0){
                printHelp();
            } else if(arg.compare("-a") == 0 || arg.compare("--algorithm") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    std::string alg(argv[i+1]);
                    if(alg.compare("primal") == 0){
                        algorithm = PRIMAL;
                    } else if(alg.compare("dual") == 0){
                        algorithm = DUAL;
                    } else {
                        std::cout << "Unknown algorithm, please use `primal` or `dual` (default).\n";
                    }
                    i++;
                }
            } else if(arg.compare("-p") == 0 || arg.compare("--path") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    path = argv[i+1];
                    i++;
                }
            } else if(arg.compare("-fl") == 0 || arg.compare("--file-list") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    fileListPath = argv[i+1];
                    i++;
                }
            } else if(arg.compare("-o") == 0 || arg.compare("--output-dir") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    outputDir = argv[i+1];
                    i++;
                }
            } else {
                printInvalidOptionError(argv, i);
                break;
            }
        }

        std::ifstream fileNames (fileListPath.data(), std::ifstream::in);
        std::vector<std::string> files;

        while (fileNames.good()) {
          std::string temp;
          fileNames >> temp;
          files.push_back(temp);
        }
        fileNames.close();

        for (unsigned int i=0; i < files.size(); i++) {
            if(!dirExists(outputDir)){
                mkdir(outputDir.c_str());
            }
            std::string command = "";
            std::string outfile = "";
            if(algorithm == PRIMAL){
                outfile = outputDir + files.at(i) + "_P_" + "Result.txt";
            } else if(algorithm == DUAL){
                outfile = outputDir + files.at(i) + "_D_" + "Result.txt";
            }
            if(fileExists(outfile)){
                command = "del " + outfile;
                system(command.data());
            }
            std::string infile =  files.at(i) + "Test.txt";
            if(fileExists(infile)){
                command = "del " + infile;
                system(command.data());
            }
            if(fileExists(path + files.at(i))){
                std::cout << "Solving: "<<path + files.at(i)<<std::endl;
                std::ofstream test(infile.data(), std::ofstream::out);
                test << path + files.at(i) << std::endl;
                test << STARTING_BASIS << std::endl;
                if(algorithm == PRIMAL){
                    test << "1" <<std::endl;
                } else if (algorithm == DUAL){
                    test << "2" <<std::endl;
                }
                test << "4" << std::endl  //RUN
                     << "n" << std::endl  //No detailed solution output
                     << "N" << std::endl  //No solution dump
                     << "3" << std::endl; //END
                test.close();
                command = program + " < " + infile + " >> " + outfile;
                system(command.data());
                command = "del " + infile;
                system(command.data());
            } else {
                std::cout << "!!! Input file not found: "<<path + files.at(i)<<std::endl;
            }
        }
    }

#else
    cerr << "Sorry, but your platform currently is not supported.\n";
#endif

    return 0;
}
