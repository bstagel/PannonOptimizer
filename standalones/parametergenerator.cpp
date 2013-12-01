#include <globals.h>

#include <string>
#include <iostream>
#include <fstream>

#include <utils/numerical.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexparameterhandler.h>

static bool error = false;
static int fileIndex = 0;
static std::string fileName = "";

struct ParameterRange {
    std::string name;
    Numerical::Double from;
    Numerical::Double to;

    Numerical::Double steplength;

    Numerical::Double getStep(int step) const {
        return from + step*steplength;
    }
};

bool checkParameters(std::vector<ParameterRange> & ranges, const ParameterHandler & handler) {
    for(unsigned int i=0; i < ranges.size(); i++){
        if(! handler.hasParameter(ranges.at(i).name)){
            return false;
        }
    }
    return true;
}

void initStepsEqually(std::vector<ParameterRange> & ranges, int number) {
    for(unsigned int i=0; i< ranges.size(); i++){
        ranges.at(i).steplength = (ranges.at(i).to - ranges.at(i).from) / (number-1);
    }
}

void computeParameter(std::vector<ParameterRange> & ranges, unsigned int parameterIndex, int number, ParameterHandler & handler ){
    if(parameterIndex<ranges.size()){
        for(int i=0; i<number; i++){
            handler.setParameterValue(ranges.at(parameterIndex).name, ranges.at(parameterIndex).getStep(i));
            std::cout<<ranges.at(parameterIndex).from<< '\n';
            std::cout<<ranges.at(parameterIndex).getStep(i)<< '\n';
            std::cout<<ranges.at(parameterIndex).to<< '\n';
            computeParameter(ranges, parameterIndex+1, number, handler);
        }
    } else {
        std::stringstream newName;
        newName << fileName << "_" << fileIndex;
        std::cout <<newName.str();
        handler.setFileName(newName.str());
        handler.writeParameterFile();
        fileIndex++;
    }
}

void generateParameters(std::vector<ParameterRange> & ranges, int number, ParameterHandler & handler ) {
    if(checkParameters(ranges, handler)){
        initStepsEqually(ranges, number);
        fileIndex = 0;
        computeParameter(ranges, 0,number, handler);
    } else {
        std::cout << "Invalid parameter name specified!\n";
    }
}

void printHelp() {
    std::cout << "Usage: NewPanOptParameterGenerator [OPTION] \n" <<
                 "Generate parameter file for the NewPanOpt solver. \n"<<
                 "\n"<<
                 "  -f, --file      \t Two valid parameter values for the argument are `linalg.PAR` and `simplex.PAR`. \n"<<
                 "  -r, --range     \t Specifies a parameter range with a NAME FROM TO triplet.\n"<<
                 "  -n, --number    \t The number of partitions in each range specified.\n"<<
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

    LinalgParameterHandler::getInstance();
    SimplexParameterHandler::getInstance();

    std::vector<ParameterRange> ranges;
    int number = 0;

    if(argc < 2){
        printHelp();
    } else if(argc > 1){
        for(int i=1; i<argc; i++){
            std::string arg(argv[i]);
//            std::cout << "arg: "<< arg << "\n";
            if(arg.compare("-h") == 0 || arg.compare("--help") == 0){
                printHelp();
            } else if(arg.compare("-f") == 0 || arg.compare("--file") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    fileName = argv[i+1];
                    i++;
                }
            } else if(arg.compare("-n") == 0 || arg.compare("--number") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    number = atoi(argv[i+1]);
                    i++;
                }
            } else if(arg.compare("-r") == 0 || arg.compare("--directory") == 0){
                if(argc < i+4 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    ParameterRange range;
                    range.name = argv[i+1];
                    range.from = atof(argv[i+2]);
                    range.to = atof(argv[i+3]);
                    ranges.push_back(range);
                    i+=3;
                }
            } else {
                printInvalidOptionError(argv, i);
                break;
            }
        }
    }

    if(error || number == 0){
        return EXIT_FAILURE;
    }

    if(fileName.compare("linalg.PAR") == 0){
        ParameterHandler & handler = LinalgParameterHandler::getInstance();
        generateParameters(ranges, number, handler);
    } else if(fileName.compare("simplex.PAR") == 0){
        ParameterHandler & handler = LinalgParameterHandler::getInstance();
        generateParameters(ranges, number, handler);
    } else if(fileName.compare("") == 0){
        std::cout << "Please specify the name of the parameter file. Use `--help` for further information!\n";
        return EXIT_FAILURE;
    } else {
        std::cout << "Invalid parameter file name. Use `--help` for further information!\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

