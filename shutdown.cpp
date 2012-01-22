#include <signal.h>

static volatile sig_atomic_t terminate = 0;

void set_terminate_flag(bool t) { terminate = static_cast<sig_atomic_t>(t); }
bool get_terminate_flag() { return static_cast<bool>(terminate); }
