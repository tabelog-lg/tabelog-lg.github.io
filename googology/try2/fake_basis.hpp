#ifndef FAKE_BASIS_HPP
#define FAKE_BASIS_HPP

#include <iostream>
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
// 定义表达式指针的变体类型
using ExprVariant = std::variant<SimpleExpPtr, FFFzExpPtr>;

// CompatyList 类型
class CompatyList {
public:
    enum class Type { Text_CLa, Text_CLb };

private:
    Type type_;
    FFFzExpPtr cla_content_;         // 用于 Text_CL.a
    CompatyListPtr clb_list_;        // 用于 Text_CL.b
    FFFzExpPtr clb_fffz_;            // 用于 Text_CL.b

public:
    // Text_CL.a: FFFzExp
    explicit CompatyList(FFFzExpPtr exp)
        : type_(Type::Text_CLa), cla_content_(exp) {}

    // Text_CL.b: CompatyList + FFFzExp
    CompatyList(CompatyListPtr list, FFFzExpPtr fffz)
        : type_(Type::Text_CLb), clb_list_(list), clb_fffz_(fffz) {}

    // 类型判断
    bool is_Text_CLa() const { return type_ == Type::Text_CLa; }
    bool is_Text_CLb() const { return type_ == Type::Text_CLb; }

    // 内容提取
    FFFzExpPtr extract_Text_CLa() const {
        if (!is_Text_CLa()) throw std::runtime_error("Not Text_CLa");
        return cla_content_;
    }

    std::pair<CompatyListPtr, FFFzExpPtr> extract_Text_CLb() const {
        if (!is_Text_CLb()) throw std::runtime_error("Not Text_CLb");
        return {clb_list_, clb_fffz_};
    }

    // 声明 to_string 方法
    std::string to_string() const;
};

// SimpleExp 类型
class SimpleExp {
public:
    enum class Type { Text_SEa, Text_SEb };

private:
    Type type_;
    FFFzExpPtr sea_content_;         // 用于 Text_SE.a
    CompatyListPtr seb_list_;        // 用于 Text_SE.b
    FFFzExpPtr seb_fffz_;            // 用于 Text_SE.b

public:
    // Text_SE.a: FFFzExp
    explicit SimpleExp(FFFzExpPtr exp)
        : type_(Type::Text_SEa), sea_content_(exp) {}

    // Text_SE.b: CompatyList + FFFzExp
    SimpleExp(CompatyListPtr list, FFFzExpPtr fffz)
        : type_(Type::Text_SEb), seb_list_(list), seb_fffz_(fffz) {}

    // 类型判断
    bool is_Text_SEa() const { return type_ == Type::Text_SEa; }
    bool is_Text_SEb() const { return type_ == Type::Text_SEb; }

    // 内容提取
    FFFzExpPtr extract_Text_SEa() const {
        if (!is_Text_SEa()) throw std::runtime_error("Not Text_SEa");
        return sea_content_;
    }

    std::pair<CompatyListPtr, FFFzExpPtr> extract_Text_SEb() const {
        if (!is_Text_SEb()) throw std::runtime_error("Not Text_SEb");
        return {seb_list_, seb_fffz_};
    }

    // 声明 to_string 方法
    std::string to_string() const;
};

// FFFzExp 类型
class FFFzExp {
public:
    enum class Type { Text_0, Text_FEa, Text_FEb };

private:
    Type type_;
    SimpleExpPtr fea_content_;      // 用于 Text_FE.a
    SimpleExpPtr feb_simple_;       // 用于 Text_FE.b
    FFFzExpPtr feb_fffz_;           // 用于 Text_FE.b

public:
    // Text_0: 表示0
    FFFzExp() : type_(Type::Text_0) {}

    // Text_FEa: SimpleExp
    explicit FFFzExp(SimpleExpPtr exp)
        : type_(Type::Text_FEa), fea_content_(exp) {}

    // Text_FEb: SimpleExp + FFFzExp
    FFFzExp(SimpleExpPtr simple, FFFzExpPtr fffz)
        : type_(Type::Text_FEb), feb_simple_(simple), feb_fffz_(fffz) {}

    // 类型判断
    bool is_Text_0() const { return type_ == Type::Text_0; }
    bool is_Text_FEa() const { return type_ == Type::Text_FEa; }
    bool is_Text_FEb() const { return type_ == Type::Text_FEb; }

    // 内容提取
    SimpleExpPtr extract_Text_FEa() const {
        if (!is_Text_FEa()) throw std::runtime_error("Not Text_FEa");
        return fea_content_;
    }

    std::pair<SimpleExpPtr, FFFzExpPtr> extract_Text_FEb() const {
        if (!is_Text_FEb()) throw std::runtime_error("Not Text_FEb");
        return {feb_simple_, feb_fffz_};
    }

    // 声明 to_string 方法
    std::string to_string() const;
};

// Transla_Shift 类型
class Transla_Shift {
public:
    enum class Type { Text_TSa, Text_TSb };

private:
    Type type_;
    // Text_TS.a: 一个 FFFzExp
    FFFzExpPtr tsa_content_;
    // Text_TS.b: 三个 Transla_Shift 和三个 FFFzExp
    Transla_ShiftPtr tsb_shift1_;
    Transla_ShiftPtr tsb_shift2_;
    Transla_ShiftPtr tsb_shift3_;
    FFFzExpPtr tsb_fffz1_;
    FFFzExpPtr tsb_fffz2_;
    FFFzExpPtr tsb_fffz3_;

public:
    // Text_TS.a: 一个 FFFzExp
    explicit Transla_Shift(FFFzExpPtr exp)
        : type_(Type::Text_TSa),
          tsa_content_(exp),
          tsb_shift1_(nullptr), tsb_shift2_(nullptr), tsb_shift3_(nullptr),
          tsb_fffz1_(nullptr), tsb_fffz2_(nullptr), tsb_fffz3_(nullptr) {}

    // Text_TS.b: 三个 Transla_Shift 和三个 FFFzExp
    Transla_Shift(Transla_ShiftPtr s1, Transla_ShiftPtr s2, Transla_ShiftPtr s3,
                 FFFzExpPtr f1, FFFzExpPtr f2, FFFzExpPtr f3)
        : type_(Type::Text_TSb),
          tsa_content_(nullptr),
          tsb_shift1_(s1), tsb_shift2_(s2), tsb_shift3_(s3),
          tsb_fffz1_(f1), tsb_fffz2_(f2), tsb_fffz3_(f3) {}

    // 类型判断
    bool is_Text_TSa() const { return type_ == Type::Text_TSa; }
    bool is_Text_TSb() const { return type_ == Type::Text_TSb; }

    // 内容提取
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

// 实现 to_string 方法

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
            return "ψZ(" + sea_content_->to_string() + ")";
        case Type::Text_SEb:
            return "ψZ[" + seb_list_->to_string() + "](" + seb_fffz_->to_string() + ")";
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

// 等长
bool equal_length(const Transla_ShiftPtr& a, const Transla_ShiftPtr& b) {
    return a->get_type() == b->get_type();
}

// 展开复杂表达式
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

// 展开兼容列
std::vector<FFFzExpPtr> expand_CompatyList(const CompatyListPtr& list) {
    std::vector<FFFzExpPtr> result;
    CompatyListPtr current = list;

    while (true) {
        if (current->is_Text_CLa()) {
            result.push_back(current->extract_Text_CLa());
            break;
        } else if (current->is_Text_CLb()) {
            auto [next_list, fffz_exp] = current->extract_Text_CLb();
            result.push_back(fffz_exp);  // 注意：Text_CLb是FFFzExp在前，CompatyList在后
            current = next_list;
        } else {
            throw std::runtime_error("Unexpected CompatyList variant");
        }
    }

    return result;
}

// 构建复杂表达式
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

// 构建兼容列
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

// FFFzExp 的首项
SimpleExpPtr get_first_FFFzExp(const FFFzExpPtr& exp) {
    auto expanded = expand_FFFzExp(exp);
    if (expanded.empty()) {
        throw std::runtime_error("No SimpleExp in FFFzExp");
    }
    return expanded.front();
}

// FFFzExp 的末项
SimpleExpPtr get_last_FFFzExp(const FFFzExpPtr& exp) {
    auto expanded = expand_FFFzExp(exp);
    if (expanded.empty()) {
        throw std::runtime_error("No SimpleExp in FFFzExp");
    }
    return expanded.back();
}

// CompatyList 的首项
FFFzExpPtr get_first_CompatyList(const CompatyListPtr& list) {
    auto expanded = expand_CompatyList(list);
    if (expanded.empty()) {
        throw std::runtime_error("No FFFzExp in CompatyList");
    }
    return expanded.front();
}

// CompatyList 的末项
FFFzExpPtr get_last_CompatyList(const CompatyListPtr& list) {
    auto expanded = expand_CompatyList(list);
    if (expanded.empty()) {
        throw std::runtime_error("No FFFzExp in CompatyList");
    }
    return expanded.back();
}

// 真包含 
bool is_directly_include(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b);
bool is_directly_include(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b);
bool is_directly_include(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b);

// 实现 1: exp_a 为 FFFzExp, exp_b 为 FFFzExp
bool is_directly_include(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b) {
    // 规则1: 如果 exp_a 是 Text_0，抛出错误
    if (exp_a->is_Text_0()) {
        throw std::runtime_error("Error in is_directly_include: exp_a cannot be Text_0.");
    }

    // 规则2: 如果 exp_a 是 Text_FEa
    if (exp_a->is_Text_FEa()) {
        auto simple_in_a = exp_a->extract_Text_FEa();
        // 判断 simple_in_a 这个 SimpleExp 是否直接包含 exp_b 这个 FFFzExp
        return is_directly_include(simple_in_a, exp_b);
    }

    // 规则3: 如果 exp_a 是 Text_FEb
    if (exp_a->is_Text_FEb()) {
        auto expanded = expand_FFFzExp(exp_a); // 得到 std::vector<SimpleExpPtr>
        // 遍历 expanded 中的每个 SimpleExp，检查它们是否直接包含 exp_b
        for (const auto& simple_exp : expanded) {
            if (is_directly_include(simple_exp, exp_b)) {
                return true;
            }
        }
        return false;
    }

    // 如果 exp_a 不是上述任何已知类型，属于未定义行为，抛出异常
    throw std::runtime_error("Error in is_directly_include(FFFzExp, FFFzExp): Unexpected type for exp_a.");
}

// 实现 2: exp_a 为 FFFzExp, exp_b 为 SimpleExp
bool is_directly_include(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b) {
    // 规则1: 如果 exp_a 是 Text_0，抛出错误
    if (exp_a->is_Text_0()) {
        throw std::runtime_error("Error in is_directly_include: exp_a cannot be Text_0.");
    }

    // 规则2: 如果 exp_a 是 Text_FEa
    if (exp_a->is_Text_FEa()) {
        auto simple_in_a = exp_a->extract_Text_FEa();
        // 直接比较两个 SimpleExp 指针是否指向同一对象
        return simple_in_a == exp_b;
    }

    // 规则3: 如果 exp_a 是 Text_FEb
    if (exp_a->is_Text_FEb()) {
        auto expanded = expand_FFFzExp(exp_a);
        // 在 SimpleExp 向量中查找是否有与 exp_b 相同的指针
        for (const auto& simple_exp : expanded) {
            if (simple_exp == exp_b) {
                return true;
            }
        }
        return false;
    }

    // 未定义行为
    throw std::runtime_error("Error in is_directly_include(FFFzExp, SimpleExp): Unexpected type for exp_a.");
}

// 实现 3: exp_a 为 SimpleExp, exp_b 为 FFFzExp
bool is_directly_include(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b) {
    // SimpleExp 没有 Text_0 类型，无需对应规则。

    if (exp_a->is_Text_SEa()) {
        // 规则4: 如果 exp_a 是 Text_SEa
        auto fffz_in_a = exp_a->extract_Text_SEa();
        // 直接比较两个 FFFzExp 指针是否指向同一对象
        return fffz_in_a == exp_b;
    }

    if (exp_a->is_Text_SEb()) {
        // 规则5: 如果 exp_a 是 Text_SEb
        auto [list_in_a, fffz_in_a] = exp_a->extract_Text_SEb();
        // 条件1：检查 exp_b 是否等于 exp_a 中的 FFFzExp
        if (fffz_in_a == exp_b) {
            return true;
        }
        // 条件2：检查 exp_b 是否在 expand_CompatyList(list_in_a) 的元素中
        auto expanded_list = expand_CompatyList(list_in_a);
        for (const auto& fffz_exp : expanded_list) {
            if (fffz_exp == exp_b) {
                return true;
            }
        }
        return false;
    }

    // 未定义行为
    throw std::runtime_error("Error in is_directly_include(SimpleExp, FFFzExp): Unexpected type for exp_a.");
}

// 不提供以下重载
// bool is_directly_include(const SimpleExpPtr& exp_a, const SimpleExpPtr& exp_b);

// 包含
// 包含是真包含的传递闭包 

// 声明函数
bool is_include(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b);
bool is_include(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b);
bool is_include(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b);
bool is_include(const SimpleExpPtr& exp_a, const SimpleExpPtr& exp_b);

// 内部辅助函数声明
bool is_include_impl(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b, std::set<void*>& visited);
bool is_include_impl(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b, std::set<void*>& visited);
bool is_include_impl(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b, std::set<void*>& visited);
bool is_include_impl(const SimpleExpPtr& exp_a, const SimpleExpPtr& exp_b, std::set<void*>& visited);

// 获取子节点
std::set<FFFzExpPtr> get_children(const FFFzExpPtr& exp, std::set<void*>& visited);
std::set<FFFzExpPtr> get_children(const SimpleExpPtr& exp, std::set<void*>& visited);

// 主函数实现
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

// 内部实现：FFFzExp 包含 FFFzExp
bool is_include_impl(const FFFzExpPtr& exp_a, const FFFzExpPtr& exp_b, std::set<void*>& visited) {
    // 避免循环递归
    if (visited.find(exp_a.get()) != visited.end()) {
        return false;
    }
    visited.insert(exp_a.get());

    // 检查直接包含
    try {
        if (is_directly_include(exp_a, exp_b)) {
            return true;
        }
    } catch (const std::runtime_error&) {
        // 忽略错误，继续检查
    }

    // 获取子节点，递归检查
    auto children = get_children(exp_a, visited);
    for (const auto& child : children) {
        if (is_include_impl(child, exp_b, visited)) {
            return true;
        }
    }

    return false;
}

// 内部实现：FFFzExp 包含 SimpleExp
bool is_include_impl(const FFFzExpPtr& exp_a, const SimpleExpPtr& exp_b, std::set<void*>& visited) {
    if (visited.find(exp_a.get()) != visited.end()) {
        return false;
    }
    visited.insert(exp_a.get());

    // 检查直接包含
    try {
        if (is_directly_include(exp_a, exp_b)) {
            return true;
        }
    } catch (const std::runtime_error&) {
        // 忽略错误，继续检查
    }

    // 获取子节点
    auto children = get_children(exp_a, visited);

    // 对于每个 FFFzExp 子节点，递归检查
    for (const auto& child : children) {
        if (is_include_impl(child, exp_b, visited)) {
            return true;
        }
    }

    return false;
}

// 内部实现：SimpleExp 包含 FFFzExp
bool is_include_impl(const SimpleExpPtr& exp_a, const FFFzExpPtr& exp_b, std::set<void*>& visited) {
    if (visited.find(exp_a.get()) != visited.end()) {
        return false;
    }
    visited.insert(exp_a.get());

    // 检查直接包含
    try {
        if (is_directly_include(exp_a, exp_b)) {
            return true;
        }
    } catch (const std::runtime_error&) {
        // 忽略错误，继续检查
    }

    // 获取子节点
    auto children = get_children(exp_a, visited);
    for (const auto& child : children) {
        if (is_include_impl(child, exp_b, visited)) {
            return true;
        }
    }

    return false;
}

// 内部实现：SimpleExp 包含 SimpleExp
bool is_include_impl(const SimpleExpPtr& exp_a, const SimpleExpPtr& exp_b, std::set<void*>& visited) {
    if (visited.find(exp_a.get()) != visited.end()) {
        return false;
    }
    visited.insert(exp_a.get());

    // 检查是否相同
    if (exp_a == exp_b) {
        return true;
    }

    // 获取子节点
    auto children = get_children(exp_a, visited);
    for (const auto& child : children) {
        // 检查每个 FFFzExp 子节点是否包含 exp_b
        if (is_include_impl(child, exp_b, visited)) {
            return true;
        }
    }

    return false;
}

// 获取 FFFzExp 的子节点
std::set<FFFzExpPtr> get_children(const FFFzExpPtr& exp, std::set<void*>& visited) {
    std::set<FFFzExpPtr> children;

    if (exp->is_Text_FEa()) {
        // Text_FEa 包含一个 SimpleExp
        // 我们需要获取这个 SimpleExp 的所有 FFFzExp 子节点
        auto simple_child = exp->extract_Text_FEa();
        if (visited.find(simple_child.get()) == visited.end()) {
            auto simple_children = get_children(simple_child, visited);
            children.insert(simple_children.begin(), simple_children.end());
        }
    }
    else if (exp->is_Text_FEb()) {
        auto [simple_exp, fffz_exp] = exp->extract_Text_FEb();

        // 获取 SimpleExp 的所有 FFFzExp 子节点
        if (visited.find(simple_exp.get()) == visited.end()) {
            auto simple_children = get_children(simple_exp, visited);
            children.insert(simple_children.begin(), simple_children.end());
        }

        // 添加 FFFzExp 子节点
        children.insert(fffz_exp);
    }
    // Text_0 没有子节点

    return children;
}

// 获取 SimpleExp 的子节点
std::set<FFFzExpPtr> get_children(const SimpleExpPtr& exp, std::set<void*>& visited) {
    std::set<FFFzExpPtr> children;

    if (exp->is_Text_SEa()) {
        // Text_SEa 直接包含一个 FFFzExp
        auto fffz_child = exp->extract_Text_SEa();
        children.insert(fffz_child);
    }
    else if (exp->is_Text_SEb()) {
        auto [compaty_list, fffz_exp] = exp->extract_Text_SEb();

        // 添加 CompatyList 展开后的所有 FFFzExp
        try {
            auto expanded_list = expand_CompatyList(compaty_list);
            for (const auto& item : expanded_list) {
                children.insert(item);
            }
        } catch (const std::runtime_error&) {
            // 忽略错误
        }

        // 添加 FFFzExp
        children.insert(fffz_exp);
    }

    return children;
}

// 辅助函数：创建测试用的基本表达式
FFFzExpPtr make_FFFzExp_0() {
    return std::make_shared<FFFzExp>();  // Text_0: "0"
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

// 层（层数） 

unsigned int calc_layer(const FFFzExpPtr& exp);
unsigned int calc_layer(const SimpleExpPtr& exp);

// 计算 FFFzExp 的层数 (重载1)
unsigned int calc_layer(const FFFzExpPtr& exp) {
    if (exp->is_Text_0()) {
        // 若 exp 是 0，则层数 = 0
        return 0;
    } else if (exp->is_Text_FEb()) {
        // 若 exp 是 Text_FEb，则层数 = get_first_FFFzExp(exp) 的层数
        auto first_simple = get_first_FFFzExp(exp);
        // 注意：get_first_FFFzExp 返回的是 SimpleExpPtr，需要计算其层数
        return calc_layer(first_simple);
    } else if (exp->is_Text_FEa()) {
        // 若 exp 是 Text_FEa，则层数 = 它的 SimpleExp 的层数
        auto simple_exp = exp->extract_Text_FEa();
        return calc_layer(simple_exp);
    } else {
        // 理论上不应到达此处，但为安全起见抛出异常
        throw std::runtime_error("Unexpected FFFzExp type in calc_layer(FFFzExpPtr)");
    }
}

// 计算 SimpleExp 的层数 (重载2)
unsigned int calc_layer(const SimpleExpPtr& exp) {
    unsigned int max_child_layer = 0;
    bool has_direct_children = false;

    // 我们需要找到所有被 exp 直接包含的项，并计算它们的层数
    // 由于库没有提供直接获取“所有直接子项”的函数，我们需要根据 exp 的类型来检查

    if (exp->is_Text_SEa()) {
        // Text_SEa: 直接包含一个 FFFzExp
        auto contained_fffz = exp->extract_Text_SEa();
        // 检查这个 FFFzExp 是否被 exp 直接包含 (根据 is_directly_include 逻辑，这总是 true)
        // 但我们还是显式计算其层数
        unsigned int child_layer = calc_layer(contained_fffz);
        max_child_layer = std::max(max_child_layer, child_layer);
        has_direct_children = true;
    } else if (exp->is_Text_SEb()) {
        // Text_SEb: 包含一个 CompatyList 和一个 FFFzExp
        auto [compaty_list, fffz_exp] = exp->extract_Text_SEb();

        // 1. 处理直接包含的 FFFzExp (fffz_exp)
        unsigned int child_layer_fffz = calc_layer(fffz_exp);
        max_child_layer = std::max(max_child_layer, child_layer_fffz);
        has_direct_children = true;

        // 2. 处理 CompatyList 中所有被直接包含的 FFFzExp
        // 根据 is_directly_include 对 Text_SEb 的定义，它直接包含其 CompatyList 展开后的所有元素
        auto expanded_list = expand_CompatyList(compaty_list);
        for (const auto& list_item : expanded_list) {
            // 对于列表中的每个 FFFzExp，计算其层数
            unsigned int child_layer_list = calc_layer(list_item);
            max_child_layer = std::max(max_child_layer, child_layer_list);
            has_direct_children = true;
        }
    } else {
        throw std::runtime_error("Unexpected SimpleExp type in calc_layer(SimpleExpPtr)");
    }

    // 它的层数是所有被它直接包含的项的层数的最大值 + 1
    // 如果没有被直接包含的项，则层数为 1 (0 + 1)？ 但根据定义，SimpleExp 至少包含一个 FFFzExp (在 Text_SEa 或 Text_SEb 中)。
    // 为了逻辑严谨，我们处理 has_direct_children 为 false 的情况（虽然理论上不会发生）。
    if (!has_direct_children) {
        // 如果一个 SimpleExp 不直接包含任何项，根据“最大值+1”的规则，最大值是0，所以层数为1。
        // 但根据 fake_basis 的设计，SimpleExp 总是包含内容的，所以这里可能不会执行。
        return 1;
    }
    return max_child_layer + 1;
}

// 所在层（相对层数）

// 辅助函数：从变体中获取表达式指针
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

// 辅助函数：判断变体类型
bool is_simple_exp(const ExprVariant& expr) {
    return std::holds_alternative<SimpleExpPtr>(expr);
}

bool is_fffz_exp(const ExprVariant& expr) {
    return std::holds_alternative<FFFzExpPtr>(expr);
}

// --- 辅助函数: 获取一个表达式直接包含的所有子表达式（不递归） ---
// 获取 SimpleExp 直接包含的所有 FFFzExp
std::set<FFFzExpPtr> get_direct_fffz_children(const SimpleExpPtr& exp) {
    std::set<FFFzExpPtr> children;
    if (exp->is_Text_SEa()) {
        // Text_SEa: 直接包含一个 FFFzExp
        children.insert(exp->extract_Text_SEa());
    } else if (exp->is_Text_SEb()) {
        auto [cl, fffz] = exp->extract_Text_SEb();
        // Text_SEb: 直接包含一个 CompatyList 和一个 FFFzExp
        // 需要展开 CompatyList
        auto list_children = expand_CompatyList(cl);
        children.insert(list_children.begin(), list_children.end());
        children.insert(fffz);
    }
    return children;
}

// 获取 FFFzExp 直接包含的所有表达式
struct DirectChildrenResult {
    std::set<SimpleExpPtr> simple_children;
    std::set<FFFzExpPtr> fffz_children;
};

DirectChildrenResult get_direct_children(const FFFzExpPtr& exp) {
    DirectChildrenResult result;
    
    // 如果 exp 是 Text_0，它不直接包含任何表达式
    if (exp->is_Text_0()) {
        return result;
    }
    
    if (exp->is_Text_FEa()) {
        // Text_FEa: 直接包含一个 SimpleExp
        result.simple_children.insert(exp->extract_Text_FEa());
    } else if (exp->is_Text_FEb()) {
        // Text_FEb: 直接包含一个 SimpleExp 和 一个 FFFzExp
        auto [simple, next_fffz] = exp->extract_Text_FEb();
        result.simple_children.insert(simple);
        result.fffz_children.insert(next_fffz);
    }
    return result;
}

// 获取任意表达式（以ExprVariant形式）直接包含的子表达式
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

// 获取表达式指针的原始指针地址（用于映射表键）
void* get_raw_pointer(const ExprVariant& expr) {
    if (is_simple_exp(expr)) {
        return get_simple_exp_ptr(expr).get();
    } else {
        return get_fffz_exp_ptr(expr).get();
    }
}

// --- 主函数: calc_relative_layer ---
unsigned int calc_relative_layer(const SimpleExpPtr& exp1, const ExprVariant& exp2) {
    // 特殊情况：如果 exp2 是 SimpleExp 且等于 exp1，则返回 0
    if (is_simple_exp(exp2) && get_simple_exp_ptr(exp2) == exp1) {
        return 0;
    }
    
    // 步骤1: 确定目标表达式
    void* target_ptr = nullptr;
    bool target_is_simple = false;
    
    if (is_simple_exp(exp2)) {
        target_ptr = get_simple_exp_ptr(exp2).get();
        target_is_simple = true;
    } else if (is_fffz_exp(exp2)) {
        auto fffz_ptr = get_fffz_exp_ptr(exp2);
        
        // 处理特殊情况：如果 exp2 是 Text_0，则直接使用其指针
        if (fffz_ptr->is_Text_0()) {
            target_ptr = fffz_ptr.get();
            target_is_simple = false;
        } else {
            // 尝试获取最后一个 SimpleExp
            try {
                SimpleExpPtr last_simple = get_last_FFFzExp(fffz_ptr);
                target_ptr = last_simple.get();
                target_is_simple = true;
            } catch (const std::runtime_error&) {
                // 如果无法获取最后一个 SimpleExp，则使用 FFFzExp 本身
                target_ptr = fffz_ptr.get();
                target_is_simple = false;
            }
        }
    } else {
        throw std::runtime_error("exp2 must be either SimpleExpPtr or FFFzExpPtr.");
    }
    
    // 步骤2: 初始化遍历结构
    std::queue<ExprVariant> expr_queue;        // 待处理表达式队列
    std::map<void*, unsigned int> layer_map;  // 记录每个表达式地址对应的层数
    std::set<void*> visited;                  // 已访问的表达式地址集合
    
    // 初始状态：exp1 作为第0层
    ExprVariant root = exp1;
    layer_map[get_raw_pointer(root)] = 0;
    visited.insert(get_raw_pointer(root));
    expr_queue.push(root);
    
    // 步骤3: 广度优先遍历，构建层数映射
    while (!expr_queue.empty()) {
        auto current = expr_queue.front();
        expr_queue.pop();
        
        void* current_ptr = get_raw_pointer(current);
        unsigned int current_layer = layer_map[current_ptr];
        
        // 获取 current 直接包含的所有子表达式
        auto children = get_direct_children_from_variant(current);
        
        // 处理直接包含的 SimpleExp 子节点
        for (const auto& child_se : children.simple_children) {
            ExprVariant child_variant = child_se;
            void* child_ptr = get_raw_pointer(child_variant);
            
            if (visited.find(child_ptr) == visited.end()) {
                visited.insert(child_ptr);
                layer_map[child_ptr] = current_layer + 1;
                expr_queue.push(child_variant);
            }
        }
        
        // 处理直接包含的 FFFzExp 子节点
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
    
    // 步骤4: 查询目标表达式的层数
    auto it = layer_map.find(target_ptr);
    if (it != layer_map.end()) {
        return it->second;
    } else {
        throw std::runtime_error("exp2 is not contained in exp1 (directly or indirectly).");
    }
}

// --- 为方便使用，提供两个重载接口 ---
unsigned int calc_relative_layer(const SimpleExpPtr& exp1, const SimpleExpPtr& exp2) {
    return calc_relative_layer(exp1, ExprVariant(exp2));
}

unsigned int calc_relative_layer(const SimpleExpPtr& exp1, const FFFzExpPtr& exp2) {
    return calc_relative_layer(exp1, ExprVariant(exp2));
}

#endif /* FAKE_BASIS_HPP */
