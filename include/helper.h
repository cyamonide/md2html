#ifndef MD2HTML_HELPER
#define MD2HTML_HELPER

#include "state.h"

#include <iostream>
#include <string>

using namespace std;

bool isList(StateInfo s);
bool isList(string s);

// Open and close tag formatters
string ot(string tag);
string ct(string tag);
string wrapTags(string tag, string content);

#endif
