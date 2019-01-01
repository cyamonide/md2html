#include "buffer.h"

#include <iostream>
#include <deque>

using namespace std;

void writeBuffer(ostream& fout, deque<string> *buf) {
    while (buf->size() > 3) {
        fout << buf->back() << "\n";
        buf->pop_back();
    }
}

void flushBuffer(ostream& fout, deque<string> *buf) {
    while (buf->size() > 0) {
        fout << buf->back() << "\n";
        buf->pop_back();
    }
}
