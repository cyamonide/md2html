#include "helper.h"

#include "state.h"

#include <iostream>
#include <string>

using namespace std;

bool isList(StateInfo s) { return s.type == "ol" || s.type == "ul"; }
bool isList(string s) { return s == "ol" || s == "ul"; }

// Open and close tag formatters
string ot(string tag) { return "<" + tag + ">"; };
string ct(string tag) { return "</" + tag + ">"; };
string wrapTags(string tag, string content) { return ot(tag) + content + ct(tag); }
