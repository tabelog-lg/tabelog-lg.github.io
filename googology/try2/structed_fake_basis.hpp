#ifndef FAKE_BASIS_HPP
#define FAKE_BASIS_HPP

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <utility>
#include <tuple>
#include <set>
#include <queue>
#include <variant>
#include <map>

class FFFzExp;
class SimpleExp;
class CompatyList;
class Transla_Shift;

using FFFzExpPtr = std::shared_ptr<FFFzExp>;
using SimpleExpPtr = std::shared_ptr<SimpleExp>;
using CompatyListPtr = std::shared_ptr<CompatyList>;
using Transla_ShiftPtr = std::shared_ptr<Transla_Shift>;
using ExprVariant = std::variant<SimpleExpPtr, FFFzExpPtr>;

class CompatyList {
public:
    enum class Type { Text_CLa, Text_CLb };

private:
    Type type_;
    FFFzExpPtr cla_content_;
    CompatyListPtr clb_list_;
    FFFzExpPtr clb_fffz_;

public:
    explicit CompatyList(FFFzExpPtr exp)
        : type_(Type::Text_CLa), cla_content_(exp) {}

    CompatyList(CompatyListPtr list, FFFzExpPtr fffz)
        : type_(Type::Text_CLb), clb_list_(list), clb_fffz_(fffz) {}

    bool is_Text_CLa() const { return type_ == Type::Text_CLa; }
    bool is_Text_CLb() const { return type_ == Type::Text_CLb; }

    FFFzExpPtr extract_Text_CLa() const {
        if (!is_Text_CLa()) throw std::runtime_error("Not Text_CLa");
        return cla_content_;
    }

    std::pair<CompatyListPtr, FFFzExpPtr> extract_Text_CLb() const {
        if (!is_Text_CLb()) throw std::runtime_error("Not Text_CLb");
        return {clb_list_, clb_fffz_};
    }

    std::string to_string() const;
};

class SimpleExp {
public:
    enum class Type { Text_SEa, Text_SEb };

private:
    Type type_;
    FFFzExpPtr sea_content_;
    CompatyListPtr seb_list_;
    FFFzExpPtr seb_fffz_;

public:
    explicit SimpleExp(FFFzExpPtr exp)
        : type_(Type::Text_SEa), sea_content_(exp) {}

    SimpleExp(CompatyListPtr list, FFFzExpPtr fffz)
        : type_(Type::Text_SEb), seb_list_(list), seb_fffz_(fffz) {}

    bool is_Text_SEa() const { return type_ == Type::Text_SEa; }
    bool is_Text_SEb() const { return type_ == Type::Text_SEb; }

    FFFzExpPtr extract_Text_SEa() const {
        if (!is_Text_SEa()) throw std::runtime_error("Not Text_SEa");
        return sea_content_;
    }

    std::pair<CompatyListPtr, FFFzExpPtr> extract_Text_SEb() const {
        if (!is_Text_SEb()) throw std::runtime_error("Not Text_SEb");
        return {seb_list_, seb_fffz_};
    }

    std::string to_string() const;
};

class FFFzExp {
public:
    enum class Type { Text_0, Text_FEa, Text_FEb };

private:
    Type type_;
    SimpleExpPtr fea_content_;
    SimpleExpPtr feb_simple_;
    FFFzExpPtr feb_fffz_;

public:
    FFFzExp() : type_(Type::Text_0) {}

    explicit FFFzExp(SimpleExpPtr exp)
        : type_(Type::Text_FEa), fea_content_(exp) {}

    FFFzExp(SimpleExpPtr simple, FFFzExpPtr fffz)
        : type_(Type::Text_FEb), feb_simple_(simple), feb_fffz_(fffz) {}

    bool is_Text_0() const { return type_ == Type::Text_0; }
    bool is_Text_FEa() const { return type_ == Type::Text_FEa; }
    bool is_Text_FEb() const { return type_ == Type::Text_FEb; }

    SimpleExpPtr extract_Text_FEa() const {
        if (!is_Text_FEa()) throw std::runtime_error("Not Text_FEa");
        return fea_content_;
    }

    std::pair<SimpleExpPtr, FFFzExpPtr> extract_Text_FEb() const {
        if (!is_Text_FEb()) throw std::runtime_error("Not Text_FEb");
        return {feb_simple_, feb_fffz_};
    }

    std::string to_string() const;
};

class Transla_Shift {
public:
    enum class Type { Text_TSa, Text_TSb };

private:
    Type type_;
    FFFzExpPtr tsa_content_;
    Transla_ShiftPtr tsb_shift1_;
    Transla_ShiftPtr tsb_shift2_;
    Transla_ShiftPtr tsb_shift3_;
    FFFzExpPtr tsb_fffz1_;
    FFFzExpPtr tsb_fffz2_;
    FFFzExpPtr tsb_fffz3_;

public:
    explicit Transla_Shift(FFFzExpPtr exp)
        : type_(Type::Text_TSa),
          tsa_content_(exp),
          tsb_shift1_(nullptr), tsb_shift2_(nullptr), tsb_shift3_(nullptr),
          tsb_fffz1_(nullptr), tsb_fffz2_(nullptr), tsb_fffz3_(nullptr) {}

    Transla_Shift(Transla_ShiftPtr s1, Transla_ShiftPtr s2, Transla_ShiftPtr s3,
                 FFFzExpPtr f1, FFFzExpPtr f2, FFFzExpPtr f3)
        : type_(Type::Text_TSb),
          tsa_content_(nullptr),
          tsb_shift1_(s1), tsb_shift2_(s2), tsb_shift3_(s3),
          tsb_fffz1_(f1), tsb_fffz2_(f2), tsb_fffz3_(f3) {}

    bool is_Text_TSa() const { return type_ == Type::Text_TSa; }
    bool is_Text_TSb() const { return type_ == Type::Text_TSb; }

    FFFzExpPtr extract_Text_TSa() const {
        if (!is_Text_TSa()) throw std::runtime_error("Not Text_TSa");
        return tsa_content_;
    }

    std::tuple<Transla_ShiftPtr, Transla_ShiftPtr, Transla_ShiftPtr,
               FFFzExpPtr, FFFzExpPtr, FFFzExpPtr> extract_Text_TSb() const {
        if (!is_Text_TSb()) throw std::runtime_error("Not Text_TSb");
        return {tsb_shift1_, tsb_shift2_, tsb_shift3_,
                tsb_fffz1_, tsb_fffz2_, tsb_fffz3_};
    }

    Type get_type() const { return type_; }
};

std::string CompatyList::to_string() const {
    switch (type_) {
        case Type::Text_CLa:
            return cla_content_->to_string();
        case Type::Text_CLb:
            return clb_fffz_->to_string() + "," + clb_list_->to_string();
        default:
            return "Unknown";
    }
}

std::string SimpleExp::to_string() const {
    switch (type_) {
        case Type::Text_SEa:
            return "¦×Z(" + sea_content_->to_string() + ")";
        case Type::Text_SEb:
            return "¦×Z[" + seb_list_->to_string() + "](" + seb_fffz_->to_string() + ")";
        default:
            return "Unknown";
    }
}

std::string FFFzExp::to_string() const {
    switch (type_) {
        case Type::Text_0:
            return "0";
        case Type::Text_FEa:
            return fea_content_->to_string();
        case Type::Text_FEb:
            return feb_simple_->to_string() + "+" + feb_fffz_->to_string();
        default:
            return "Unknown";
    }
}

bool equal_length(const Transla_ShiftPtr& a, const Transla_ShiftPtr& b) {
    return a->get_type() == b->get_type();
}

std::vector<SimpleExpPtr> expand_FFFzExp(const FFFzExpPtr& exp) {
    std::vector<SimpleExpPtr> result;
    FFFzExpPtr current = exp;

    while (true) {
        if (current->is_Text_FEa()) {
            result.push_back(current->extract_Text_FEa());
            break;
        } else if (current->is_Text_FEb()) {
            auto [simple_exp, next_exp] = current->extract_Text_FEb();
            result.push_back(simple_exp);
            current = next_exp;

            if (current->is_Text_0()) {
                throw std::runtime_error("Cannot expand Text_0 in FFFzExp");
            }
        } else if (current->is_Text_0()) {
            throw std::runtime_error("Cannot expand Text_0 in FFFzExp");
        } else {
            throw std::runtime_error("Unexpected FFFzExp variant");
        }
    }

    return result;
}

std::vector<FFFzExpPtr> expand_CompatyList(const CompatyListPtr& list) {
    std::vector<FFFzExpPtr> result;
    CompatyListPtr current = list;

    while (true) {
        if (current->is_Text_CLa()) {
            result.push_back(current->extract_Text_CLa());
            break;
        } else if (current->is_Text_CLb()) {
            auto [next_list, fffz_exp] = current->extract_Text_CLb();
            result.push_back(fffz_exp);
            current = next_list;
        } else {
            throw std::runtime_error("Unexpected CompatyList variant");
        }
    }

    return result;
}

FFFzExpPtr combine_FFFzExp(const std::vector<SimpleExpPtr>& exps) {
    if (exps.empty()) {
        throw std::runtime_error("Empty vector cannot be combined into FFFzExp");
    }

    FFFzExpPtr result = std::make_shared<FFFzExp>(exps.back());

    for (auto it = exps.rbegin() + 1; it != exps.rend(); ++it) {
        result = std::make_shared<FFFzExp>(*it, result);
    }

    return result;
}

CompatyListPtr combine_CompatyList(const std::vector<FFFzExpPtr>& exps) {
    if (exps.empty()) {
        throw std::runtime_error("Empty vector cannot be combined into CompatyList");
    }

    CompatyListPtr result = std::make_shared<CompatyList>(exps.back());

    for (auto it = exps.rbegin() + 1; it != exps.rend(); ++it) {
        result = std::make_shared<CompatyList>(result, *it);
    }

    return result;
}

SimpleExpPtr get_first_FFFzExp(const FFFzExpPtr& exp) {
    auto expanded = expand_FFFzExp(exp);
    if (expanded.empty()) {
        throw std::runtime_error("No SimpleExp in FFFzExp");
    }
    return expanded.front();
}

SimpleExpPtr get_last_FFFzExp(const FFFzExpPtr& exp) {
    auto expanded = expand_FFFzExp(exp);
    if (expanded.empty()) {
        throw std::runtime_error("No SimpleExp in FFFzExp");
    }
    return expanded.back();
}

FFFzExpPtr get_first_CompatyList(const CompatyListPtr& list) {
    auto expanded = expand_CompatyList(list);
    if (expanded.empty()) {
        throw std::runtime_error("No FFFzExp in CompatyList");
    }
    return expanded.front();
}

FFFzExpPtr get_last_CompatyList(const CompatyListPtr& list) {
    auto expanded = expand_CompatyList(list);
    if (expanded.empty()) {
        throw std::runtime_error("No FFFzExp in CompatyList");
    }
    return expanded.back();
}

bool is_directly_include(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b);
bool is_directly_include(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b);
bool is_directly_include(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b);

bool is_directly_include(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b) {
    if (exp_a->is_Text_0()) {
        throw std::runtime_error("Error in is_directly_include: exp_a cannot be Text_0.");
    }

    if (exp_a->is_Text_FEa()) {
        auto simple_in_a = exp_a->extract_Text_FEa();
        return is_directly_include(simple_in_a, exp_b);
    }

    if (exp_a->is_Text_FEb()) {
        auto expanded = expand_FFFzExp(exp_a);
        for (const auto& simple_exp : expanded) {
            if (is_directly_include(simple_exp, exp_b)) {
                return true;
            }
        }
        return false;
    }

    throw std::runtime_error("Error in is_directly_include(FFFzExp, FFFzExp): Unexpected type for exp_a.");
}

bool is_directly_include(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b) {
    if (exp_a->is_Text_0()) {
        throw std::runtime_error("Error in is_directly_include: exp_a cannot be Text_0.");
    }

    if (exp_a->is_Text_FEa()) {
        auto simple_in_a = exp_a->extract_Text_FEa();
        return simple_in_a == exp_b;
    }

    if (exp_a->is_Text_FEb()) {
        auto expanded = expand_FFFzExp(exp_a);
        for (const auto& simple_exp : expanded) {
            if (simple_exp == exp_b) {
                return true;
            }
        }
        return false;
    }

    throw std::runtime_error("Error in is_directly_include(FFFzExp, SimpleExp): Unexpected type for exp_a.");
}

bool is_directly_include(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b) {
    if (exp_a->is_Text_SEa()) {
        auto fffz_in_a = exp_a->extract_Text_SEa();
        return fffz_in_a == exp_b;
    }

    if (exp_a->is_Text_SEb()) {
        auto [list_in_a, fffz_in_a] = exp_a->extract_Text_SEb();
        if (fffz_in_a == exp_b) {
            return true;
        }
        auto expanded_list = expand_CompatyList(list_in_a);
        for (const auto& fffz_exp : expanded_list) {
            if (fffz_exp == exp_b) {
                return true;
            }
        }
        return false;
    }

    throw std::runtime_error("Error in is_directly_include(SimpleExp, FFFzExp): Unexpected type for exp_a.");
}

bool is_include(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b);
bool is_include(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b);
bool is_include(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b);
bool is_include(const SimpleExpPtr& exp_a, const SimpleExpPtr& exp_b);

bool is_include_impl(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b, std::set<void*>& visited);
bool is_include_impl(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b, std::set<void*>& visited);
bool is_include_impl(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b, std::set<void*>& visited);
bool is_include_impl(const SimpleExpPtr& exp_a, const SimpleExpPtr& exp_b, std::set<void*>& visited);

std::set<FFFzExpPtr> get_children(const FFFzExpPtr& exp, std::set<void*>& visited);
std::set<FFFzExpPtr> get_children(const SimpleExpPtr& exp, std::set<void*>& visited);

bool is_include(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b) {
    std::set<void*> visited;
    return is_include_impl(exp_a, exp_b, visited);
}

bool is_include(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b) {
    std::set<void*> visited;
    return is_include_impl(exp_a, exp_b, visited);
}

bool is_include(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b) {
    std::set<void*> visited;
    return is_include_impl(exp_a, exp_b, visited);
}

bool is_include(const SimpleExpPtr& exp_a, const SimpleExpPtr& exp_b) {
    std::set<void*> visited;
    return is_include_impl(exp_a, exp_b, visited);
}

bool is_include_impl(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b, std::set<void*>& visited) {
    if (visited.find(exp_a.get()) != visited.end()) {
        return false;
    }
    visited.insert(exp_a.get());

    try {
        if (is_directly_include(exp_a, exp_b)) {
            return true;
        }
    } catch (const std::runtime_error&) { }

    auto children = get_children(exp_a, visited);
    for (const auto& child : children) {
        if (is_include_impl(child, exp_b, visited)) {
            return true;
        }
    }

    return false;
}

bool is_include_impl(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b, std::set<void*>& visited) {
    if (visited.find(exp_a.get()) != visited.end()) {
        return false;
    }
    visited.insert(exp_a.get());

    try {
        if (is_directly_include(exp_a, exp_b)) {
            return true;
        }
    } catch (const std::runtime_error&) { }

    auto children = get_children(exp_a, visited);

    for (const auto& child : children) {
        if (is_include_impl(child, exp_b, visited)) {
            return true;
        }
    }

    return false;
}

bool is_include_impl(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b, std::set<void*>& visited) {
    if (visited.find(exp_a.get()) != visited.end()) {
        return false;
    }
    visited.insert(exp_a.get());

    try {
        if (is_directly_include(exp_a, exp_b)) {
            return true;
        }
    } catch (const std::runtime_error&) { }

    auto children = get_children(exp_a, visited);
    for (const auto& child : children) {
        if (is_include_impl(child, exp_b, visited)) {
            return true;
        }
    }

    return false;
}

bool is_include_impl(const SimpleExpPtr& exp_a, const SimpleExpPtr& exp_b, std::set<void*>& visited) {
    if (visited.find(exp_a.get()) != visited.end()) {
        return false;
    }
    visited.insert(exp_a.get());

    if (exp_a == exp_b) {
        return true;
    }

    auto children = get_children(exp_a, visited);
    for (const auto& child : children) {
        if (is_include_impl(child, exp_b, visited)) {
            return true;
        }
    }

    return false;
}

std::set<FFFzExpPtr> get_children(const FFFzExpPtr& exp, std::set<void*>& visited) {
    std::set<FFFzExpPtr> children;

    if (exp->is_Text_FEa()) {
        auto simple_child = exp->extract_Text_FEa();
        if (visited.find(simple_child.get()) == visited.end()) {
            auto simple_children = get_children(simple_child, visited);
            children.insert(simple_children.begin(), simple_children.end());
        }
    }
    else if (exp->is_Text_FEb()) {
        auto [simple_exp, fffz_exp] = exp->extract_Text_FEb();

        if (visited.find(simple_exp.get()) == visited.end()) {
            auto simple_children = get_children(simple_exp, visited);
            children.insert(simple_children.begin(), simple_children.end());
        }

        children.insert(fffz_exp);
    }

    return children;
}

std::set<FFFzExpPtr> get_children(const SimpleExpPtr& exp, std::set<void*>& visited) {
    std::set<FFFzExpPtr> children;

    if (exp->is_Text_SEa()) {
        auto fffz_child = exp->extract_Text_SEa();
        children.insert(fffz_child);
    }
    else if (exp->is_Text_SEb()) {
        auto [compaty_list, fffz_exp] = exp->extract_Text_SEb();

        try {
            auto expanded_list = expand_CompatyList(compaty_list);
            for (const auto& item : expanded_list) {
                children.insert(item);
            }
        } catch (const std::runtime_error&) { }

        children.insert(fffz_exp);
    }

    return children;
}

FFFzExpPtr make_FFFzExp_0() {
    return std::make_shared<FFFzExp>();
}

FFFzExpPtr make_FFFzExp_FEa(SimpleExpPtr se) {
    return std::make_shared<FFFzExp>(se);
}

FFFzExpPtr make_FFFzExp_FEb(SimpleExpPtr se, FFFzExpPtr fe) {
    return std::make_shared<FFFzExp>(se, fe);
}

SimpleExpPtr make_SimpleExp_SEa(FFFzExpPtr fe) {
    return std::make_shared<SimpleExp>(fe);
}

SimpleExpPtr make_SimpleExp_SEb(CompatyListPtr cl, FFFzExpPtr fe) {
    return std::make_shared<SimpleExp>(cl, fe);
}

CompatyListPtr make_CompatyList_CLa(FFFzExpPtr fe) {
    return std::make_shared<CompatyList>(fe);
}

CompatyListPtr make_CompatyList_CLb(CompatyListPtr cl, FFFzExpPtr fe) {
    return std::make_shared<CompatyList>(cl, fe);
}

unsigned int calc_layer(const FFFzExpPtr& exp);
unsigned int calc_layer(const SimpleExpPtr& exp);

unsigned int calc_layer(const FFFzExpPtr& exp) {
    if (exp->is_Text_0()) {
        return 0;
    } else if (exp->is_Text_FEb()) {
        auto first_simple = get_first_FFFzExp(exp);
        return calc_layer(first_simple);
    } else if (exp->is_Text_FEa()) {
        auto simple_exp = exp->extract_Text_FEa();
        return calc_layer(simple_exp);
    } else {
        throw std::runtime_error("Unexpected FFFzExp type in calc_layer(FFFzExpPtr)");
    }
}

unsigned int calc_layer(const SimpleExpPtr& exp) {
    unsigned int max_child_layer = 0;
    bool has_direct_children = false;

    if (exp->is_Text_SEa()) {
        auto contained_fffz = exp->extract_Text_SEa();
        unsigned int child_layer = calc_layer(contained_fffz);
        max_child_layer = std::max(max_child_layer, child_layer);
        has_direct_children = true;
    } else if (exp->is_Text_SEb()) {
        auto [compaty_list, fffz_exp] = exp->extract_Text_SEb();

        unsigned int child_layer_fffz = calc_layer(fffz_exp);
        max_child_layer = std::max(max_child_layer, child_layer_fffz);
        has_direct_children = true;

        auto expanded_list = expand_CompatyList(compaty_list);
        for (const auto& list_item : expanded_list) {
            unsigned int child_layer_list = calc_layer(list_item);
            max_child_layer = std::max(max_child_layer, child_layer_list);
            has_direct_children = true;
        }
    } else {
        throw std::runtime_error("Unexpected SimpleExp type in calc_layer(SimpleExpPtr)");
    }

    if (!has_direct_children) {
        return 1;
    }
    return max_child_layer + 1;
}

SimpleExpPtr get_simple_exp_ptr(const ExprVariant& expr) {
    if (std::holds_alternative<SimpleExpPtr>(expr)) {
        return std::get<SimpleExpPtr>(expr);
    }
    return nullptr;
}

FFFzExpPtr get_fffz_exp_ptr(const ExprVariant& expr) {
    if (std::holds_alternative<FFFzExpPtr>(expr)) {
        return std::get<FFFzExpPtr>(expr);
    }
    return nullptr;
}

bool is_simple_exp(const ExprVariant& expr) {
    return std::holds_alternative<SimpleExpPtr>(expr);
}

bool is_fffz_exp(const ExprVariant& expr) {
    return std::holds_alternative<FFFzExpPtr>(expr);
}

std::set<FFFzExpPtr> get_direct_fffz_children(const SimpleExpPtr& exp) {
    std::set<FFFzExpPtr> children;
    if (exp->is_Text_SEa()) {
        children.insert(exp->extract_Text_SEa());
    } else if (exp->is_Text_SEb()) {
        auto [cl, fffz] = exp->extract_Text_SEb();
        auto list_children = expand_CompatyList(cl);
        children.insert(list_children.begin(), list_children.end());
        children.insert(fffz);
    }
    return children;
}

struct DirectChildrenResult {
    std::set<SimpleExpPtr> simple_children;
    std::set<FFFzExpPtr> fffz_children;
};

DirectChildrenResult get_direct_children(const FFFzExpPtr& exp) {
    DirectChildrenResult result;

    if (exp->is_Text_0()) {
        return result;
    }
    
    if (exp->is_Text_FEa()) {
        result.simple_children.insert(exp->extract_Text_FEa());
    } else if (exp->is_Text_FEb()) {
        auto [simple, next_fffz] = exp->extract_Text_FEb();
        result.simple_children.insert(simple);
        result.fffz_children.insert(next_fffz);
    }
    return result;
}

DirectChildrenResult get_direct_children_from_variant(const ExprVariant& expr) {
    DirectChildrenResult result;
    
    if (is_simple_exp(expr)) {
        auto simple_ptr = get_simple_exp_ptr(expr);
        auto fffz_children = get_direct_fffz_children(simple_ptr);
        result.fffz_children = std::move(fffz_children);
    } else if (is_fffz_exp(expr)) {
        auto fffz_ptr = get_fffz_exp_ptr(expr);
        result = get_direct_children(fffz_ptr);
    }
    
    return result;
}

void* get_raw_pointer(const ExprVariant& expr) {
    if (is_simple_exp(expr)) {
        return get_simple_exp_ptr(expr).get();
    } else {
        return get_fffz_exp_ptr(expr).get();
    }
}

unsigned int calc_relative_layer(const SimpleExpPtr& exp1, const ExprVariant& exp2) {
    if (is_simple_exp(exp2) && get_simple_exp_ptr(exp2) == exp1) {
        return 0;
    }
    
    void* target_ptr = nullptr;
    bool target_is_simple = false;
    
    if (is_simple_exp(exp2)) {
        target_ptr = get_simple_exp_ptr(exp2).get();
        target_is_simple = true;
    } else if (is_fffz_exp(exp2)) {
        auto fffz_ptr = get_fffz_exp_ptr(exp2);
        
        if (fffz_ptr->is_Text_0()) {
            target_ptr = fffz_ptr.get();
            target_is_simple = false;
        } else {
            try {
                SimpleExpPtr last_simple = get_last_FFFzExp(fffz_ptr);
                target_ptr = last_simple.get();
                target_is_simple = true;
            } catch (const std::runtime_error&) {
                target_ptr = fffz_ptr.get();
                target_is_simple = false;
            }
        }
    } else {
        throw std::runtime_error("exp2 must be either SimpleExpPtr or FFFzExpPtr.");
    }
    
    std::queue<ExprVariant> expr_queue;
    std::map<void*, unsigned int> layer_map;
    std::set<void*> visited;
    
    ExprVariant root = exp1;
    layer_map[get_raw_pointer(root)] = 0;
    visited.insert(get_raw_pointer(root));
    expr_queue.push(root);
    
    while (!expr_queue.empty()) {
        auto current = expr_queue.front();
        expr_queue.pop();
        
        void* current_ptr = get_raw_pointer(current);
        unsigned int current_layer = layer_map[current_ptr];
        
        auto children = get_direct_children_from_variant(current);
        
        for (const auto& child_se : children.simple_children) {
            ExprVariant child_variant = child_se;
            void* child_ptr = get_raw_pointer(child_variant);
            
            if (visited.find(child_ptr) == visited.end()) {
                visited.insert(child_ptr);
                layer_map[child_ptr] = current_layer + 1;
                expr_queue.push(child_variant);
            }
        }
        
        for (const auto& child_fe : children.fffz_children) {
            ExprVariant child_variant = child_fe;
            void* child_ptr = get_raw_pointer(child_variant);
            
            if (visited.find(child_ptr) == visited.end()) {
                visited.insert(child_ptr);
                layer_map[child_ptr] = current_layer + 1;
                expr_queue.push(child_variant);
            }
        }
    }
    
    auto it = layer_map.find(target_ptr);
    if (it != layer_map.end()) {
        return it->second;
    } else {
        throw std::runtime_error("exp2 is not contained in exp1 (directly or indirectly).");
    }
}

unsigned int calc_relative_layer(const SimpleExpPtr& exp1, const SimpleExpPtr& exp2) {
    return calc_relative_layer(exp1, ExprVariant(exp2));
}

unsigned int calc_relative_layer(const SimpleExpPtr& exp1, const FFFzExpPtr& exp2) {
    return calc_relative_layer(exp1, ExprVariant(exp2));
}

#endif /* FAKE_BASIS_HPP */
