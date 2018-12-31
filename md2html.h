#ifndef MD2HTML
#define MD2HTML

#include <string>

using namespace std;

const regex r_code("`[^\\s][^`]*[^\\s]`");
const regex r_link("\\[[^\\[\\]\\(\\)]+\\]\\([^\\[\\]\\(\\)]+\\)");
const regex r_link_title("\\[[^\\[\\]\\(\\)]+\\]\\([^\\[\\]\\(\\)]+ \"[^\\[\\]\\(\\)]+\"\\)");
const regex r_triple_a("\\*\\*\\*[^\\s][^\\*]*[^\\s]\\*\\*\\*");
const regex r_triple_u("___[^\\s][^_]*[^\\s]___");
const regex r_bold_a("\\*\\*[^\\s][^\\*]*[^\\s]\\*\\*");
const regex r_bold_u("__[^\\s][^_]*[^\\s]__");
const regex r_italics_a("\\*[^\\s][^\\*]*[^\\s]\\*");
const regex r_italics_u("_[^\\s][^_]*[^\\s]_");


// DEPRECATED: Replaced by a simpler implementation using a stack
class State
{
public:
    int f_par;
    int f_ol;
    int f_ul;
    int f_bq;
    int f_cb;

    State(int par, int ol, int ul, int bq, int cb);
    State(const State &s);
    ~State();

    bool isZero();
};

State::State(int par = 0, int ol = 0, int ul = 0, int bq = 0, int cb = 0)
{
    f_par = par;
    f_ol = ol;
    f_ul = ul;
    f_bq = bq;
    f_cb = cb;
}

State::State(const State &s) {
    f_par = s.f_par;
    f_ol = s.f_ol;
    f_ul = s.f_ul;
    f_bq = s.f_bq;
    f_cb = s.f_cb;
}

State::~State()
{
}

bool State::isZero() {
    return f_par + f_ol + f_ul + f_bq + f_cb == 0;
}


struct StateInfo
{
    string type = "";
    int data = 0;
};


/*
        HELPER FUNCTIONS
*/

bool isList(StateInfo s);
bool isList(string s);

string ot(string tag);
string ct(string tag);
string wrapTags(string tag, string content);

/*
        INLINE RESOLVES
*/

string resolveCode(string line);
string resolveLink(string line);
string resolveTriple(string line);
string resolveBold(string line);
string resolveItalics(string line);

string resolveInline(string line);

/*
        BLOCK RESOLVES
*/

void resolveList(deque<string> *buf, stack<StateInfo> *state, string thisType);

void resolveLine(deque<string> *buf, stack<StateInfo> *state);
void closeAllOpen(deque<string> *buf, stack<StateInfo> *state);

/*
        BUFFER MANAGEMENT
*/

void writeBuffer(ostream& fout, deque<string> *buf);
void flushBuffer(ostream& fout, deque<string> *buf);

/**
 * Parses markdown headings indicated by hashtags.
 * 
 * @param line The line of text to be parsed, beginning with at least one hashtag.
 * @return The same line of text converted fully into HTML
 */
string heading(string line);

/**
 * Parses markdown headings indicated by alternate syntax.
 * 
 * @param line The line of text to be parsed.
 *        c The character used in the line immediately following. Either '=' or '-'.
 * @return The same line of text converted fully into HTML.
 */
string headingAlt(string line, char c);

/**
 * 
 */
bool isHeadingAlt(string line);

char getHeadingAltChar(string line);

/**
 * Parses a markdown paragraph.
 * 
 * @param line The line of text to be parsed.
 * @return The same line of text converted fully into HTML.
 */
string paragraph(string line);

string blockquotes(string line);

string parseLine(string line);

/**
 * headingAlt: if this line isHeadingAlt, then run headingAlt on the last element of buffer
 * numbered list: if 1. is matched, start a <ol>, and <li> with this line
 *      save lastIndentDist = 0
 *      if the next line is also matched as 1. then close </li> on last line and continue with <li>
 *      if regex matches the string to start with 1. and there are n leading spaces, this will define indentSize globally
 *      (lastIndentDist - thisIndentDist) / indentSize is the number of indents
 *      if this is a negative number -b, we must close b <ol> tags
 *      if this is a positive number b, we must open b <ol> tags
 * 
 *      empty line closes all <ol> tags
 * 
 *      What if the next line is at the same indent as the _text_ from the last line?
 *          Check if there is a double space in the last line, and if os, <br> it
 * 
 */

/****************************************/
/**************** INLINE ****************/
/****************************************/

#endif