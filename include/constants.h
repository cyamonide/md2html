#ifndef MD2HTML_CONSTANTS
#define MD2HTML_CONSTANTS

#include <regex>

using namespace std;

const regex R_CODE("`[^\\s][^`]*[^\\s]`");
const regex R_LINK("\\[[^\\[\\]\\(\\)]+\\]\\([^\\[\\]\\(\\)]+\\)");
const regex R_LINK_TITLE("\\[[^\\[\\]\\(\\)]+\\]\\([^\\[\\]\\(\\)]+ \"[^\\[\\]\\(\\)]+\"\\)");
const regex R_TRIPLE_A("\\*\\*\\*[^\\s][^\\*]*[^\\s]\\*\\*\\*");
const regex R_TRIPLE_U("___[^\\s][^_]*[^\\s]___");
const regex R_BOLD_A("\\*\\*[^\\s][^\\*]*[^\\s]\\*\\*");
const regex R_BOLD_U("__[^\\s][^_]*[^\\s]__");
const regex R_ITALICS_A("\\*[^\\s][^\\*]*[^\\s]\\*");
const regex R_ITALICS_U("_[^\\s][^_]*[^\\s]_");

const regex R_HEADING("#{1,6} .*");
const regex R_HEADING_ALT("(=+|-+)");
const regex R_ORDERED("\\s*\\d+\\. .*");
const regex R_UNORDERED("\\s*(-|\\*|\\+) .*");
const regex R_HTML_TAG("\\s*<([A-Za-z0-9]+)*>\\s*");
const regex R_HORIZONTAL_RULE("(\\*{3,}|-{3,}|_{3,})");
const regex R_CODEBLOCK_FENCED("(`{3}\\w*|~{3}\\w*)");
const regex R_CODEBLOCK_TABBED("(\\t| {4})[^\\s].*");

const string FILEPATH_IN = "./test/test.md";
const string FILEPATH_OUT = "./test/out.html";

#endif
