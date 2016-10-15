//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

#include <globals.h>

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>

#include <lp/model.h>
#include <lp/mpsproblem.h>
#include <lp/lpproblem.h>
#include <lp/presolver.h>
#include <simplex/dualsimplex.h>
#include <simplex/primalsimplex.h>
#include <simplex/simplexparameterhandler.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexcontroller.h>

#include <utils/tokenizer.h>
#include <simplex/checker.h>

#include <linalg/indexeddensevector.h>

void solve(std::string filename, bool dump_vars = false) {

    Model model;

    if(filename.size() > 3 && filename.substr(filename.size() - 3, 3) == ".LP") {
        LpModelBuilder* builder = new LpModelBuilder();
        builder->loadFromFile(filename);
        model.build(*builder);
        delete builder;
    } else {
        MpsModelBuilder* builder = new MpsModelBuilder();
        builder->loadFromFile(filename.c_str());
        model.build(*builder);
        delete builder;
    }
    if(SimplexParameterHandler::getInstance().getIntegerParameterValue("Global.debug_level") > 1) {
        LPINFO("Number of nonzeros: "<<model.getMatrix().nonZeros());
        LPINFO("Density: "<<model.getMatrix().density());
    }

    if(SimplexParameterHandler::getInstance().getBoolParameterValue("Starting.Presolve.enable") == true){
        Presolver presolver(&model);
        try {
            presolver.presolve();
#ifndef NDEBUG
            presolver.printStatistics();
#endif
//            exit(-1);
        } catch(Presolver::PresolverException e) {
            LPERROR("[Presolver] " << e.getMessage());
            exit(-1);
        }
    }

    if(SimplexParameterHandler::getInstance().getBoolParameterValue("Starting.Scaling.enable") == true){
        model.scale();
    }

    //init simplexController
    SimplexController simplexController;

    simplexController.solve(model);

    if(dump_vars) {
        const DenseVector& solution = simplexController.getPrimalSolution();
        for(unsigned i = 0; i < model.variableCount(); i++) {
            LPINFO(model.getVariable(i).getName() << " : " << solution[i]);
        }
        LPINFO("-");
    }
}

void printHelp() {
    std::cout << "Usage: NewPanOptDual [OPTION] [FILE] \n" <<
                 "Solve [FILE] with the dual simplex method. \n"<<
                 "\n"<<
                 "Algorithm specific parameters can be given in the .PAR parameter files. \n"<<
                 "If these files not exist, use the `-p` argument to generate them. \n"<<
                 "\n"<<
                 "   -$PARAMETER_NAME \t Specifies the value of a parameter .\n"<<
                 "   -d, --directory \t Solve every MPS file listed in the FILE directory.\n"<<
                 "   -f, --file      \t Solve an MPS file.\n"<<
                 "  -fl, --file-list \t Solve all the MPS files listed in text file.\n"<<
                 "   -p, --parameter-file \t Generate the default parameter files.\n"<<
                 "  -dv, --dump-variables \t Prints the variable values to the output after solution.\n"
                 "   -o, --output    \t Redirect the solver output to a file.\n"<<
                 "   -h, --help      \t Displays this help.\n"<<
                 "\n";

}

bool isFile(std::string path){
    struct stat s;
    if( stat(path.c_str(),&s) == 0 )
    {
        if( s.st_mode & S_IFREG )
        {
            return true;
        }
    }
    return false;
}

bool isDir(std::string path){
    struct stat s;
    if( stat(path.c_str(),&s) == 0 )
    {
        if( s.st_mode & S_IFDIR )
        {
            return true;
        }
    }
    return false;
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

void generateParameterFiles() {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (".")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            std::string entry(ent->d_name);
            if(entry.size()>=4 && entry.substr(entry.size()-4 , 4).compare(".PAR") == 0){
                remove(entry.c_str());
                std::cout << "File " << entry << " removed! \n";
            }
        }
        closedir (dir);
        SimplexParameterHandler::getInstance().initParameters();
        SimplexParameterHandler::getInstance().writeParameterFile();
        LinalgParameterHandler::getInstance().initParameters();
        LinalgParameterHandler::getInstance().writeParameterFile();
        InitPanOpt::getInstance().getArchitecture().generateParameterFile();
        std::cout << "Default paramterer files generated! \n";
    } else {
        std::cout << "Error opening the working directory.\n";
    }
}

void solveDir(std::string dirPath) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (dirPath.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            std::string entry(ent->d_name);
            if(entry.size()>=4){
                LPINFO("ENTRY: "<<entry);
            }
            if(entry.size()>=4 && (entry.substr(entry.size()-4 , 4).compare(".MPS") == 0 ||
                                   entry.substr(entry.size()-4 , 4).compare(".mps") == 0) ){
                std::string filePath = dirPath;
                filePath.append({PATH_SEPARATOR}).append(entry);
                solve(filePath);
            }
        }
        closedir (dir);
    } else {
        std::cout << "Error opening the working directory.\n";
    }
}

void solveFileList(std::string fileListPath) {
    std::string line;
    std::ifstream fileList(fileListPath);
    if(fileList.is_open()) {
        while(getline(fileList,line) ) {
            std::cout << "LINE: "<<line << "\n";
            if(line.size()>=4 && line.substr(line.size()-4 , 4).compare(".MPS") == 0){
                solve(line);
            } else {
                std::cout << "Invalid record in the list: "<<line << "\n";
            }

        }
        fileList.close();
    }
    else {
        std::cout << "Unable to open file list.\n";
    }
}

void redirectOutput(std::string path) {
    OutputHandler::getInstance().disableAllColors();
    freopen(path.c_str(), "a", stderr);
}

bool setParameter(ParameterHandler& handler, const std::string& arg, const char * value){
    if(handler.getParameterType(arg) == Entry::BOOL){
        if(strcmp(value,"true") == 0){
            handler.setParameterValue(arg, true);
        } else if(strcmp(value,"false") == 0){
            handler.setParameterValue(arg, false);
        } else {
            return false;
        }
    } else if(handler.getParameterType(arg) == Entry::DOUBLE) {
        handler.setParameterValue(arg, atof(value));
    } else if(handler.getParameterType(arg) == Entry::INTEGER) {
        handler.setParameterValue(arg, atoi(value));
    } else if(handler.getParameterType(arg) == Entry::STRING) {
        handler.setParameterValue(arg, std::string(value));
    } else {
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    //setbuf(stdout, 0);
    std::vector<std::pair<std::string, std::string> > solvables;
    bool outputRedirected = false;
    bool dump_vars = false;

    ParameterHandler& linalgHandler = LinalgParameterHandler::getInstance();
    ParameterHandler& simplexHandler = SimplexParameterHandler::getInstance();
    if(argc < 2){
        printHelp();
    } else if(argc > 1){
        for(int i=1; i<argc; i++){
            std::string arg(argv[i]);
            if(arg.compare("-h") == 0 || arg.compare("--help") == 0){
                printHelp();
            } else if(arg.compare("-d") == 0 || arg.compare("--directory") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    std::string path(argv[i+1]);
                    if(isDir(path)){
                        solvables.push_back(std::pair<std::string, std::string>(std::string("d"),path));
                        i++;
                    } else {
                        printInvalidOperandError(argv, i, i+1);
                        break;
                    }
                }
            } else if(arg.compare("-dv") == 0 || arg.compare("--dump-variables") == 0) {
                dump_vars = true;
            } else if(arg.compare("-f") == 0 || arg.compare("--file") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    std::string path(argv[i+1]);
                    if(isFile(path)){
                        solvables.push_back(std::pair<std::string, std::string>(std::string("f"),path));
                        i++;
                    } else {
                        printInvalidOperandError(argv, i, i+1);
                        break;
                    }
                }
            } else if(arg.compare("-fl") == 0 || arg.compare("--file-list") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    std::string path(argv[i+1]);
                    if(isFile(path)){
                        solvables.push_back(std::pair<std::string, std::string>(std::string("fl"),path));
                        i++;
                    } else {
                        printInvalidOperandError(argv, i, i+1);
                        break;
                    }
                }
            } else if(arg.compare("-p") == 0 || arg.compare("--parameter-file") == 0){
                generateParameterFiles();
            } else if(arg.compare("-o") == 0 || arg.compare("--output") == 0){
                if(argc < i+2 ){
                    printMissingOperandError(argv);
                    break;
                } else {
                    std::string path(argv[i+1]);
                    redirectOutput(path);
                    outputRedirected = true;
                    i++;
                }
            } else if(arg.compare(0,1,"-") == 0) {
                arg.erase(arg.begin());
                //Check if the given value is a linalg parameter
                if(linalgHandler.hasParameter(arg)){
                    if(!setParameter(linalgHandler,arg,argv[i+1])){
                        printInvalidOperandError(argv, i, i+1);
                        break;
                    }
                }
                //Check if the givel value is a simplex parameter
                else if(simplexHandler.hasParameter(arg)){
                    if(!setParameter(simplexHandler,arg,argv[i+1])){
                        printInvalidOperandError(argv, i, i+1);
                        break;
                    }
                } else {
                    printInvalidOptionError(argv, i);
                    break;
                }
                i++;
            } else {
                printInvalidOptionError(argv, i);
                break;
            }
        }
    }

    for(unsigned int i=0; i<solvables.size(); i++){
        if(solvables[i].first.compare("d") == 0){
            solveDir(solvables[i].second);
        } else if(solvables[i].first.compare("f") == 0){
            solve(solvables[i].second, dump_vars);
        } else if(solvables[i].first.compare("fl") == 0){
            solveFileList(solvables[i].second);
        }
    }

    if(outputRedirected){
        fclose(stdout);
    }

    return EXIT_SUCCESS;
}

