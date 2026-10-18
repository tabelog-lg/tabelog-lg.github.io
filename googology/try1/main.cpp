#include "processor_base.hpp"
#include "register_int.hpp"
#include "register_vector.hpp"

int main() {
    try {
        std::string selected_id = "vector";

        auto processor = ProcessorFactory::create(selected_id);
        std::cout << "Using processor: " << processor->name()
                  << " (ID: " << processor->id() << ")\n";

        std::any z = processor->zero();
        std::any l = processor->limit(3);
        std::any e = processor->expand(l, 2);

        std::cout << "\nZero value: ";
        processor->output(z);

        std::cout << "Limit value: ";
        processor->output(l);

        std::cout << "Expanded value: ";
        processor->output(e);

        std::cout << "\nComparison tests:\n";
        std::cout << "zero == zero: " << processor->compare(z, z) << "\n";
        std::cout << "zero == limit: " << processor->compare(z, l) << "\n";
        std::cout << "limit == limit: " << processor->compare(l, l) << "\n";
        std::cout << "limit == expanded: " << processor->compare(l, e) << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\nAvailable processor IDs: ";
    auto ids = ProcessorFactory::registered_ids();
    for (const auto& id : ids) {
        std::cout << id << " ";
    }
    std::cout << std::endl;

    return 0;
}
