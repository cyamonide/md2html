#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <queue>
#include <regex>
#include <algorithm>
#include "md2html.h"

using namespace std;

// Requires line be a link and link only
string resolveLink(string line) {
    regex r("[\\[]\\w+[\\]][\\(]\\w+[\\)]");
    smatch m;

    regex_search(line, m, r);

    string link = m.str();
    string href = link.substr(link.find("(") + 1);
    href = href.substr(0, href.length() - 1);
    string disp = link.substr(1, link.find("]") - 1);

    line.replace(m.position(), m.str().length(), "<a href=\"" + href + "\">" + disp + "</a>");
    
    return line;
}

string resolveCIB(string line) {
    regex r("(\\s\\*\\*\\w+|\\s`\\w+|\\s_\\w+)");
    smatch m;
    if (regex_search(line, m, r)) {
        cout << "found" << endl;
        return "";
    }
    cout << "not found" << endl;
    return line;
}

// Requires two `'s exist in line
string resolveInlineCode(string line) {
    regex r("`");
    sregex_iterator it(line.begin(), line.end(), r);
    sregex_iterator it_end;

    int i1 = it->position();
    int i2 = (++it)->position();

    return resolveInline(line.substr(0, i1)) + "<code>" + line.substr(i1 + 1, i2 - i1 - 1) + "</code>" + resolveInline(line.substr(i2 + 1));
}



string resolveInline(string line) {
    regex r("[\\[]\\w+[\\]][\\(]\\w+[\\)]");
    smatch m;
    if (regex_search(line, m, r)) {
        return resolveInline(line.substr(0, m.position())) + 
        resolveLink(line.substr(m.position(), m.str().length())) + 
        resolveInline(line.substr(m.position() + m.str().length()));
    }
    if (count(line.begin(), line.end(), '`') >= 2) return resolveInlineCode(line);
    // if (count(line.begin(), line.end(), '*') )

    return line;
}

// bold recognition will be done with matching ** regex pairs


int main() {
    string test1 = "Foobar, foobat, **[Foobar](Foobat)** this better work.";
    string test2 = "Foobar, `foobat`, `**[Foobar](Foobat)** this` better work.";
    string test3 = "The quick **[brown](fox)** jumped `over` [the](lazy) dog.";

    string utest = test3;

    cout << utest << endl;
    cout << resolveInline(utest) << endl;

    return 0;
}