#ifndef MD2HTML_STATE
#define MD2HTML_STATE

#include <string>

using namespace std;

// DEPRECATED: Replaced by a simpler implementation using a stack
class State
{
public:
    int f_par;
    int f_ol;
    int f_ul;
    int f_bq;
    int f_cb;

    State(int par, int ol, int ul, int bq, int cb);
    State(const State &s);
    ~State();

    bool isZero();
};

State::State(int par = 0, int ol = 0, int ul = 0, int bq = 0, int cb = 0)
{
    f_par = par;
    f_ol = ol;
    f_ul = ul;
    f_bq = bq;
    f_cb = cb;
}

State::State(const State &s) {
    f_par = s.f_par;
    f_ol = s.f_ol;
    f_ul = s.f_ul;
    f_bq = s.f_bq;
    f_cb = s.f_cb;
}

State::~State()
{
}

bool State::isZero() {
    return f_par + f_ol + f_ul + f_bq + f_cb == 0;
}


struct StateInfo
{
    string type = "";
    int data = 0;
};

#endif