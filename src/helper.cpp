#include "helper.h"

bool isList(Tag s) { return s.type == "ol" || s.type == "ul"; }
bool isList(std::string s) { return s == "ol" || s == "ul"; }

// Open and close tag formatters
std::string ot(std::string tag) { return "<" + tag + ">"; };
std::string ct(std::string tag) { return "</" + tag + ">"; };
std::string wrapTags(std::string tag, std::string content) { return ot(tag) + content + ct(tag); }
