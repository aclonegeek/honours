#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

int main(const int argc, const char** argv) {
    doctest::Context ctx;

    ctx.applyCommandLine(argc, argv);

    int res = ctx.run();
    if (ctx.shouldExit()) {
        return res;
    }

    return res;
}
