#include <fstream>
#include <iostream>
#include <string>

#include "constants.h"
#include "md2html.h"

int main(int argc, char const *argv[])
{
    Md2html m;

    if (argc >= 3)
    {
        m.setInputPath(argv[1]);
        m.setOutputPath(argv[2]);
    }
    else if (argc == 2)
    {
        m.setInputPath(argv[1]);
        m.setOutputPath("out.html");
    }
    else
    {
        return 1;
    }

    m.start();

    return 0;
}