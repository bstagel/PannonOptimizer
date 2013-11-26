#include <globals.h>

#include <string>
#include <sys/stat.h>
#include <dirent.h>

#include <lp/model.h>
#include <lp/mpsproblem.h>
#include <simplex/dualsimplex.h>

void solve(std::string filename) {
    //Init
   Simplex* simplex = new DualSimplex();
   Model model;
   MpsModelBuilder builder;
   builder.loadFromFile(filename.c_str());
   model.build(builder);

   simplex->setModel(model);
   simplex->solve();

   //Release
   delete simplex;
   simplex = NULL;
}

void printHelp() {
    std::cout << "Usage: NewPanOptDual [OPTION] [FILE] \n" <<
                 "Solve [FILE] with the dual simplex method. \n"<<
                 "\n"<<
                 "Algorithm specific parameters can be given in the .PAR parameter files. \n"<<
                 "If these files not exist, use the `-p` argument to generate them. \n"<<
                 "\n"<<
                 "  -d, --directory \t Solve every MPS file listed in the FILE directory.\n"<<
                 "  -f, --file      \t Solve every MPS file listed in the FILE file.\n"<<
                 "  -p              \t Generate the default parameter files.\n"<<
                 "  -h, --help      \t Displays this help.\n"<<
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
                 "Try `"<< argv[0] <<" --help` for more information.\n"<<
                 "\n";
}

void printInvalidOperandError(char** argv, int opindex, int argindex) {
    std::cout << argv[0]<< ": invalid operand: `" << argv[argindex]<<"` for option: `"<< argv[opindex] <<"`\n"<<
                 "Try `"<< argv[0] <<" --help` for more information.\n"<<
                 "\n";
}

void printMissingOperandError(char** argv) {
    std::cout << argv[0]<< ": missing operand \n"<<
                 "Try `"<< argv[0] <<" --help` for more information.\n"<<
                 "\n";
}

void generateParameterFiles() {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (".")) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        std::string entry(ent->d_name);
        std::cout << entry;
        if(entry.substr(std::string::npos -4 , 4).compare(".PAR") == 0){
            std::cout << "PAR HERE! \n";
        }
      }
      closedir (dir);
    } else {
      /* could not open directory */
      std::cout << "Error opening the working directory";
    }
}

int main(int argc, char** argv)
{
    setbuf(stdout, 0);
    if(argc < 2){
        printHelp();
    } else if(argc > 1){
        std::string arg(argv[1]);
        if(arg.compare("-h") == 0 || arg.compare("--help") == 0){
            printHelp();
        } else if(arg.compare("-d") == 0 || arg.compare("--directory") == 0){
            if(argc < 3 ){
                std::cout << argc;
                printMissingOperandError(argv);
            } else {
                std::string path(argv[2]);
                if(isDir(path)){
                    std::cout << "DIR argument - OK\n";
                } else {
                    printInvalidOperandError(argv, 1, 2);
                }
            }
        } else if(arg.compare("-f") == 0 || arg.compare("--file") == 0){
            if(argc < 3 ){
                printMissingOperandError(argv);
            } else {
                std::string path(argv[2]);
                if(isDir(path)){
                    std::cout << "FILE argument - OK";
                } else {
                    printInvalidOperandError(argv, 1, 2);
                }
            }
        } else {
            printInvalidOptionError(argv, 1);
        }
    }


    return EXIT_SUCCESS;
}

