#ifndef PROCESSOR_BASE_HPP
#define PROCESSOR_BASE_HPP

#include <iostream>
#include <vector>
#include <memory>
#include <any>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <string>
#include <typeindex>

class GenericProcessor {
public:
    virtual ~GenericProcessor() = default;
    virtual std::any zero() = 0;
    virtual std::any limit(unsigned int n) = 0;
    virtual void output(const std::any& data) = 0;
    virtual std::any expand(const std::any& data, unsigned int n) = 0;
    virtual std::string id() const = 0;
    virtual std::string name() const = 0;
    virtual bool compare(const std::any& a, const std::any& b) = 0;
};

class ProcessorFactory {
public:
    template <typename T, typename Impl>
    static void register_processor(const std::string& type_id) {
        creators()[type_id] = [] {
            return std::make_unique<GenericProcessorImpl<T, Impl>>();
        };
    }

    static std::unique_ptr<GenericProcessor> create(const std::string& type_id) {
        auto it = creators().find(type_id);
        if (it == creators().end()) {
            throw std::runtime_error("Unregistered processor type: " + type_id);
        }
        return it->second();
    }

    static std::vector<std::string> registered_ids() {
        std::vector<std::string> keys;
        for (auto& pair : creators()) {
            keys.push_back(pair.first);
        }
        return keys;
    }

private:
    using CreatorFunc = std::function<std::unique_ptr<GenericProcessor>()>;

    static std::unordered_map<std::string, CreatorFunc>& creators() {
        static std::unordered_map<std::string, CreatorFunc> instance;
        return instance;
    }

    template <typename T, typename Impl>
    class GenericProcessorImpl : public GenericProcessor {
    public:
        std::any zero() override { return Impl::zero(); }

        std::any limit(unsigned int n) override {
            return Impl::limit(n);
        }

        void output(const std::any& data) override {
            try {
                Impl::output(std::any_cast<T>(data));
            } catch (const std::bad_any_cast&) {
                throw std::runtime_error(std::string("Type mismatch in output for: ") + typeid(T).name());
            }
        }

        std::any expand(const std::any& data, unsigned int n) override {
            try {
                return Impl::expand(std::any_cast<T>(data), n);
            } catch (const std::bad_any_cast&) {
                throw std::runtime_error(std::string("Type mismatch in expand for: ") + typeid(T).name());
            }
        }

        std::string id() const override { return Impl::id(); }

        std::string name() const override { return Impl::name(); }

        bool compare(const std::any& a, const std::any& b) override {
            try {
                return Impl::compare(
                    std::any_cast<T>(a),
                    std::any_cast<T>(b)
                );
            } catch (const std::bad_any_cast&) {
                return false;
            }
        }
    };
};

template <typename T, typename Impl>
struct TypeRegistrar {
    TypeRegistrar(const std::string& type_id) {
        ProcessorFactory::register_processor<T, Impl>(type_id);
    }
};

#endif
