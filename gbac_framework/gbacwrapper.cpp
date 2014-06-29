#include <iostream>
#include <fstream>

std::istream& safeGetline(std::istream& is, std::string& t)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}

int main()
{
    std::ifstream ifs("arglist.txt");
    if(!ifs) {
        std::cout << "Failed to open the file." << std::endl;
        return EXIT_FAILURE;
    }

    std::string line;
    while(!safeGetline(ifs, line).eof()){
        std::cout << "Executing the following command: " << line << '\n';
        system(line.c_str());
    }

    ifs.close();

    return 0;
}

