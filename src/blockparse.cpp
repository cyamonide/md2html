#include "blockparse.h"

#include "state.h"
#include "constants.h"
#include "helper.h"
#include "inlineparse.h"

#include <iostream>
#include <string>
#include <regex>
#include <deque>
#include <stack>

using namespace std;

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

void resolveLine(deque<string> *buf, stack<StateInfo> *state) {
    // Regex obj
    smatch m;
    string line = buf->at(0);

    // Headings
    if (regex_match(line, R_HEADING)) {
        // Close all open tags
        closeAllOpen(buf, state);

        regex_search(line, m, regex("#{1,6}"));
        int heading_level = m.str().length();
        buf->at(0) = wrapTags("h" + to_string(heading_level), resolveInline(line.substr(heading_level + 1)));
    }
    // Alternate headings
    else if (regex_match(line, R_HEADING_ALT)) {
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
    else if (regex_match(line, R_ORDERED)) {
        // Assumes indentation consistent across tabs/spaces
        resolveList(buf, state, "ol");
    }
    // Unordered list
    else if (regex_match(line, R_UNORDERED)) {
        resolveList(buf, state, "ul");
    }
    // HTML tag by itself
    else if (regex_match(line, R_HTML_TAG)) {
        // Do nothing
    }
    // Horizontal rule
    else if (regex_match(line, R_HORIZONTAL_RULE)) {
        buf->at(0) = "<hr>";
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