#include "processor_base.hpp"
#include <vector>

struct VectorProcessor {
    static std::vector<double> zero() {
        return {};
    }

    static std::vector<double> limit(unsigned int n) {
        return std::vector<double>(n, 100.0);
    }

    static void output(const std::vector<double>& v) {
        std::cout << "Vector[";
        for (size_t i = 0; i < v.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << v[i];
        }
        std::cout << "]" << std::endl;
    }

    static std::vector<double> expand(const std::vector<double>& v, unsigned int n) {
        if (v.empty()) return {};

        std::vector<double> result;
        for (size_t i = 0; i < n; i++) {
            for (double d : v) {
                result.push_back(d * (i + 1));
            }
        }
        return result;
    }

    static std::string id() { return "vector"; }

    static std::string name() { return "Vector Processor"; }

    static bool compare(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); i++) {
            if (std::abs(a[i] - b[i]) > 1e-6) return false;
        }
        return true;
    }
};

namespace {
    TypeRegistrar<std::vector<double>, VectorProcessor> vector_registrar("vector");
}
