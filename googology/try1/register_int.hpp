#include "processor_base.hpp"

struct IntProcessor {
    static int zero() { return 0; }

    static int limit(unsigned int n) {
        return static_cast<int>(n);
    }

    static void output(int val) {
        std::cout << "Integer: " << val << std::endl;
    }

    static int expand(int val, unsigned int n) {
        return val * static_cast<int>(n);
    }

    static std::string id() { return "int"; }

    static std::string name() { return "Integer Processor"; }

    static bool compare(int a, int b) {
        return a == b;
    }
};

namespace {
    TypeRegistrar<int, IntProcessor> int_registrar("int");
}
