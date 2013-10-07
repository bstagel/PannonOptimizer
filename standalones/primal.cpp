#include <globals.h>

#include <lp/model.h>
#include <lp/mpsproblem.h>
#include <simplex/primalsimplex.h>

int main(int argc, char** argv)
{
    setbuf(stdout, 0);

    if(argc < 2){
        std::cout << "Please give an MPS file as an argument!\n";
        return -1;
    }

    //Init
    Simplex* simplex = new PrimalSimplex();
    Model model;
    MpsModelBuilder builder;
    builder.loadFromFile(argv[1]);
    model.build(builder);

    simplex->setModel(model);

    simplex->solve();

    //Release
    delete simplex;
    simplex = NULL;

    return EXIT_SUCCESS;
}

