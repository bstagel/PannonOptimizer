#include <globals.h>

#include <lp/model.h>
#include <lp/mpsproblem.h>
#include <simplex/dualsimplex.h>

int main(int, char** )
{
    setbuf(stdout, 0);

    //Init
    Simplex* simplex = new DualSimplex();
    Model model;
    MpsModelBuilder builder;
    model.build(builder);

    simplex->setModel(model);


    //Release
    delete simplex;
    simplex = NULL;

    return EXIT_SUCCESS;
}

