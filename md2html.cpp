#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <queue>
#include <regex>
#include <algorithm>
#include "md2html.h"

using namespace std;

string buffer[2];
int olCount = 0;
int ulCount = 0;
bool pOpen = false;

string wrapTags(string line, string tag) {
    return "<" + tag + ">" + line + "</" + tag + ">";
}

string heading(string line) {
    // Count #'s
    int hcount = 0;
    while (hcount < line.length() && line[hcount] == '#' && hcount < 7) hcount++;
    // Ignore #'s greater than 7
    if (hcount >= 7) return "<p>" + parseInline(line);
    // Generate HTML
    string tag = "h" + to_string(hcount);
    return wrapTags(parseInline(line.substr(hcount)), tag);
}

string headingAlt(string line, char c) {
    line = line.substr(3, line.length() - 3);
    if (c == '=') return wrapTags(line, "h1");
    return wrapTags(line, "h2");
}

bool isHeadingAlt(string line) {
    return regex_match(line, regex("\\s*[=-]+\\s*"));
}

char getHeadingAltChar(string line) {
    while (line[0] == ' ') line = line.substr(1);
    return line[0];
}


string parseLine(string line) {
    // Empty line
    if (regex_match(line, regex("\\s*"))) {
        if (pOpen) buffer[1] += "</p>";
        pOpen = false;
        return line;
    }

    // headings
    if (line[0] == '#') return heading(line);

    // alt headings
    if (isHeadingAlt(line)) {
        if (buffer[1] != "") {
            buffer[1] = headingAlt(buffer[1], getHeadingAltChar(line));
        }
        return "";
    }

    if (pOpen) {
        return parseInline(line);
    }
    pOpen = true;
    return "<p>" + parseInline(line);
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

string resolveLinks(string line) {
    regex r("[\\[]\\w+[\\]][\\(]\\w+[\\)]");
    smatch m;
    if (regex_search(line, m, r)) {
        string link = m.str();
        string href = link.substr(link.find("(") + 1);
        href = href.substr(0, href.length() - 1);
        string disp = link.substr(1, link.find("]") - 1);

        line.replace(m.position(), m.str().length(), "<a href=\"" + href + "\">" + disp + "</a>");
        cout << line << endl;
        return "";
    }
    
    cout << "not found" << endl;
    return line;
}

string parseInline(string line) {
    // Remove leading spaces
    while (line.length() > 0 && line[0] == ' ') line = line.substr(1);

    // Empty line
    if (line.length() < 2) return line;

    // End of line break
    if (line[line.length() - 1] == ' ' && line[line.length() - 2] == ' ') {
        return parseInline(line.substr(0, line.length() - 2)) + "<br>";
    }

    // 

    // Other codestuffs

    return line;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "No input file defined!");
        return 1;
    }

    fstream fin;
    ofstream fout;

    fin.open(argv[1]);
    if (argc > 2) fout.open(argv[2]);
    else fout.open("out.html");

    string foo;

    getline(fin, foo);
    buffer[0] = parseLine(foo);

    while (getline(fin, foo)) {
        buffer[1] = buffer[0];
        buffer[0] = foo;
        buffer[0] = parseLine(buffer[0]);
        fout << buffer[1] << endl;
        cout << buffer[1] << endl;
    }

    fout << buffer[0] << endl;
    cout << buffer[0] << endl;

    fin.close();
    fout.close();

    return 0;
}