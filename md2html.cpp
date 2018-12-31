#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <regex>
#include <map>
#include <deque>
#include <stack>
#include "md2html.h"

using namespace std;

/*
        HELPER FUNCTIONS
*/

bool isList(StateInfo s) { return s.type == "ol" || s.type == "ul"; }
bool isList(string s) { return s == "ol" || s == "ul"; }

// Open and close tag formatters
string ot(string tag) { return "<" + tag + ">"; };
string ct(string tag) { return "</" + tag + ">"; };
string wrapTags(string tag, string content) { return ot(tag) + content + ct(tag); }

/*
        INLINE RESOLVES
*/

string resolveCode(string line) {
    line = line.substr(1, line.length() - 2);
    return wrapTags("code", line);
}

// Requires line be a link and link only
string resolveLink(string line) {
    // Add Title
    if (regex_match(line, r_link_title)) {
        string href = line.substr(line.find("(") + 1);
        string title = href;

        href = href.substr(0, href.find(" "));

        title = title.substr(title.find(" ") + 1);
        title = title.substr(0, title.length() - 1);

        string disp = line.substr(1, line.find("]") - 1);

        return "<a href=\"" + href + "\" title=" + title + ">" + disp + "</a>";
    }

    string href = line.substr(line.find("(") + 1);
    href = href.substr(0, href.length() - 1);
    string disp = line.substr(1, line.find("]") - 1);

    // line.replace(0, line.length(), "<a href=\"" + href + "\">" + disp + "</a>");
    
    return "<a href=\"" + href + "\">" + disp + "</a>";
}

string resolveTriple(string line) {
    line = line.substr(3, line.length() - 6);
    return wrapTags("em", wrapTags("strong", resolveInline(line)));
}

string resolveBold(string line) {
    line = line.substr(2, line.length() - 4);
    return wrapTags("strong", resolveInline(line));
}

string resolveItalics(string line) {
    line = line.substr(1, line.length() - 2);
    return wrapTags("em", resolveInline(line));
}

string resolveInline(string line) {
    // Double space line break
    if (regex_match(line, regex(".*[^ ]  "))) return resolveInline(line.substr(0, line.length() - 2)) + "<br>";

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

/*
        BLOCK RESOLVES
*/

void resolveList(deque<string> *buf, stack<StateInfo> *state, string thisType) {
    // Close li tag if it exists
    if (!state->empty() && state->top().type == "li") {
        buf->at(1) += ct("li");
        state->pop();
    }

    string line = buf->at(0);
    smatch m;

    // Get indentation size
    if (thisType == "ol") regex_search(line, m, regex("\\d*\\. "));
    else regex_search(line, m, regex("(-|\\*|\\+) "));
    int thisIndent = m.position();
    // Strip bullet or digits
    line = line.substr(m.position() + m.str().length());
    // Set opposite list type (ordered/unordered)
    string oppType = "ul";
    if (thisType == "ul") oppType = "ol";

    // If state empty, create a new list
    if (state->empty()) {
        state->push({ thisType, thisIndent });
        buf->at(0) = ot(thisType) + ot("li") + resolveInline(line);
        state->push({ "li" });
    }
    // Otherwise, run through cases
    else {
        StateInfo top = state->top();

        // Start a new list if:
        // top isn't a list, or is a list but less indented than this line
        if (!isList(top) ||
        (isList(top) && top.data < thisIndent)) {
            state->push({ thisType, thisIndent });
            buf->at(0) = ot(thisType) + ot("li") + resolveInline(line);
            state->push({ "li" });
        }
        // Continue a list if:
        // top is the same type and indent as this line
        else if (top.type == thisType && top.data == thisIndent) {
            buf->at(0) = ot("li") + resolveInline(line);
            state->push({ "li" });
        }
        // Restart list of different type if:
        // top has same indentation but different type
        else if (top.data == thisIndent && top.type != thisType) {
            state->pop();
            state->push({ thisType, thisIndent });
            buf->at(1) += ct(oppType);
            buf->at(0) = ot(thisType) + ot("li") + resolveInline(line);
            state->push({ "li" });
        }
        // Terminate nested lists if:
        // top indentation is greater than that of this line
        else if (top.data > thisIndent) {
            string lastType = thisType;
            // Terminate all nested lists with indentation 
            while (!state->empty() && isList(state->top()) && state->top().data > thisIndent) {
                buf->at(1) += ct(state->top().type);
                lastType = state->top().type;
                state->pop();
            }

            // If somebody doesn't adhere to proper indentation,
            if (state->empty() || !isList(state->top()) || state->top().data < thisIndent) {
                // Trim last close tag
                buf->at(1) = buf->at(1).substr(0, buf->at(1).length() - 5);
                // Re-open list
                state->push({ lastType, thisIndent });
                // Add this line to that list
                buf->at(0) = ot("li") + resolveInline(line);
                state->push({ "li" });
            }
            else if (state->top().data == thisIndent) {
                buf->at(0) = ot("li") + resolveInline(line);
                state->push({ "li" });
            } else {
                cout << "Something went terribly wrong with list termination." << endl;
            }
        }
    }
}

const regex r_heading("#{1,6} .*");
const regex r_heading_alt("(=+|-+)");
const regex r_ordered("\\s*\\d+\\. .*");
const regex r_unordered("\\s*(-|\\*|\\+) .*");
const regex r_html_tag("\\s*<([A-Za-z0-9]+)*>\\s*");

void resolveLine(deque<string> *buf, stack<StateInfo> *state) {
    // Regex obj
    smatch m;
    string line = buf->at(0);

    // Headings
    if (regex_match(line, r_heading)) {
        // Close all open tags
        closeAllOpen(buf, state);

        regex_search(line, m, regex("#{1,6}"));
        int heading_level = m.str().length();
        buf->at(0) = wrapTags("h" + to_string(heading_level), resolveInline(line.substr(heading_level + 1)));
    }
    // Alternate headings
    else if (regex_match(line, r_heading_alt)) {
        // Determine if previous line was heading-compatible (rendered as paragraph)
        if (!regex_match(buf->at(1), regex("<p>.*"))) {
            buf->at(0) = ot("p") + buf->at(0);
            state->push({ "p" });
            return;
        }
        state->pop();
        // Determine first or second level heading
        string tag = "h1";
        if (line[0] == '-') tag = "h2";
        buf->at(1) = wrapTags(tag, buf->at(1).substr(3, buf->at(1).length() - 7));
        buf->pop_front();
        
        closeAllOpen(buf, state);
    }
    // Ordered list
    else if (regex_match(line, r_ordered)) {
        // Assumes indentation consistent across tabs/spaces
        resolveList(buf, state, "ol");
    }
    // Unordered list
    else if (regex_match(line, r_unordered)) {
        resolveList(buf, state, "ul");
    }
    // HTML tag by itself
    else if (regex_match(line, r_html_tag)) {
        // Do nothing
    }
    // Blank line
    else if (regex_match(line, regex("\\s*"))) {
        // pop all states
        closeAllOpen(buf, state);
    }
    // Line by itself - no syntax
    else {
        // Append this line to previous line
        if (!state->empty()) {
            buf->at(1) += " " + resolveInline(line);
            buf->pop_front();
        }
        // Start new paragraph if necessary (state empty)
        else {
            buf->at(0) = ot("p") + resolveInline(buf->at(0));
            state->push({ "p", 0 });
        }
    }
}

void closeAllOpen(deque<string> *buf, stack<StateInfo> *state) {
    while (!state->empty()) {
        buf->at(1) += ct(state->top().type);
        state->pop();
    }
}

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
    fin.open("test.md");
    ofstream fout;
    fout.open("out.html", ios::ate);
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