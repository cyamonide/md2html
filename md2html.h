#ifndef MD2HTML
#define MD2HTML

#include <string>

using namespace std;

/**
 * Adds matching HTML tags around a string.
 * 
 * @param line String of text to be wrapped.
 *        tag HTML tag without angle brackets.
 * @return That string of text with HTML tags around it.
 */
string wrapTags(string line, string tag);

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

string resolveLinks(string line);

string bold(string line);

string italics(string line);

string parseInline(string line);

#endif