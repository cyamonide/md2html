#ifndef MD2HTML_INLINE_PARSE
#define MD2HTML_INLINE_PARSE

#include <iostream>
#include <string>

using namespace std;

string resolveCode(string line);

// Requires line be a link and link only
string resolveLink(string line);
string resolveTriple(string line);
string resolveBold(string line);
string resolveItalics(string line);

string resolveInline(string line);

#endif
