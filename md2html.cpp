#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <queue>
#include <regex>
#include <map>
#include <algorithm>
#include "md2html.h"

using namespace std;

// Status flags
int f_par = 0;  // Paragraph
int f_ol = 0;   // Ordered list
int f_ul = 0;   // Unordered list
int f_bq = 0;   // Blockquote
int f_cb = 0;   // Codeblock


string resolveCode(string line) {
    line = line.substr(1, line.length() - 2);
    return "<code>" + line + "</code>";
}

// Requires line be a link and link only
string resolveLink(string line) {
    string href = line.substr(line.find("(") + 1);
    href = href.substr(0, href.length() - 1);
    string disp = line.substr(1, line.find("]") - 1);

    line.replace(0, line.length(), "<a href=\"" + href + "\">" + disp + "</a>");
    
    return line;
}

string resolveTriple(string line) {
    line = line.substr(3, line.length() - 6);
    return "<em><strong>" + resolveInline(line) + "</strong></em>";
}

string resolveBold(string line) {
    line = line.substr(2, line.length() - 4);
    return "<strong>" + resolveInline(line) + "</strong>";
}

string resolveItalics(string line) {
    line = line.substr(1, line.length() - 2);
    return "<em>" + resolveInline(line) + "</em>";
}

string resolveInline(string line) {
    // Regex obj
    smatch m;

    pair<int, string> mFirst;
    mFirst.first = line.length();
    mFirst.second = "";
    string mType = "";

    // Resolve inline code
    if (regex_search(line, m, r_code)) {
        if (m.position() < mFirst.first) {
            mFirst.first = m.position();
            mFirst.second = m.str();
            mType = "code";
        }
    }
    // Resolve links
    if (regex_search(line, m, r_link)) {
        if (m.position() < mFirst.first) {
            mFirst.first = m.position();
            mFirst.second = m.str();
            mType = "link";
        }
    }
    // Resolve triple asterisk/italics
    if (regex_search(line, m, r_triple_a) || regex_search(line, m, r_triple_u)) {
        if (m.position() < mFirst.first) {
            mFirst.first = m.position();
            mFirst.second = m.str();
            mType = "triple";
        }
    }
    // Resolve bold
    if (regex_search(line, m, r_bold_a) || regex_search(line, m, r_bold_u)) {
        if (m.position() < mFirst.first) {
            mFirst.first = m.position();
            mFirst.second = m.str();
            mType = "bold";
        }
    }
    // Resolve italics
    if (regex_search(line, m, r_italics_a) || regex_search(line, m, r_italics_u)) {
        if (m.position() < mFirst.first) {
            mFirst.first = m.position();
            mFirst.second = m.str();
            mType = "italics";
        }
    }

    if (mType == "") return line;

    string retval = resolveInline(line.substr(0, mFirst.first));
    string content = line.substr(mFirst.first, mFirst.second.length());

    if (mType == "code") {
        retval += resolveCode(content);
    } else if (mType == "link") {
        retval += resolveLink(content);
    } else if (mType == "triple") {
        retval += resolveTriple(content);
    } else if (mType == "bold") {
        retval += resolveBold(content);
    } else if (mType == "italics") {
        retval += resolveItalics(content);
    }

    retval += resolveInline(line.substr(mFirst.first + mFirst.second.length()));

    return retval;
}

// bold recognition will be done with matching ** regex pairs

bool assert_resolveInline(string test, string expected, int num) {
    string result = resolveInline(test);
    if (result == expected) {
        cout << "resolveInline: test" << num << " OK" << endl;
        return true;
    }
     
    cout << "resolveInline test" << num << " expected: " << expected << endl;
    cout << "\t got: " << result << endl << endl;
    return false;
}

int main() {
    string test1 = "Foobar, foobat, **[Foobar](Foobat)** this better work.";
    string ex1 = "Foobar, foobat, <strong><a href=\"Foobat\">Foobar</a></strong> this better work.";
    assert_resolveInline(test1, ex1, 1);

    string test2 = "Foobar, `foobat`, `**[Foobar](Foobat)** this` better work.";
    string ex2 = "Foobar, <code>foobat</code>, <code>**[Foobar](Foobat)** this</code> better work.";
    assert_resolveInline(test2, ex2, 2);

    string test3 = "The quick **[brown](fox)** jumped `over` [the](lazy) dog.";
    string ex3 = "The quick <strong><a href=\"fox\">brown</a></strong> jumped <code>over</code> <a href=\"lazy\">the</a> dog.";
    assert_resolveInline(test3, ex3, 3);

    string test4 = "This is ***A triple*** test and so **_is this_** and *__this as well__*.";
    string ex4 = "This is <em><strong>A triple</strong></em> test and so <strong><em>is this</em></strong> and <em><strong>this as well</strong></em>.";
    assert_resolveInline(test4, ex4, 4);

    return 0;
}