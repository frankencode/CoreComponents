module;

#include <cc/AppendList>

export module cc_core;

export namespace cc {
int getNine() { return 9; }

using cc::AppendList;
using cc::AppendListTransaction;
using cc::AppendListIterator;
}
