#include "md2html.h"

#include "state.h"
#include "helper.h"
#include "blockparse.h"
#include "inlineparse.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <regex>
#include <deque>
#include <map>
#include <stack>

using namespace std;

/*
        BUFFER MANAGEMENT
*/

void writeBuffer(ostream& fout, deque<string> *buf) {
    while (buf->size() > 3) {
        fout << buf->back() << "\n";
        buf->pop_back();
    }
}

void flushBuffer(ostream& fout, deque<string> *buf) {
    while (buf->size() > 0) {
        fout << buf->back() << "\n";
        buf->pop_back();
    }
}

/*
        ASSERTS
*/

bool assert_resolveInline(string test, string expected, int num);

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

    // Filestreams
    ifstream fin;
    fin.open(infilepath);
    ofstream fout;
    fout.open(outfilepath, ios::ate);
    // Buffer deque
    deque<string> buf;
    // Line buffer
    string line_buf;
    // State
    stack<StateInfo> state;

    while (getline(fin, line_buf)) {
        buf.push_front(line_buf);
        resolveLine(&buf, &state);
        writeBuffer(fout, &buf);
    }

    // Close all open tags
    buf.push_front("");
    closeAllOpen(&buf, &state);

    flushBuffer(fout, &buf);

    fin.close();
    fout.close();

    return 0;
}

// ASSERTION TESTS //

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