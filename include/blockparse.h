#ifndef MD2HTML_BLOCK_PARSE
#define MD2HTML_BLOCK_PARSE

#include <iostream>
#include <string>
#include <deque>
#include <stack>

#include "state.h"

using namespace std;

void resolveList(deque<string> *buf, stack<StateInfo> *state, string thisType);

void resolveLine(deque<string> *buf, stack<StateInfo> *state);

void closeAllOpen(deque<string> *buf, stack<StateInfo> *state);

#endif
