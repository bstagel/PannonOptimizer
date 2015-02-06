#include "filereportgenerator.h"
#include <string>
#include <iostream>

void FileReportGenerator::getFile(const std::string & directory,
                                  const std::string & fileName,
                                  std::ofstream * output) const
{
    std::string path = fixPath(directory) + fileName;
    /*char delim;
#ifdef __linux__
    delim = '/';
#endif
#ifdef WIN32
    delim = '\\';
#endif
    if (path[ path.size() - 1 ] != delim) {
        path = path + delim + fileName;
    } else {
        path = path + fileName;
    }*/

    system((std::string("mkdir ") + directory).c_str());

    output->open(path.c_str());
    if (output->is_open() == false) {
        // TODO: throw an exception
    }
}

void FileReportGenerator::clearDirectory(const std::string &directory) const
{
#ifdef __linux__
    system((std::string("rm ") + fixPath(directory) + "*").c_str());
#endif
#ifdef WIN32
    system((std::string("del ") + fixPath(directory) + "* /q").c_str());
#endif
}

std::string FileReportGenerator::getNewIndexedFileName(const std::string & prefix,
                                                       const std::string & postfix) const
{
    unsigned int index = 0;
    std::ifstream ifs;
    do {
        if (ifs.is_open()) {
            ifs.close();
        }
        std::string name = prefix + std::to_string(index) + postfix;
        ifs.open(name.c_str());
        if (ifs.is_open() == false) {
            return name;
        }
        index++;
    } while (ifs.is_open() == true);
    return "";
}

std::string FileReportGenerator::fixPath(const std::string & path) const
{
    std::string result = std::string(path);
    char delim;
#ifdef __linux__
    delim = '/';
#endif
#ifdef WIN32
    delim = '\\';
#endif
    if (result[ result.size() - 1 ] != delim) {
        result = result + delim;
    }
    return result;
}
