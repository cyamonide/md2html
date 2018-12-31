#ifndef MD2HTML_CONSTANTS
#define MD2HTML_CONSTANTS

#include <regex>

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

const regex r_heading("#{1,6} .*");
const regex r_heading_alt("(=+|-+)");
const regex r_ordered("\\s*\\d+\\. .*");
const regex r_unordered("\\s*(-|\\*|\\+) .*");
const regex r_html_tag("\\s*<([A-Za-z0-9]+)*>\\s*");

#endif
