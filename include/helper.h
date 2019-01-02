#ifndef MD2HTML_HELPER_H_
#define MD2HTML_HELPER_H_

#include <iostream>
#include <string>

#include "md2html.h"

bool isList(Tag s);
bool isList(std::string s);

// Open and close tag formatters
std::string ot(std::string tag);
std::string ct(std::string tag);
std::string wrapTags(std::string tag, std::string content);

#endif
