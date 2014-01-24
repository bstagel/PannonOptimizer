#include <globals.h>

#include <string>
#include <iostream>
#include <fstream>

#include <dirent.h>

#include <utils/numerical.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexparameterhandler.h>

static const std::string endpoint = "'http://mishra.lpds.sztaki.hu:9091'";
static const std::string grid = "'edgidemo'";
static const std::string tag = "'PanOptSolverTag'";
static const std::string exeName = "NewPanOptSolver";
static const std::string scriptName = "panopt_wsclient.sh";
static const std::string filelist = "filelist.txt";
static const std::string exportResult = "export_result.txt";

static bool error = false;
static int fileIndex = 0;
static std::string fileName = "";

struct ParameterValue {
    std::string name;
    std::string value;
    std::string type;
};

struct ParameterRange {
    std::string name;
    std::string from;
    Numerical::Double steplength;
    std::string type;
    int number;

    Numerical::Double getDoubleStep(int step) const {
        if(step == 0){
            if(type.compare("double") == 0){
                return atof(from.c_str());
            } else {
                std::cout << "Invalid parameter Type specified!\n";
                return 0;
            }
        }
        else{
            return steplength*getDoubleStep(step-1);
        }
    }

    int getIntStep(int step) const {
        if(step == 0){
            if(type.compare("int") == 0){
                return atoi(from.c_str());
            } else {
                std::cout << "Invalid parameter Type specified!\n";
                return 0;
            }
        }
        else{
            return steplength*getIntStep(step-1);
        }
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

bool checkParameters(std::vector<ParameterValue> & values, const ParameterHandler & handler) {
    for(unsigned int i=0; i < values.size(); i++){
        if(! handler.hasParameter(values.at(i).name)){
            return false;
        }
    }
    return true;
}

void computeParameter(std::vector<ParameterRange> & ranges, unsigned int rangeIndex, ParameterHandler & handler ){
    if(rangeIndex<ranges.size()){
        for(int i=0; i<ranges.at(rangeIndex).number; i++){
            if(ranges.at(rangeIndex).type.compare("double") == 0){
                handler.setParameterValue(ranges.at(rangeIndex).name, ranges.at(rangeIndex).getDoubleStep(i));
            } else if(ranges.at(rangeIndex).type.compare("int") == 0){
                handler.setParameterValue(ranges.at(rangeIndex).name, ranges.at(rangeIndex).getIntStep(i));
            } else {
                std::cout << "Invalid parameter Type specified!\n";
                break;
            }
//            std::cout<<ranges.at(rangeIndex).from<< '\n';
//            std::cout<<ranges.at(rangeIndex).steplength<< '\n';
//            if(ranges.at(rangeIndex).type.compare("double") == 0){
//                std::cout<<ranges.at(rangeIndex).getDoubleStep(i)<< '\n';
//            } else if(ranges.at(rangeIndex).type.compare("int") == 0){
//                std::cout<<ranges.at(rangeIndex).getIntStep(i)<< '\n';
//            }
            computeParameter(ranges, rangeIndex+1, handler);
        }
    } else {
        std::stringstream newName;
        newName << fileName << "_" << fileIndex;
        std::cout << "Generating " << newName.str()<<"\n";
        handler.setFileName(newName.str());
        handler.writeParameterFile();
        fileIndex++;
    }
}

void generateParameters(std::vector<ParameterRange> & ranges, std::vector<ParameterValue> values, ParameterHandler & handler ) {
    if(checkParameters(ranges, handler)){
        for(unsigned int i=0; i<values.size(); i++){
            if(values.at(i).type.compare("double") == 0){
                handler.setParameterValue(values.at(i).name, atof(values.at(i).value.c_str()));
            } else if(values.at(i).type.compare("int") == 0){
                handler.setParameterValue(values.at(i).name, atoi(values.at(i).value.c_str()));
            } else if(values.at(i).type.compare("bool") == 0) {
                if(values.at(i).value.compare("true") == 0){
                    handler.setParameterValue(values.at(i).name, true);
                } else if(values.at(i).value.compare("false") == 0){
                    handler.setParameterValue(values.at(i).name, false);
                } else {
                    std::cout << "Wrong value for bool parameter!\n";
                    break;
                }
            } else if(values.at(i).type.compare("string") == 0) {
                handler.setParameterValue(values.at(i).name, (values.at(i).value));
            } else {
                std::cout << "Invalid parameter Type specified!\n";
                break;
            }
        }
        fileIndex = 0;
        computeParameter(ranges, 0, handler);
    } else {
        std::cout << "Invalid parameter name specified!\n";
    }
}

void generateGbacScript(std::string remoteDir) {
    std::ofstream gbac_script(scriptName);
    gbac_script << "ENDPOINT=" << endpoint << std::endl;
    gbac_script << "GRID=" << grid << std::endl;
    gbac_script << "TAG=" << tag << std::endl;
    gbac_script << "EXENAME=" << exeName << std::endl;
    gbac_script << "FILELIST=" << filelist << std::endl;
    gbac_script << "PATH='http://opres.mik.uni-pannon.hu/gbac/" << remoteDir << "/'" << std::endl;
    gbac_script << std::endl;

    int linalgCount = 0;
    int simplexCount = 0;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (".")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            std::string entry(ent->d_name);
            if(entry.size()>=11 && entry.substr(0 , 11).compare("linalg.PAR_") == 0){
                linalgCount++;
            }
            if(entry.size()>=12 && entry.substr(0 , 12).compare("simplex.PAR_") == 0){
                simplexCount++;
            }
        }
        closedir (dir);
    } else {
        std::cout << "Error opening the working directory.\n";
    }

    for(int linalgIndex=0; linalgIndex<linalgCount; linalgIndex++){
        for(int simplexIndex=0; simplexIndex<simplexCount; simplexIndex++){
            gbac_script << "OUTPUT_NAME='result_" << linalgIndex << "_" << simplexIndex << ".txt'" << std::endl;
            gbac_script << "wsclient \\" << std::endl;
            gbac_script << "\t -m add \\" << std::endl;
            gbac_script << "\t -e \"$ENDPOINT\" \\" << std::endl;
            gbac_script << "\t -g \"$GRID\" \\" << std::endl;
            gbac_script << "\t -a \"-fl $FILELIST -o $OUTPUT_NAME\" \\" << std::endl;
            gbac_script << "\t -n gbac \\" << std::endl;
            gbac_script << "\t -i $EXENAME.app.tgz=\"$PATH\"NewPanOptSolver.app.tgz \\" << std::endl;
            gbac_script << "\t -i $FILELIST=\"$PATH\"$FILELIST \\" << std::endl;
            gbac_script << "\t -i linalg.PAR=\"$PATH\"linalg.PAR_" << linalgIndex << " \\" << std::endl;
            gbac_script << "\t -i simplex.PAR=\"$PATH\"simplex.PAR_" <<simplexIndex << " \\" << std::endl;
            gbac_script << "\t -t \"$TAG\" \\" << std::endl;
            gbac_script << "\t -o \"$OUTPUT_NAME\" \\" << std::endl;
            gbac_script << "\t -o " << exportResult << std::endl;
            gbac_script << std::endl;
        }
    }
}

void printHelp() {
    std::cout << "Usage: NewPanOptParameterGenerator [OPTION] \n" <<
                 "Generate parameter file for the NewPanOpt solver. \n"<<
                 "\n"<<
                 "  -f, --file      \t Two valid parameter values for the argument are `linalg.PAR` and `simplex.PAR`. \n"<<
                 "  -r, --range     \t Specifies a parameter range with a NAME TYPE FROM STEP NUMBER tuple.\n"<<
                 "  -p, --parameter \t Specifies a parameter range with a NAME TYPE VALUE.\n"<<
                 "  -s, --script    \t Generate GBAC script REMOTEDIR.\n"<<
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
    std::vector<ParameterValue> values;

    bool generateScript = false;
    std::string path;

    if(argc < 2){
        printHelp();
    } else if(argc > 1){
        for(int i=1; i<argc; i++){
            std::string arg(argv[i]);
//            std::cout << "arg: "<< arg << "\n";
            if(arg.compare("-h") == 0 || arg.compare("--help") == 0){
                printHelp();
            } else if(arg.compare("-s") == 0 || arg.compare("--script") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    generateScript = true;
                    path = argv[i+1];
                    i++;
                }
            } else if(arg.compare("-f") == 0 || arg.compare("--file") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    fileName = argv[i+1];
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
                if(argc < i+6 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    ParameterRange range;
                    range.name = argv[i+1];
                    range.type = argv[i+2];
                    range.from = argv[i+3];
                    range.steplength = atof(argv[i+4]);
                    range.number = atoi(argv[i+5]);
                    ranges.push_back(range);
                    i+=5;
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

    if(fileName.compare("linalg.PAR") == 0){
        ParameterHandler & handler = LinalgParameterHandler::getInstance();
        generateParameters(ranges, values, handler);
        std::cout << "linalg.PAR files generated!\n";
    } else if(fileName.compare("simplex.PAR") == 0){
        ParameterHandler & handler = SimplexParameterHandler::getInstance();
        generateParameters(ranges, values, handler);
        std::cout << "simplex.PAR files generated!\n";
    } else if(generateScript){
        generateGbacScript(path);
        std::cout << "GBAC script generated!\n";
    } else if(fileName.compare("") == 0){
        std::cout << "Please specify the name of the parameter file. Use `--help` for further information!\n";
        return EXIT_FAILURE;
    } else {
        std::cout << "Invalid parameter file name. Use `--help` for further information!\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

