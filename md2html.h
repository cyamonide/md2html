#ifndef MD2HTML
#define MD2HTML

#include <string>

using namespace std;

const regex r_code("`[^\\s][^`]*[^\\s]`");
const regex r_link("[\\[]\\w+[\\]][\\(]\\w+[\\)]");
const regex r_triple_a("\\*\\*\\*[^\\s][^\\*]*[^\\s]\\*\\*\\*");
const regex r_triple_u("___[^\\s][^_]*[^\\s]___");
const regex r_bold_a("\\*\\*[^\\s][^\\*]*[^\\s]\\*\\*");
const regex r_bold_u("__[^\\s][^_]*[^\\s]__");
const regex r_italics_a("\\*[^\\s][^\\*]*[^\\s]\\*");
const regex r_italics_u("_[^\\s][^_]*[^\\s]_");


string resolveCode(string line);
string resolveLink(string line);
string resolveTriple(string line);
string resolveBold(string line);
string resolveItalics(string line);

string resolveInline(string line);

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