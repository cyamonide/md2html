#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

using namespace std;

string header(string body) {
    
}

string parseLine(string line) {
    
}

// call parseInline recursively
/*
    Inline syntax
        Math $ $
        Bold
        Italics

*/

/*

    Note: it might not be necessary to look for matching syntax
    Go through the string linearly, and keep track of which ones are open and/or closed.
    If there are unmatched characters...

    

*/

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "No input file defined!");
        return 1;
    }

    fstream fin;
    ofstream fout;

    fin.open(argv[1]);
    if (argc > 2) {
        fout.open(argv[2]);
    } else {
        fout.open("out.html");
    }

    for (int i = 0; i <= 10; i++) {
        string foo;
        getline(fin, foo);

        for (int j = 0; j < foo.length(); j++) {
            if (foo[j] == ' ') {
                foo[j] = '~';
            }
        }

        cout << i << endl;
        cout << foo << endl << endl;
    }

    fin.close();
    fout.close();

    return 0;
}