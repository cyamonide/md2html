#ifndef MD2HTML_BUFFER
#define MD2HTML_BUFFER

#include <iostream>
#include <deque>

using namespace std;

void writeBuffer(ostream& fout, deque<string> *buf);

void flushBuffer(ostream& fout, deque<string> *buf);

#endif