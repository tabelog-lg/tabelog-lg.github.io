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
#include <cassert>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <functional>

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

/*

// 测试函数

// 处理过程显示
void process_FFFzExp(FFFzExpPtr a) {
    if (a->is_Text_FEb()) {
        auto [simple_exp, fffz_exp] = a->extract_Text_FEb();
        std::cout << "处理 Text_FEb: " << a->to_string() << "\n";
        std::cout << "  SimpleExp: " << simple_exp->to_string() << "\n";
        std::cout << "  FFFzExp: " << fffz_exp->to_string() << "\n";
    }
    else if (a->is_Text_FEa()) {
        auto simple_exp = a->extract_Text_FEa();
        std::cout << "处理 Text_FEa: " << a->to_string() << "\n";
        std::cout << "  SimpleExp: " << simple_exp->to_string() << "\n";
    }
    else if (a->is_Text_0()) {
        std::cout << "处理 Text_0: " << a->to_string() << "\n";
    }
}

// 测试 FFFzExp, SimpleExp, CompatyList, Transla_Shift, 等长
void test_structure() {
    // 创建原子 FFFzExp (Text_0)
    auto f0 = std::make_shared<FFFzExp>();
    std::cout << "f0: " << f0->to_string() << "\n\n";

    // 创建 CompatyList (Text_CLa)
    auto cl1 = std::make_shared<CompatyList>(f0);
    std::cout << "cl1 (Text_CLa): " << cl1->to_string() << "\n";

    // 创建 CompatyList (Text_CLb)
    auto cl2 = std::make_shared<CompatyList>(cl1, f0);
    std::cout << "cl2 (Text_CLb): " << cl2->to_string() << "\n\n";

    // 创建 SimpleExp (Text_SEa)
    auto se1 = std::make_shared<SimpleExp>(f0);
    std::cout << "se1 (Text_SEa): " << se1->to_string() << "\n";

    // 创建 SimpleExp (Text_SEb)
    auto se2 = std::make_shared<SimpleExp>(cl1, f0);
    std::cout << "se2 (Text_SEb): " << se2->to_string() << "\n\n";

    // 创建 FFFzExp (Text_FEa)
    auto f1 = std::make_shared<FFFzExp>(se1);
    std::cout << "f1 (Text_FEa): " << f1->to_string() << "\n";

    // 创建 FFFzExp (Text_FEb)
    auto f2 = std::make_shared<FFFzExp>(se1, f0);
    std::cout << "f2 (Text_FEb): " << f2->to_string() << "\n\n";

    // 创建更复杂的 FFFzExp (Text_FEb)
    auto f3 = std::make_shared<FFFzExp>(se2, f1);
    std::cout << "f3 (Text_FEb): " << f3->to_string() << "\n";

    // 处理 FFFzExp
    std::cout << "\n处理 f3:\n";
    process_FFFzExp(f3);

    // ================== 新增 Transla_Shift 测试 ==================
    std::cout << "\n===== Transla_Shift 测试 =====" << std::endl;

    // 创建 Text_TSa 实例
    auto tsa1 = std::make_shared<Transla_Shift>(f0);
    auto tsa2 = std::make_shared<Transla_Shift>(f1);

    // 创建 Text_TSb 实例
    auto tsb1 = std::make_shared<Transla_Shift>(
        tsa1, tsa2, tsa1,  // 三个 Transla_Shift
        f0, f1, f2         // 三个 FFFzExp
    );

    // 测试类型判断
    std::cout << "tsa1 is Text_TSa: " << std::boolalpha << tsa1->is_Text_TSa() << std::endl;
    std::cout << "tsa1 is Text_TSb: " << std::boolalpha << tsa1->is_Text_TSb() << std::endl;
    std::cout << "tsb1 is Text_TSa: " << std::boolalpha << tsb1->is_Text_TSa() << std::endl;
    std::cout << "tsb1 is Text_TSb: " << std::boolalpha << tsb1->is_Text_TSb() << std::endl;

    // 测试提取方法
    try {
        auto content_tsa = tsa1->extract_Text_TSa();
        std::cout << "tsa1 content: " << content_tsa->to_string() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error extracting tsa1: " << e.what() << std::endl;
    }

    try {
        auto [s1, s2, s3, f1_ts, f2_ts, f3_ts] = tsb1->extract_Text_TSb();
        std::cout << "tsb1 shifts: "
                  << s1->is_Text_TSa() << ", "
                  << s2->is_Text_TSa() << ", "
                  << s3->is_Text_TSa() << std::endl;
        std::cout << "tsb1 fffzs: "
                  << f1_ts->to_string() << ", "
                  << f2_ts->to_string() << ", "
                  << f3_ts->to_string() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error extracting tsb1: " << e.what() << std::endl;
    }

    // 测试 equal_length 函数
    std::cout << "tsa1 and tsa2 are equal length: "
              << std::boolalpha << equal_length(tsa1, tsa2) << std::endl;
    std::cout << "tsa1 and tsb1 are equal length: "
              << std::boolalpha << equal_length(tsa1, tsb1) << std::endl;
    std::cout << "tsb1 and tsb1 are equal length: "
              << std::boolalpha << equal_length(tsb1, tsb1) << std::endl;
}

*/

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

/*

// 辅助函数：打印测试结果
void print_test_result(const std::string& test_name, bool passed) {
    std::cout << (passed ? "[PASS] " : "[FAIL] ") << test_name << std::endl;
}

// 测试函数
void test_is_include_case_1() {
    std::cout << "=== 测试用例 1: 直接包含 (FFFzExp 包含 SimpleExp) ===" << std::endl;

    // 创建表达式: Text_FEa(Text_SEa(Text_0))
    auto f0 = make_FFFzExp_0();  // "0"
    auto se1 = make_SimpleExp_SEa(f0);  // "Z(0)"
    auto fe1 = make_FFFzExp_FEa(se1);  // "Z(0)"

    // 测试: fe1 应该包含 se1
    bool result = is_include(fe1, se1);
    std::cout << "表达式: " << fe1->to_string() << std::endl;
    std::cout << "是否包含: " << se1->to_string() << " -> " << (result ? "是" : "否") << std::endl;
    std::cout << "期望: 是" << std::endl;
    std::cout << "测试结果: " << (result ? "通过" : "失败") << std::endl << std::endl;
}

void test_is_include_case_2() {
    std::cout << "=== 测试用例 2: 间接包含 (FFFzExp 包含 FFFzExp) ===" << std::endl;

    // 创建表达式: Text_FEa(Text_SEb(Text_CLa(Text_0), Text_0))
    auto f0 = make_FFFzExp_0();  // "0"
    auto cl1 = make_CompatyList_CLa(f0);  // "0"
    auto se1 = make_SimpleExp_SEb(cl1, f0);  // "Z[0](0)"
    auto fe1 = make_FFFzExp_FEa(se1);  // "Z[0](0)"

    // 测试: fe1 应该包含 f0 (通过se1间接包含)
    bool result = is_include(fe1, f0);
    std::cout << "表达式: " << fe1->to_string() << std::endl;
    std::cout << "是否包含: " << f0->to_string() << " -> " << (result ? "是" : "否") << std::endl;
    std::cout << "期望: 是" << std::endl;
    std::cout << "测试结果: " << (result ? "通过" : "失败") << std::endl << std::endl;
}

void test_is_include_case_3() {
    std::cout << "=== 测试用例 3: 复杂嵌套包含 (FFFzExp 包含 SimpleExp) ===" << std::endl;

    // 创建表达式: Text_FEb(Text_SEa(Text_0), Text_FEa(Text_SEb(Text_CLa(Text_0), Text_0)))
    auto f0 = make_FFFzExp_0();  // "0"

    // 第一部分: Text_FEa(Text_SEb(...))
    auto cl1 = make_CompatyList_CLa(f0);  // "0"
    auto se1 = make_SimpleExp_SEb(cl1, f0);  // "Z[0](0)"
    auto fe1 = make_FFFzExp_FEa(se1);  // "Z[0](0)"

    // 第二部分: Text_SEa(Text_0)
    auto se2 = make_SimpleExp_SEa(f0);  // "Z(0)"

    // 组合: Text_FEb(se2, fe1)
    auto fe2 = make_FFFzExp_FEb(se2, fe1);  // "Z(0)+Z[0](0)"

    // 测试: fe2 应该包含 se1
    bool result = is_include(fe2, se1);
    std::cout << "表达式: " << fe2->to_string() << std::endl;
    std::cout << "是否包含: " << se1->to_string() << " -> " << (result ? "是" : "否") << std::endl;
    std::cout << "期望: 是" << std::endl;
    std::cout << "测试结果: " << (result ? "通过" : "失败") << std::endl << std::endl;
}

void test_is_include_case_4() {
    std::cout << "=== 测试用例 4: SimpleExp 包含 FFFzExp ===" << std::endl;

    // 创建表达式: Text_SEb(Text_CLb(Text_CLa(Text_0), Text_0), Text_0)
    auto f0 = make_FFFzExp_0();  // "0"
    auto f1 = make_FFFzExp_0();  // 另一个 "0"

    auto cl1 = make_CompatyList_CLa(f1);  // "0"
    auto cl2 = make_CompatyList_CLb(cl1, f0);  // "0,0"
    auto se1 = make_SimpleExp_SEb(cl2, f0);  // "Z[0,0](0)"

    // 测试: se1 应该包含 f1
    bool result = is_include(se1, f1);
    std::cout << "表达式: " << se1->to_string() << std::endl;
    std::cout << "是否包含: " << f1->to_string() << " -> " << (result ? "是" : "否") << std::endl;
    std::cout << "期望: 是" << std::endl;
    std::cout << "测试结果: " << (result ? "通过" : "失败") << std::endl << std::endl;
}

void test_is_include_case_5() {
    std::cout << "=== 测试用例 5: SimpleExp 包含 SimpleExp (指针相等) ===" << std::endl;

    auto f0 = make_FFFzExp_0();  // "0"
    auto se1 = make_SimpleExp_SEa(f0);  // "Z(0)"
    auto se2 = make_SimpleExp_SEa(f0);  // 另一个 "Z(0)"，但不是同一个对象

    // 测试1: se1 应该包含 se1 自己
    bool result1 = is_include(se1, se1);
    std::cout << "表达式: " << se1->to_string() << std::endl;
    std::cout << "是否包含自己: " << (result1 ? "是" : "否") << std::endl;
    std::cout << "期望: 是" << std::endl;
    std::cout << "测试结果: " << (result1 ? "通过" : "失败") << std::endl;

    // 测试2: se1 不应该包含 se2
    bool result2 = is_include(se1, se2);
    std::cout << "是否包含另一个相等的表达式: " << (result2 ? "是" : "否") << std::endl;
    std::cout << "期望: 否" << std::endl;
    std::cout << "测试结果: " << (!result2 ? "通过" : "失败") << std::endl << std::endl;
}

void test_is_include_case_6() {
    std::cout << "=== 测试用例 6: 不包含的情况 ===" << std::endl;

    auto f0 = make_FFFzExp_0();  // "0"
    auto f1 = make_FFFzExp_0();  // 另一个 "0"
    auto se1 = make_SimpleExp_SEa(f0);  // "Z(0)"
    auto se2 = make_SimpleExp_SEa(f1);  // 另一个 "Z(0)"
    auto fe1 = make_FFFzExp_FEa(se1);  // "Z(0)"

    // 创建一个完全无关的表达式
    auto f2 = make_FFFzExp_0();
    auto se3 = make_SimpleExp_SEa(f2);
    auto fe2 = make_FFFzExp_FEa(se3);

    // 测试1: fe1 不应该包含 se2
    bool result1 = is_include(fe1, se2);
    std::cout << "表达式: " << fe1->to_string() << std::endl;
    std::cout << "是否包含: " << se2->to_string() << " -> " << (result1 ? "是" : "否") << std::endl;
    std::cout << "期望: 否" << std::endl;
    std::cout << "测试结果: " << (!result1 ? "通过" : "失败") << std::endl;

    // 测试2: fe1 不应该包含 fe2
    bool result2 = is_include(fe1, fe2);
    std::cout << "是否包含: " << fe2->to_string() << " -> " << (result2 ? "是" : "否") << std::endl;
    std::cout << "期望: 否" << std::endl;
    std::cout << "测试结果: " << (!result2 ? "通过" : "失败") << std::endl << std::endl;
}

void test_is_include_case_7() {
    std::cout << "=== 测试用例 7: 多层嵌套传递包含 ===" << std::endl;

    // 创建复杂嵌套结构
    auto f0 = make_FFFzExp_0();  // "0"

    // 第三层: Text_SEa(Text_0)
    auto se_leaf = make_SimpleExp_SEa(f0);

    // 第二层: Text_FEa(se_leaf)
    auto fe_middle = make_FFFzExp_FEa(se_leaf);

    // 第二层: Text_SEa(fe_middle)
    auto se_middle = make_SimpleExp_SEa(fe_middle);

    // 第一层: Text_FEa(se_middle)
    auto fe_root = make_FFFzExp_FEa(se_middle);

    // 测试: fe_root 应该包含 f0 (通过多层传递)
    bool result = is_include(fe_root, f0);
    std::cout << "根表达式: " << fe_root->to_string() << std::endl;
    std::cout << "结构: fe_root(se_middle(fe_middle(se_leaf(f0))))" << std::endl;
    std::cout << "是否包含最内层的: " << f0->to_string() << " -> " << (result ? "是" : "否") << std::endl;
    std::cout << "期望: 是" << std::endl;
    std::cout << "测试结果: " << (result ? "通过" : "失败") << std::endl << std::endl;
}

void test_is_include_case_8() {
    std::cout << "=== 测试用例 8: 处理 Text_0 的情况 ===" << std::endl;

    auto f0 = make_FFFzExp_0();  // "0"
    auto f1 = make_FFFzExp_0();  // 另一个 "0"

    // 测试1: f0 不应该包含 f1
    bool result1 = is_include(f0, f1);
    std::cout << "表达式: " << f0->to_string() << std::endl;
    std::cout << "是否包含: " << f1->to_string() << " -> " << (result1 ? "是" : "否") << std::endl;
    std::cout << "期望: 否" << std::endl;
    std::cout << "测试结果: " << (!result1 ? "通过" : "失败") << std::endl;

    // 测试2: 其他表达式不应该包含 Text_0 (除非它是子节点)
    auto se1 = make_SimpleExp_SEa(f0);
    auto fe1 = make_FFFzExp_FEa(se1);

    bool result2 = is_include(fe1, f1);  // f1 不是 f0
    std::cout << "表达式: " << fe1->to_string() << std::endl;
    std::cout << "是否包含另一个: " << f1->to_string() << " -> " << (result2 ? "是" : "否") << std::endl;
    std::cout << "期望: 否" << std::endl;
    std::cout << "测试结果: " << (!result2 ? "通过" : "失败") << std::endl << std::endl;
}

void test_is_include_case_9() {
    std::cout << "=== 测试用例 9: 复杂 CompatyList 包含 ===" << std::endl;

    // 创建复杂列表结构
    auto f0 = make_FFFzExp_0();  // "0"
    auto f1 = make_FFFzExp_0();  // 另一个 "0"
    auto f2 = make_FFFzExp_0();  // 第三个 "0"

    // 创建列表: [f0, f1, f2]
    auto cl1 = make_CompatyList_CLa(f2);  // "0"
    auto cl2 = make_CompatyList_CLb(cl1, f1);  // "0,0"
    auto cl3 = make_CompatyList_CLb(cl2, f0);  // "0,0,0"

    // 创建 SimpleExp: Text_SEb(cl3, f0)
    auto se1 = make_SimpleExp_SEb(cl3, f0);  // "Z[0,0,0](0)"

    // 测试: se1 应该包含 f1
    bool result = is_include(se1, f1);
    std::cout << "表达式: " << se1->to_string() << std::endl;
    std::cout << "是否包含列表中的第二个元素: " << f1->to_string() << " -> " << (result ? "是" : "否") << std::endl;
    std::cout << "期望: 是" << std::endl;
    std::cout << "测试结果: " << (result ? "通过" : "失败") << std::endl << std::endl;
}

int test_is_include() {
    std::cout << "开始测试 is_include 方法..." << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        test_is_include_case_1();
        test_is_include_case_2();
        test_is_include_case_3();
        test_is_include_case_4();
        test_is_include_case_5();
        test_is_include_case_6();
        test_is_include_case_7();
        test_is_include_case_8();
        test_is_include_case_9();

        std::cout << "========================================" << std::endl;
        std::cout << "所有测试用例执行完成！" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }

	return 0;
}

// 辅助函数：打印向量内容
void print_FFFzExp_vector(const std::vector<FFFzExpPtr>& vec, const std::string& name) {
    std::cout << name << " 展开结果 (" << vec.size() << " 个元素):" << std::endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << "  [" << i << "] " << vec[i]->to_string() << std::endl;
    }
}

void print_SimpleExp_vector(const std::vector<SimpleExpPtr>& vec, const std::string& name) {
    std::cout << name << " 展开结果 (" << vec.size() << " 个元素):" << std::endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << "  [" << i << "] " << vec[i]->to_string() << std::endl;
    }
}

// 测试 expand_FFFzExp
void test_expand_FFFzExp() {
    std::cout << "=== 测试 expand_FFFzExp ===" << std::endl;

    // 测试用例 1: Text_FEa
    std::cout << "\n1. 测试 Text_FEa:" << std::endl;
    auto f0 = make_FFFzExp_0();
    auto se1 = make_SimpleExp_SEa(f0);  // Z(0)
    auto fe1 = make_FFFzExp_FEa(se1);   // Z(0)

    try {
        auto expanded = expand_FFFzExp(fe1);
        std::cout << "原始表达式: " << fe1->to_string() << std::endl;
        print_SimpleExp_vector(expanded, "expand_FFFzExp");

        if (expanded.size() == 1 && expanded[0]->to_string() == "ψZ(0)") {
            std::cout << "? 测试通过" << std::endl;
        } else {
            std::cout << "? 测试失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // 测试用例 2: Text_FEb (多层嵌套)
    std::cout << "\n2. 测试 Text_FEb (多层嵌套):" << std::endl;
    auto f1 = make_FFFzExp_0();
    auto f2 = make_FFFzExp_0();
    auto se2 = make_SimpleExp_SEa(f1);  // Z(0)
    auto se3 = make_SimpleExp_SEa(f2);  // Z(0)

    // 构建: se2 + se3 + se2
    auto fe2 = make_FFFzExp_FEa(se2);              // Z(0)
    auto fe3 = make_FFFzExp_FEb(se3, fe2);         // Z(0)+Z(0)
    auto fe4 = make_FFFzExp_FEb(se2, fe3);         // Z(0)+Z(0)+Z(0)

    try {
        auto expanded = expand_FFFzExp(fe4);
        std::cout << "原始表达式: " << fe4->to_string() << std::endl;
        print_SimpleExp_vector(expanded, "expand_FFFzExp");

        if (expanded.size() == 3) {
            std::cout << "? 展开后元素数量正确" << std::endl;

            // 检查顺序是否正确
            bool order_correct = true;
            for (size_t i = 0; i < expanded.size(); ++i) {
                if (expanded[i]->to_string() != "ψZ(0)") {
                    order_correct = false;
                    break;
                }
            }
            if (order_correct) {
                std::cout << "? 展开后顺序正确" << std::endl;
            } else {
                std::cout << "? 展开后顺序错误" << std::endl;
            }
        } else {
            std::cout << "? 展开后元素数量错误，期望 3，实际 " << expanded.size() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // 测试用例 3: Text_0 (应该抛出异常)
    std::cout << "\n3. 测试 Text_0 (应该抛出异常):" << std::endl;
    auto fe0 = make_FFFzExp_0();

    try {
		std::cout << "\n run1\n";
        auto expanded = expand_FFFzExp(fe0);
        std::cout << "? 测试失败: 应该抛出异常但没有" << std::endl;
        print_SimpleExp_vector(expanded, "expand_FFFzExp");
    } catch (const std::runtime_error& e) {
        std::cout << "? 测试通过: 正确抛出异常: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    } catch (...) {
    	std::cerr << "未知类型的异常" << std::endl;
	}

    std::cout << "========================================" << std::endl;
}

// 测试 expand_CompatyList
void test_expand_CompatyList() {
    std::cout << "\n=== 测试 expand_CompatyList ===" << std::endl;

    // 测试用例 1: Text_CLa
    std::cout << "\n1. 测试 Text_CLa:" << std::endl;
    auto f0 = make_FFFzExp_0();
    auto cl1 = make_CompatyList_CLa(f0);

    try {
        auto expanded = expand_CompatyList(cl1);
        std::cout << "原始列表: " << cl1->to_string() << std::endl;
        print_FFFzExp_vector(expanded, "expand_CompatyList");

        if (expanded.size() == 1 && expanded[0]->to_string() == "0") {
            std::cout << "? 测试通过" << std::endl;
        } else {
            std::cout << "? 测试失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // 测试用例 2: Text_CLb (多层嵌套)
    std::cout << "\n2. 测试 Text_CLb (多层嵌套):" << std::endl;
    auto f1 = make_FFFzExp_0();
    auto f2 = make_FFFzExp_0();
    auto f3 = make_FFFzExp_0();

    // 构建: [f1, f2, f3]
    auto cl2 = make_CompatyList_CLa(f3);          // f3
    auto cl3 = make_CompatyList_CLb(cl2, f2);     // f2, f3
    auto cl4 = make_CompatyList_CLb(cl3, f1);     // f1, f2, f3

    try {
        auto expanded = expand_CompatyList(cl4);
        std::cout << "原始列表: " << cl4->to_string() << std::endl;
        print_FFFzExp_vector(expanded, "expand_CompatyList");

        if (expanded.size() == 3) {
            std::cout << "? 展开后元素数量正确" << std::endl;

            // 检查顺序是否正确 (注意: 展开顺序应该是 f1, f2, f3)
            bool order_correct = true;
            for (size_t i = 0; i < expanded.size(); ++i) {
                if (expanded[i]->to_string() != "0") {
                    order_correct = false;
                    break;
                }
            }
            if (order_correct) {
                std::cout << "? 展开后顺序正确" << std::endl;
            } else {
                std::cout << "? 展开后顺序错误" << std::endl;
            }
        } else {
            std::cout << "? 展开后元素数量错误，期望 3，实际 " << expanded.size() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // 测试用例 3: 更复杂的列表
    std::cout << "\n3. 测试复杂列表 (包含不同的 FFFzExp):" << std::endl;
    auto f4 = make_FFFzExp_0();
    auto se4 = make_SimpleExp_SEa(f4);
    auto fe5 = make_FFFzExp_FEa(se4);  // Z(0)

    // 构建: [0, Z(0), 0]
    auto cl5 = make_CompatyList_CLa(f4);          // 0
    auto cl6 = make_CompatyList_CLb(cl5, fe5);    // Z(0), 0
    auto cl7 = make_CompatyList_CLb(cl6, f4);     // 0, Z(0), 0

    try {
        auto expanded = expand_CompatyList(cl7);
        std::cout << "原始列表: " << cl7->to_string() << std::endl;
        print_FFFzExp_vector(expanded, "expand_CompatyList");

        if (expanded.size() == 3) {
            std::cout << "? 展开后元素数量正确" << std::endl;

            // 检查元素是否正确
            if (expanded[0]->to_string() == "0" &&
                expanded[1]->to_string() == "ψZ(0)" &&
                expanded[2]->to_string() == "0") {
                std::cout << "? 展开后元素正确" << std::endl;
            } else {
                std::cout << "? 展开后元素错误" << std::endl;
                for (size_t i = 0; i < expanded.size(); ++i) {
                    std::cout << "  [" << i << "] 实际: " << expanded[i]->to_string() << std::endl;
                }
            }
        } else {
            std::cout << "? 展开后元素数量错误" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    std::cout << "========================================" << std::endl;
}

// 测试 combine_FFFzExp
void test_combine_FFFzExp() {
    std::cout << "\n=== 测试 combine_FFFzExp ===" << std::endl;

    // 测试用例 1: 单个 SimpleExp
    std::cout << "\n1. 测试单个 SimpleExp:" << std::endl;
    auto f0 = make_FFFzExp_0();
    auto se1 = make_SimpleExp_SEa(f0);  // Z(0)

    std::vector<SimpleExpPtr> vec1 = {se1};

    try {
        auto combined = combine_FFFzExp(vec1);
        std::cout << "输入向量: 1 个 SimpleExp" << std::endl;
        std::cout << "  [0] " << se1->to_string() << std::endl;
        std::cout << "组合结果: " << combined->to_string() << std::endl;

        if (combined->is_Text_FEa() && combined->to_string() == "ψZ(0)") {
            std::cout << "? 测试通过" << std::endl;
        } else {
            std::cout << "? 测试失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // 测试用例 2: 多个 SimpleExp
    std::cout << "\n2. 测试多个 SimpleExp:" << std::endl;
    auto f1 = make_FFFzExp_0();
    auto f2 = make_FFFzExp_0();
    auto f3 = make_FFFzExp_0();

    auto se2 = make_SimpleExp_SEa(f1);  // Z(0)
    auto se3 = make_SimpleExp_SEa(f2);  // Z(0)
    auto se4 = make_SimpleExp_SEa(f3);  // Z(0)

    std::vector<SimpleExpPtr> vec2 = {se2, se3, se4};

    try {
        auto combined = combine_FFFzExp(vec2);
        std::cout << "输入向量: 3 个 SimpleExp" << std::endl;
        for (size_t i = 0; i < vec2.size(); ++i) {
            std::cout << "  [" << i << "] " << vec2[i]->to_string() << std::endl;
        }
        std::cout << "组合结果: " << combined->to_string() << std::endl;

        // 预期结果: Z(0)+Z(0)+Z(0)
        if (combined->is_Text_FEb()) {
            std::cout << "? 组合结果为 Text_FEb" << std::endl;

            // 展开验证
            auto expanded = expand_FFFzExp(combined);
            if (expanded.size() == 3) {
                std::cout << "? 展开后元素数量正确" << std::endl;

                bool all_correct = true;
                for (const auto& exp : expanded) {
                    if (exp->to_string() != "ψZ(0)") {
                        all_correct = false;
                        break;
                    }
                }
                if (all_correct) {
                    std::cout << "? 展开后元素内容正确" << std::endl;
                } else {
                    std::cout << "? 展开后元素内容错误" << std::endl;
                }
            } else {
                std::cout << "? 展开后元素数量错误" << std::endl;
            }
        } else {
            std::cout << "? 组合结果类型错误" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // 测试用例 3: 空向量 (应该抛出异常)
    std::cout << "\n3. 测试空向量 (应该抛出异常):" << std::endl;
    std::vector<SimpleExpPtr> vec3;

    try {
        auto combined = combine_FFFzExp(vec3);
        std::cout << "? 测试失败: 应该抛出异常但没有" << std::endl;
        std::cout << "组合结果: " << combined->to_string() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "? 测试通过: 正确抛出异常: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // 测试用例 4: 组合展开的逆操作
    std::cout << "\n4. 测试组合与展开的逆操作:" << std::endl;
    auto f4 = make_FFFzExp_0();
    auto se5 = make_SimpleExp_SEa(f4);  // Z(0)
    auto se6 = make_SimpleExp_SEb(make_CompatyList_CLa(f4), f4);  // Z[0](0)

    std::vector<SimpleExpPtr> vec4 = {se5, se6};

    try {
        auto combined = combine_FFFzExp(vec4);
        std::cout << "输入向量:" << std::endl;
        std::cout << "  [0] " << se5->to_string() << std::endl;
        std::cout << "  [1] " << se6->to_string() << std::endl;
        std::cout << "组合结果: " << combined->to_string() << std::endl;

        auto expanded = expand_FFFzExp(combined);
        std::cout << "重新展开结果:" << std::endl;
        print_SimpleExp_vector(expanded, "重新展开");

        if (expanded.size() == 2 &&
            expanded[0]->to_string() == "ψZ(0)" &&
            expanded[1]->to_string() == "ψZ[0](0)") {
            std::cout << "? 组合展开逆操作正确" << std::endl;
        } else {
            std::cout << "? 组合展开逆操作错误" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    std::cout << "========================================" << std::endl;
}

// 测试 combine_CompatyList
void test_combine_CompatyList() {
    std::cout << "\n=== 测试 combine_CompatyList ===" << std::endl;

    // 测试用例 1: 单个 FFFzExp
    std::cout << "\n1. 测试单个 FFFzExp:" << std::endl;
    auto f0 = make_FFFzExp_0();

    std::vector<FFFzExpPtr> vec1 = {f0};

    try {
        auto combined = combine_CompatyList(vec1);
        std::cout << "输入向量: 1 个 FFFzExp" << std::endl;
        std::cout << "  [0] " << f0->to_string() << std::endl;
        std::cout << "组合结果: " << combined->to_string() << std::endl;

        if (combined->is_Text_CLa() && combined->to_string() == "0") {
            std::cout << "? 测试通过" << std::endl;
        } else {
            std::cout << "? 测试失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // 测试用例 2: 多个 FFFzExp
    std::cout << "\n2. 测试多个 FFFzExp:" << std::endl;
    auto f1 = make_FFFzExp_0();
    auto se1 = make_SimpleExp_SEa(f1);
    auto fe1 = make_FFFzExp_FEa(se1);  // Z(0)
    auto f2 = make_FFFzExp_0();

    std::vector<FFFzExpPtr> vec2 = {f1, fe1, f2};  // 0, Z(0), 0

    try {
        auto combined = combine_CompatyList(vec2);
        std::cout << "输入向量: 3 个 FFFzExp" << std::endl;
        for (size_t i = 0; i < vec2.size(); ++i) {
            std::cout << "  [" << i << "] " << vec2[i]->to_string() << std::endl;
        }
        std::cout << "组合结果: " << combined->to_string() << std::endl;

        // 预期结果: 0,Z(0),0
        if (combined->is_Text_CLb()) {
            std::cout << "? 组合结果为 Text_CLb" << std::endl;

            // 展开验证
            auto expanded = expand_CompatyList(combined);
            if (expanded.size() == 3) {
                std::cout << "? 展开后元素数量正确" << std::endl;

                if (expanded[0]->to_string() == "0" &&
                    expanded[1]->to_string() == "ψZ(0)" &&
                    expanded[2]->to_string() == "0") {
                    std::cout << "? 展开后元素内容正确" << std::endl;
                } else {
                    std::cout << "? 展开后元素内容错误" << std::endl;
                    print_FFFzExp_vector(expanded, "展开结果");
                }
            } else {
                std::cout << "? 展开后元素数量错误" << std::endl;
            }
        } else {
            std::cout << "? 组合结果类型错误" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // 测试用例 3: 空向量 (应该抛出异常)
    std::cout << "\n3. 测试空向量 (应该抛出异常):" << std::endl;
    std::vector<FFFzExpPtr> vec3;

    try {
        auto combined = combine_CompatyList(vec3);
        std::cout << "? 测试失败: 应该抛出异常但没有" << std::endl;
        std::cout << "组合结果: " << combined->to_string() << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "? 测试通过: 正确抛出异常: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // 测试用例 4: 组合展开的逆操作
    std::cout << "\n4. 测试组合与展开的逆操作:" << std::endl;
    auto f3 = make_FFFzExp_0();
    auto f4 = make_FFFzExp_0();
    auto se2 = make_SimpleExp_SEa(f4);
    auto fe2 = make_FFFzExp_FEa(se2);
    auto se3 = make_SimpleExp_SEb(make_CompatyList_CLa(f4), f4);
    auto fe3 = make_FFFzExp_FEa(se3);

    std::vector<FFFzExpPtr> vec4 = {f3, fe2, fe3};  // 0, Z(0), Z[0](0)

    try {
        auto combined = combine_CompatyList(vec4);
        std::cout << "输入向量:" << std::endl;
        for (size_t i = 0; i < vec4.size(); ++i) {
            std::cout << "  [" << i << "] " << vec4[i]->to_string() << std::endl;
        }
        std::cout << "组合结果: " << combined->to_string() << std::endl;

        auto expanded = expand_CompatyList(combined);
        std::cout << "重新展开结果:" << std::endl;
        print_FFFzExp_vector(expanded, "重新展开");

        if (expanded.size() == 3 &&
            expanded[0]->to_string() == "0" &&
            expanded[1]->to_string() == "ψZ(0)" &&
            expanded[2]->to_string() == "ψZ[0](0)") {
            std::cout << "? 组合展开逆操作正确" << std::endl;
        } else {
            std::cout << "? 组合展开逆操作错误" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    std::cout << "========================================" << std::endl;
}

int main() {
    std::cout << "开始测试 expand 和 combine 方法..." << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        test_expand_FFFzExp();
        test_expand_CompatyList();
        test_combine_FFFzExp();
        test_combine_CompatyList();

        std::cout << "\n========================================" << std::endl;
        std::cout << "所有测试用例执行完成！" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
    	return 2;
	}

    return 0;
}

*/

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

/*

// 辅助函数：获取 SimpleExp 的所有直接子节点（FFFzExp）
std::vector<FFFzExpPtr> get_direct_children(const SimpleExpPtr& exp) {
    std::vector<FFFzExpPtr> children;
    
    if (exp->is_Text_SEa()) {
        // Text_SEa: 直接包含一个 FFFzExp
        auto contained_fffz = exp->extract_Text_SEa();
        children.push_back(contained_fffz);
    } else if (exp->is_Text_SEb()) {
        // Text_SEb: 包含一个 CompatyList 和一个 FFFzExp
        auto [compaty_list, fffz_exp] = exp->extract_Text_SEb();
        
        // 1. 处理直接包含的 FFFzExp
        children.push_back(fffz_exp);
        
        // 2. 处理 CompatyList 中的所有 FFFzExp
        auto expanded_list = expand_CompatyList(compaty_list);
        for (const auto& list_item : expanded_list) {
            children.push_back(list_item);
        }
    }
    
    return children;
}

// 辅助函数：获取 FFFzExp 的最后一个 SimpleExp
SimpleExpPtr get_last_simple(const FFFzExpPtr& exp) {
    if (exp->is_Text_0()) {
        throw std::runtime_error("Cannot get last simple of Text_0");
    }
    return get_last_FFFzExp(exp);
}

// 计算相对层数的主函数 (BFS实现)
unsigned int calc_relative_layer(const SimpleExpPtr& exp1, const SimpleExpPtr& exp2) {
    // 特殊情况：如果 exp1 和 exp2 是同一个表达式
    if (exp1 == exp2) {
        throw std::runtime_error("exp2 is not contained in exp1 (or its descendants).");
    }
    
    // 使用BFS搜索最短路径
    std::queue<std::pair<SimpleExpPtr, unsigned int>> bfs_queue; // (节点, 当前层数)
    std::set<void*> visited;
    
    // 从 exp1 开始，层数为0
    bfs_queue.push({exp1, 0});
    visited.insert(exp1.get());
    
    while (!bfs_queue.empty()) {
        auto [current, current_depth] = bfs_queue.front();
        bfs_queue.pop();
        
        // 获取当前节点的所有直接子节点（FFFzExp）
        auto direct_children = get_direct_children(current);
        
        for (const auto& child_fffz : direct_children) {
            if (child_fffz->is_Text_0()) {
                continue; // 跳过Text_0
            }
            
            // 获取子节点的最后一个 SimpleExp
            SimpleExpPtr child_simple;
            try {
                child_simple = get_last_simple(child_fffz);
            } catch (const std::runtime_error&) {
                continue; // 如果获取失败，跳过这个子节点
            }
            
            // 检查是否找到目标
            if (child_simple == exp2) {
                return current_depth + 1;
            }
            
            // 如果还没有访问过这个子节点，加入队列
            if (visited.find(child_simple.get()) == visited.end()) {
                visited.insert(child_simple.get());
                bfs_queue.push({child_simple, current_depth + 1});
            }
        }
    }
    
    // 如果没有找到路径
    throw std::runtime_error("exp2 is not contained in exp1 (or its descendants).");
}

unsigned int calc_relative_layer(const SimpleExpPtr& exp1, const FFFzExpPtr& exp2) {
    if (exp2->is_Text_0()) {
        throw std::runtime_error("Error in calc_relative_layer: exp2 cannot be Text_0.");
    }
    
    // 若 exp2 为 FFFzExp，则 exp2 的相对层数等于 get_last_FFFzExp(exp2) 的相对层数
    auto last_simple_of_exp2 = get_last_FFFzExp(exp2);
    return calc_relative_layer(exp1, last_simple_of_exp2);
}

*/

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

/*

void test_calc_layer_basic() {
    std::cout << "=== 测试 calc_layer 基本功能 ===" << std::endl;
    
    // 测试 1: Text_0 的层数应为 0
    auto zero = make_FFFzExp_0();
    unsigned int layer_zero = calc_layer(zero);
    std::cout << "1. Text_0 的层数: " << layer_zero;
    assert(layer_zero == 0 && "Text_0 的层数应为 0");
    std::cout << " ?" << std::endl;
    
    // 测试 2: 构造 ψZ(0) 并测试其层数
    auto psi_z_0 = make_SimpleExp_SEa(zero);
    auto fffz_psi_z_0 = make_FFFzExp_FEa(psi_z_0);
    unsigned int layer_psi_z_0 = calc_layer(fffz_psi_z_0);
    std::cout << "2. ψZ(0) 的层数: " << layer_psi_z_0;
    // ψZ(0) 中，SimpleExp 直接包含 0，0 的层数是 0，所以 ψZ(0) 的层数是 0+1=1
    // 通过 FFFzExp 计算时，Text_FEa 返回其 SimpleExp 的层数，也是 1
    assert(layer_psi_z_0 == 1 && "ψZ(0) 的层数应为 1");
    std::cout << " ?" << std::endl;
    
    // 测试 3: 构造 ψZ[0](0) 并测试其层数
    auto cl_0 = make_CompatyList_CLa(zero);
    auto psi_z_array_0 = make_SimpleExp_SEb(cl_0, zero);
    auto fffz_psi_z_array_0 = make_FFFzExp_FEa(psi_z_array_0);
    unsigned int layer_psi_z_array_0 = calc_layer(fffz_psi_z_array_0);
    std::cout << "3. ψZ[0](0) 的层数: " << layer_psi_z_array_0;
    // ψZ[0](0) 直接包含两个 0，最大层数是 0，所以层数是 0+1=1
    assert(layer_psi_z_array_0 == 1 && "ψZ[0](0) 的层数应为 1");
    std::cout << " ?" << std::endl;
    
    // 测试 4: 构造 ψZ[ψZ(0),0](0) 并测试其层数
    auto cl_inner = make_CompatyList_CLa(fffz_psi_z_0);
    auto cl_outer = make_CompatyList_CLb(cl_inner, zero);
    auto psi_z_complex = make_SimpleExp_SEb(cl_outer, zero);
    auto fffz_psi_z_complex = make_FFFzExp_FEa(psi_z_complex);
    unsigned int layer_psi_z_complex = calc_layer(fffz_psi_z_complex);
    std::cout << "4. ψZ[ψZ(0),0](0) 的层数: " << layer_psi_z_complex;
    // 它直接包含 ψZ(0) 和 0，ψZ(0) 的层数是 1，0 的层数是 0，最大值是 1，所以层数是 1+1=2
    assert(layer_psi_z_complex == 2 && "ψZ[ψZ(0),0](0) 的层数应为 2");
    std::cout << " ?" << std::endl;
    
    // 测试 5: 测试 Text_FEb
    auto fffz_feb = make_FFFzExp_FEb(psi_z_0, fffz_psi_z_array_0);
    unsigned int layer_feb = calc_layer(fffz_feb);
    std::cout << "5. ψZ(0)+ψZ[0](0) 的层数: " << layer_feb;
    // Text_FEb 的层数等于其第一个 SimpleExp 的层数，即 ψZ(0) 的层数 1
    assert(layer_feb == 1 && "ψZ(0)+ψZ[0](0) 的层数应为 1");
    std::cout << " ?" << std::endl;
    
    std::cout << "calc_layer 基本功能测试通过！" << std::endl << std::endl;
}

void test_calc_relative_layer_basic() {
    std::cout << "=== 测试 calc_relative_layer 基本功能 ===" << std::endl;
    
    // 构造测试表达式
    auto zero = make_FFFzExp_0();
    auto psi_z_0 = make_SimpleExp_SEa(zero);
    auto fffz_psi_z_0 = make_FFFzExp_FEa(psi_z_0);
    
    auto cl_inner = make_CompatyList_CLa(fffz_psi_z_0);
    auto cl_outer = make_CompatyList_CLb(cl_inner, zero);
    auto psi_z_complex = make_SimpleExp_SEb(cl_outer, zero);
    auto fffz_psi_z_complex = make_FFFzExp_FEa(psi_z_complex);
    
    // 测试 1: 测试 ψZ(0) 在 ψZ[ψZ(0),0](0) 中的相对层数
    unsigned int relative_layer1 = calc_relative_layer(psi_z_complex, psi_z_0);
    std::cout << "1. ψZ(0) 在 ψZ[ψZ(0),0](0) 中的相对层数: " << relative_layer1;
    // ψZ(0) 被 ψZ[ψZ(0),0](0) 直接包含，所以层数应为 1
    // assert(relative_layer1 == 1 && "ψZ(0) 在 ψZ[ψZ(0),0](0) 中的相对层数应为 1");
    std::cout << " ?" << std::endl;
    
    // 测试 2: 测试 0 在 ψZ[ψZ(0),0](0) 中的相对层数
    auto simple_zero = make_SimpleExp_SEa(zero);
    unsigned int relative_layer2 = calc_relative_layer(psi_z_complex, simple_zero);
    std::cout << "2. 0 在 ψZ[ψZ(0),0](0) 中的相对层数: " << relative_layer2;
    // 0 被 ψZ[ψZ(0),0](0) 直接包含，所以层数应为 1
    assert(relative_layer2 == 1 && "0 在 ψZ[ψZ(0),0](0) 中的相对层数应为 1");
    std::cout << " ?" << std::endl;
    
    // 测试 3: 测试 ψZ(0) 在 ψZ[ψZ(0),0](0) 中作为 FFFzExp 的相对层数
    unsigned int relative_layer3 = calc_relative_layer(psi_z_complex, fffz_psi_z_0);
    std::cout << "3. ψZ(0) 在 ψZ[ψZ(0),0](0) 中的相对层数(FFFzExp): " << relative_layer3;
    // ψZ(0) 被 ψZ[ψZ(0),0](0) 直接包含，所以层数应为 1
    assert(relative_layer3 == 1 && "ψZ(0) 在 ψZ[ψZ(0),0](0) 中的相对层数(FFFzExp)应为 1");
    std::cout << " ?" << std::endl;
    
    // 构造一个更复杂的表达式测试嵌套包含
    // ψZ[ψZ[0](0),0](0)
    auto cl_0 = make_CompatyList_CLa(zero);
    auto psi_z_array_0 = make_SimpleExp_SEb(cl_0, zero);
    auto fffz_psi_z_array_0 = make_FFFzExp_FEa(psi_z_array_0);
    
    auto cl_nested_inner = make_CompatyList_CLa(fffz_psi_z_array_0);
    auto cl_nested_outer = make_CompatyList_CLb(cl_nested_inner, zero);
    auto psi_z_nested = make_SimpleExp_SEb(cl_nested_outer, zero);
    
    // 测试 4: 测试 0 在 ψZ[ψZ[0](0),0](0) 中的相对层数
    unsigned int relative_layer4 = calc_relative_layer(psi_z_nested, simple_zero);
    std::cout << "4. 0 在 ψZ[ψZ[0](0),0](0) 中的相对层数: " << relative_layer4;
    // 0 被 ψZ[ψZ[0](0),0](0) 直接包含，所以层数应为 1
    assert(relative_layer4 == 1 && "0 在 ψZ[ψZ[0](0),0](0) 中的相对层数应为 1");
    std::cout << " ?" << std::endl;
    
    std::cout << "calc_relative_layer 基本功能测试通过！" << std::endl << std::endl;
}

void test_calc_relative_layer_exceptions() {
    std::cout << "=== 测试 calc_relative_layer 异常情况 ===" << std::endl;
    
    // 构造测试表达式
    auto zero = make_FFFzExp_0();
    auto psi_z_0 = make_SimpleExp_SEa(zero);
    
    // 测试 1: 测试 Text_0 作为 exp2
    std::cout << "1. 测试 Text_0 作为 exp2 的异常: ";
    try {
        calc_relative_layer(psi_z_0, zero);
        std::cout << "? 应该抛出异常但没有抛出" << std::endl;
        assert(false && "应该抛出异常但没有抛出");
    } catch (const std::runtime_error& e) {
        std::string msg = e.what();
        if (msg.find("exp2 cannot be Text_0") != std::string::npos ||
            msg.find("Error in calc_relative_layer") != std::string::npos) {
            std::cout << "? 正确抛出异常: " << e.what() << std::endl;
        } else {
            std::cout << "? 抛出异常但消息不正确: " << e.what() << std::endl;
            assert(false && "异常消息不正确");
        }
    }
    
    // 测试 2: 测试 exp2 不被 exp1 包含的情况
    std::cout << "2. 测试 exp2 不被 exp1 包含的异常: ";
    auto other_simple = make_SimpleExp_SEa(zero);
    try {
        calc_relative_layer(psi_z_0, other_simple);
        std::cout << "? 应该抛出异常但没有抛出" << std::endl;
        assert(false && "应该抛出异常但没有抛出");
    } catch (const std::runtime_error& e) {
        std::string msg = e.what();
        if (msg.find("exp2 is not contained in exp1") != std::string::npos) {
            std::cout << "? 正确抛出异常: " << e.what() << std::endl;
        } else {
            std::cout << "? 抛出异常但消息不正确: " << e.what() << std::endl;
            assert(false && "异常消息不正确");
        }
    }
    
    // 测试 3: 测试 exp2 等于 exp1 的情况
    std::cout << "3. 测试 exp2 等于 exp1 的异常: ";
    try {
        calc_relative_layer(psi_z_0, psi_z_0);
        std::cout << "? 应该抛出异常但没有抛出" << std::endl;
        assert(false && "应该抛出异常但没有抛出");
    } catch (const std::runtime_error& e) {
        std::string msg = e.what();
        if (msg.find("exp2 is not contained in exp1") != std::string::npos) {
            std::cout << "? 正确抛出异常: " << e.what() << std::endl;
        } else {
            std::cout << "? 抛出异常但消息不正确: " << e.what() << std::endl;
            assert(false && "异常消息不正确");
        }
    }
    
    std::cout << "calc_relative_layer 异常测试通过！" << std::endl << std::endl;
}

void test_calc_layer_exceptions() {
    std::cout << "=== 测试 calc_layer 特殊情况 ===" << std::endl;
    
    // 测试 calc_layer 对 SimpleExp 的处理
    auto zero = make_FFFzExp_0();
    
    // 测试 SimpleExp 直接包含 0 的情况
    auto simple_with_zero = make_SimpleExp_SEa(zero);
    unsigned int layer = calc_layer(simple_with_zero);
    std::cout << "1. SimpleExp 直接包含 0 的层数: " << layer;
    assert(layer == 1 && "SimpleExp 直接包含 0 的层数应为 1");
    std::cout << " ?" << std::endl;
    
    std::cout << "calc_layer 特殊情况测试通过！" << std::endl << std::endl;
}

void test_complex_cases() {
    std::cout << "=== 测试复杂情况 ===" << std::endl;
    
    // 构建复杂表达式树进行测试
    auto zero = make_FFFzExp_0();
    
    // 构造 ψZ(0)
    auto psi_z_0 = make_SimpleExp_SEa(zero);
    auto fffz_psi_z_0 = make_FFFzExp_FEa(psi_z_0);
    
    // 构造 ψZ[ψZ(0)](ψZ(0))
    auto cl_psi = make_CompatyList_CLa(fffz_psi_z_0);
    auto psi_z_in_psi = make_SimpleExp_SEb(cl_psi, fffz_psi_z_0);
    auto fffz_psi_z_in_psi = make_FFFzExp_FEa(psi_z_in_psi);
    
    // 测试层数
    unsigned int layer1 = calc_layer(fffz_psi_z_in_psi);
    std::cout << "1. ψZ[ψZ(0)](ψZ(0)) 的层数: " << layer1;
    // 它直接包含两个 ψZ(0)，ψZ(0) 的层数是 1，所以层数是 1+1=2
    assert(layer1 == 2 && "ψZ[ψZ(0)](ψZ(0)) 的层数应为 2");
    std::cout << " ?" << std::endl;
    
    // 测试相对层数
    unsigned int relative_layer1 = calc_relative_layer(psi_z_in_psi, psi_z_0);
    std::cout << "2. ψZ(0) 在 ψZ[ψZ(0)](ψZ(0)) 中的相对层数: " << relative_layer1;
    // ψZ(0) 被 ψZ[ψZ(0)](ψZ(0)) 直接包含（在列表中），所以层数应为 1
    assert(relative_layer1 == 1 && "ψZ(0) 在 ψZ[ψZ(0)](ψZ(0)) 中的相对层数应为 1");
    std::cout << " ?" << std::endl;
    
    // 构造 ψZ[ψZ[0](0)](0)
    auto cl_0 = make_CompatyList_CLa(zero);
    auto psi_z_array_0 = make_SimpleExp_SEb(cl_0, zero);
    auto fffz_psi_z_array_0 = make_FFFzExp_FEa(psi_z_array_0);
    
    auto cl_psi_array = make_CompatyList_CLa(fffz_psi_z_array_0);
    auto psi_z_array_in_zero = make_SimpleExp_SEb(cl_psi_array, zero);
    
    // 测试相对层数
    auto simple_zero = make_SimpleExp_SEa(zero);
    unsigned int relative_layer2 = calc_relative_layer(psi_z_array_in_zero, simple_zero);
    std::cout << "3. 0 在 ψZ[ψZ[0](0)](0) 中的相对层数: " << relative_layer2;
    // 0 被 ψZ[ψZ[0](0)](0) 直接包含，所以层数应为 1
    assert(relative_layer2 == 1 && "0 在 ψZ[ψZ[0](0)](0) 中的相对层数应为 1");
    std::cout << " ?" << std::endl;
    
    std::cout << "复杂情况测试通过！" << std::endl << std::endl;
}

int main() {
    std::cout << "开始测试 fake_basis 库的 calc_layer 和 calc_relative_layer 函数" << std::endl;
    std::cout << "================================================" << std::endl << std::endl;
    
    try {
        test_calc_layer_basic();
        test_calc_layer_exceptions();
        test_calc_relative_layer_basic();
        test_calc_relative_layer_exceptions();
        test_complex_cases();
        
        std::cout << "================================================" << std::endl;
        std::cout << "所有测试通过！?" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "未知错误导致测试失败" << std::endl;
        return 1;
    }
}

*/

/*

void test_calc_relative_layer() {
    std::cout << "=== 开始测试 calc_relative_layer ===" << std::endl;
    int passed = 0;
    int total = 0;
    
    auto test_case = [&](const std::string& name, bool condition) {
        total++;
        if (condition) {
            std::cout << "? 测试通过: " << name << std::endl;
            passed++;
        } else {
            std::cout << "? 测试失败: " << name << std::endl;
        }
    };
    
    try {
        // --- 测试用例 1: 简单直接包含 ---
        {
            // 构造: exp1 = Z[0](0)，其中包含一个SimpleExp
            auto leaf1 = make_FFFzExp_0();  // Text_0
            auto leaf2 = make_FFFzExp_0();  // Text_0
            auto inner_simple = make_SimpleExp_SEa(leaf2);
            auto fffz_exp = make_FFFzExp_FEa(inner_simple);
            
            auto list1 = make_CompatyList_CLa(fffz_exp);
            auto exp1 = make_SimpleExp_SEb(list1, leaf1);
            
            // 测试1: inner_simple 的层数应该是2
            unsigned int layer = calc_relative_layer(exp1, inner_simple);
            test_case("测试用例1-1: 间接包含的SimpleExp", layer == 2);
            
            // 测试2: leaf1 的层数应该是1 (Text_0 的直接层数)
            unsigned int layer2 = calc_relative_layer(exp1, leaf1);
            test_case("测试用例1-2: 直接包含的Text_0", layer2 == 1);
            
            // 测试3: fffz_exp 的层数应该是1
            unsigned int layer3 = calc_relative_layer(exp1, fffz_exp);
            test_case("测试用例1-3: 直接包含的FFFzExp(包含SimpleExp)", layer3 == 1);
        }
        
        // --- 测试用例 2: 自包含情况 ---
        {
            auto a = make_FFFzExp_0();
            auto simple_a = make_SimpleExp_SEa(a);
            
            // 自包含：simple_a 在 simple_a 中应该是第0层
            unsigned int layer = calc_relative_layer(simple_a, simple_a);
            test_case("测试用例2-1: SimpleExp自包含", layer == 0);
            
            // a 在 simple_a 中应该是第1层
            unsigned int layer2 = calc_relative_layer(simple_a, a);
            test_case("测试用例2-2: 直接包含的Text_0自包含", layer2 == 1);
        }
        
        // --- 测试用例 3: Text_0 特殊处理 ---
        {
            // 构造: exp1 = Z(0)
            auto leaf1 = make_FFFzExp_0();
            auto exp1 = make_SimpleExp_SEa(leaf1);
            
            // 测试: leaf1 是 Text_0，在 exp1 中应该是第1层
            unsigned int layer = calc_relative_layer(exp1, leaf1);
            test_case("测试用例3: Text_0在Z(0)中的层数", layer == 1);
        }
        
        // --- 测试用例 4: 表达式不包含目标 ---
        {
            auto a = make_FFFzExp_0();
            auto b = make_FFFzExp_0();
            
            auto simple_a = make_SimpleExp_SEa(a);
            auto simple_b = make_SimpleExp_SEa(b);
            
            auto exp1 = simple_a;  // Z(0)
            
            bool exception_thrown = false;
            try {
                calc_relative_layer(exp1, simple_b);  // simple_b不在exp1中
            } catch (const std::runtime_error&) {
                exception_thrown = true;
            }
            
            test_case("测试用例4: 不包含的表达式抛出异常", exception_thrown);
        }
        
        // --- 测试用例 5: 复杂嵌套结构中的Text_0 ---
        {
            // 构造: exp1 = Z[Z[0](0)](0)
            auto leaf1 = make_FFFzExp_0();  // 最外层
            auto leaf2 = make_FFFzExp_0();  // 中间层
            auto leaf3 = make_FFFzExp_0();  // 最内层
            
            // 最内层: Z(0)
            auto inner_simple = make_SimpleExp_SEa(leaf3);
            auto inner_fffz = make_FFFzExp_FEa(inner_simple);
            
            // 中间层列表: [Z(0)]
            auto middle_list = make_CompatyList_CLa(inner_fffz);
            // 中间层表达式: Z[Z(0)](0)
            auto middle_simple = make_SimpleExp_SEb(middle_list, leaf2);
            auto middle_fffz = make_FFFzExp_FEa(middle_simple);
            
            // 外层列表: [Z[Z(0)](0)]
            auto outer_list = make_CompatyList_CLa(middle_fffz);
            // 外层表达式: exp1 = Z[Z[Z(0)](0)](0)
            auto exp1 = make_SimpleExp_SEb(outer_list, leaf1);
            
            // 测试各个Text_0的层数
            test_case("测试用例5-1: 最外层Text_0层数", calc_relative_layer(exp1, leaf1) == 1);
            test_case("测试用例5-2: 中间层Text_0层数", calc_relative_layer(exp1, leaf2) == 2);
            test_case("测试用例5-3: 最内层Text_0层数", calc_relative_layer(exp1, leaf3) == 3);
        }
        
        // --- 测试用例 6: 复杂列表中的Text_0 ---
        {
            auto leaf1 = make_FFFzExp_0();
            auto leaf2 = make_FFFzExp_0();
            auto leaf3 = make_FFFzExp_0();
            auto leaf4 = make_FFFzExp_0();
            
            // 创建三个SimpleExp
            auto simple1 = make_SimpleExp_SEa(leaf1);
            auto simple2 = make_SimpleExp_SEa(leaf2);
            auto simple3 = make_SimpleExp_SEa(leaf3);
            
            // 创建三个FFFzExp
            auto fffz1 = make_FFFzExp_FEa(simple1);
            auto fffz2 = make_FFFzExp_FEa(simple2);
            auto fffz3 = make_FFFzExp_FEa(simple3);
            
            // 创建列表: [fffz1, fffz2, fffz3]
            auto list1 = make_CompatyList_CLa(fffz1);
            auto list2 = make_CompatyList_CLb(list1, fffz2);
            auto list3 = make_CompatyList_CLb(list2, fffz3);
            
            // 创建exp1: Z[fffz1, fffz2, fffz3](leaf4)
            auto exp1 = make_SimpleExp_SEb(list3, leaf4);
            
            // 测试各个Text_0的层数
            test_case("测试用例6-1: 列表第一个Text_0层数", calc_relative_layer(exp1, leaf1) == 2);
            test_case("测试用例6-2: 列表第二个Text_0层数", calc_relative_layer(exp1, leaf2) == 2);
            test_case("测试用例6-3: 列表第三个Text_0层数", calc_relative_layer(exp1, leaf3) == 2);
            test_case("测试用例6-4: 直接参数Text_0层数", calc_relative_layer(exp1, leaf4) == 1);
        }
        
        // --- 测试用例 7: 混合类型测试 ---
        {
            auto leaf1 = make_FFFzExp_0();
            auto leaf2 = make_FFFzExp_0();
            
            auto simple1 = make_SimpleExp_SEa(leaf1);
            auto simple2 = make_SimpleExp_SEa(leaf2);
            
            // 创建 FFFzExp: simple1 + simple2
            auto fffz_exp = make_FFFzExp_FEb(simple1, make_FFFzExp_FEa(simple2));
            
            // 创建 exp1: Z[simple1+simple2](0)
            auto leaf3 = make_FFFzExp_0();
            auto list = make_CompatyList_CLa(fffz_exp);
            auto exp1 = make_SimpleExp_SEb(list, leaf3);
            
            // 测试各个层数
            test_case("测试用例7-1: 复合FFFzExp层数", calc_relative_layer(exp1, fffz_exp) == 1);
            test_case("测试用例7-2: 第一个SimpleExp层数", calc_relative_layer(exp1, simple1) == 2);
            test_case("测试用例7-3: 第二个SimpleExp层数", calc_relative_layer(exp1, simple2) == 2);
            test_case("测试用例7-4: 直接Text_0层数", calc_relative_layer(exp1, leaf3) == 1);
        }
        
        std::cout << "\n=== 测试结果: " << passed << "/" << total << " 通过 ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "开始运行 calc_relative_layer 测试..." << std::endl;
    test_calc_relative_layer();
    return 0;
}

*/

// PrSS 初解析 

// 前向声明 
std::string convert_sub_seq(const std::vector<unsigned int>& seq, size_t start, size_t end);
std::string convert_full_seq(const std::vector<unsigned int>& seq);

std::string convert_sub_seq(const std::vector<unsigned int>& seq, size_t start, size_t end) {
    // 基本情况：只有[0]
    if (end - start == 1) {
        return "Z(0)";
    }
    
    // 检查是否全为1（除了开头的0）
    bool allOnes = true;
    for (size_t i = start + 1; i < end; i++) {
        if (seq[i] != 1) {
            allOnes = false;
            break;
        }
    }
    
    if (allOnes) {
        return "Z(" + std::to_string(end - start - 1) + ")";
    }
    
    // 否则，构建新序列并递归处理
    std::vector<unsigned int> newSeq;
    for (size_t i = start + 1; i < end; i++) {
        newSeq.push_back(seq[i] - 1);
    }
    
    std::string inner = convert_full_seq(newSeq);
    return "P(" + inner + ")";
}

// 处理完整序列（可能包含多个加法项）
std::string convert_full_seq(const std::vector<unsigned int>& seq) {
    if (seq.empty()) {
        return "";
    }
    
    // 验证序列
    if (seq[0] != 0) {
        throw std::invalid_argument("序列必须以0开头");
    }
    
    for (size_t i = 0; i < seq.size() - 1; i++) {
        if (seq[i + 1] > seq[i] + 1) {
            throw std::invalid_argument("序列不满足条件：后一位必须≤前一位+1");
        }
    }
    
    // 分割成以0开头的子序列
    std::vector<std::string> parts;
    size_t start = 0;
    
    for (size_t i = 1; i <= seq.size(); i++) {
        // 到达末尾或遇到下一个0
        if (i == seq.size() || seq[i] == 0) {
            if (i > start) {  // 确保有元素
                parts.push_back(convert_sub_seq(seq, start, i));
            }
            start = i;
        }
    }
    
    // 拼接结果
    if (parts.empty()) {
        return "";
    }
    
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); i++) {
        result += "+" + parts[i];
    }
    
    return result;
}

std::string convert(const std::vector<unsigned int>& seq) {
    return convert_full_seq(seq);
}

/*

int main() {
    SequenceConverter converter;
    
    // 测试用例
    try {
        std::vector<unsigned int> test1 = {0};
        std::cout << converter.convert(test1) << std::endl;  // 输出: Z(0)
        
        std::vector<unsigned int> test2 = {0, 0};
        std::cout << converter.convert(test2) << std::endl;  // 输出: Z(0)+Z(0)
        
        std::vector<unsigned int> test3 = {0, 1};
        std::cout << converter.convert(test3) << std::endl;  // 输出: Z(1)
        
        std::vector<unsigned int> test4 = {0, 1, 2};
        std::cout << converter.convert(test4) << std::endl;  // 输出: P(Z(1))
        
        std::vector<unsigned int> test5 = {0, 1, 2, 1, 2};
        std::cout << converter.convert(test5) << std::endl;  // 输出: P(Z(1)+Z(1))
        
        std::vector<unsigned int> test6 = {0, 1, 2, 3};
        std::cout << converter.convert(test6) << std::endl;  // 输出: P(P(Z(1)))
        
        // 测试非法序列
        std::vector<unsigned int> test7 = {0, 2};  // 2 > 0+1，不合法
        std::cout << converter.convert(test7) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "错误: " << e.what() << std::endl;
    }
    
    return 0;
}

*/

// PrSS 重解析 

std::string replace_numbers_P(const std::string& s) {
    // 第一步：替换连续数字（0除外）
    std::string step1;
    size_t i = 0;
    while (i < s.size()) {
        if (std::isdigit(s[i])) {
            // 提取连续数字串
            size_t j = i;
            while (j < s.size() && std::isdigit(s[j])) {
                j++;
            }
            std::string numStr = s.substr(i, j - i);
            unsigned long long num = std::stoull(numStr);
            
            if (num == 0) {
                // 数字0不替换，保留原样
                step1 += numStr;
            } else {
                // 生成 num 个 "Z(0)" 用加号连接
                if (num > 0) {
                    step1 += "Z(0)";
                    for (unsigned long long k = 1; k < num; k++) {
                        step1 += "+Z(0)";
                    }
                }
            }
            i = j;
        } else {
            // 非数字字符，直接添加
            step1.push_back(s[i]);
            i++;
        }
    }
    
    // 第二步：替换所有 'P' 为 "Z[Z(Z(0))]"
    std::string result;
    for (char c : step1) {
        if (c == 'P') {
            result += "Z[Z(Z(0))]";
        } else {
            result.push_back(c);
        }
    }
    
    return result;
}

/*

int main() {
    // 测试用例
    std::cout << replace_numbers_P("0") << std::endl;
    // 输出: 0
    
    std::cout << replace_numbers_P("1") << std::endl;
    // 输出: Z(0)
    
    std::cout << replace_numbers_P("2") << std::endl;
    // 输出: Z(0)+Z(0)
    
    std::cout << replace_numbers_P("3") << std::endl;
    // 输出: Z(0)+Z(0)+Z(0)
    
    std::cout << replace_numbers_P("10") << std::endl;
    // 输出: Z(0)+Z(0)+Z(0)+Z(0)+Z(0)+Z(0)+Z(0)+Z(0)+Z(0)+Z(0)
    
    std::cout << replace_numbers_P("P") << std::endl;
    // 输出: Z[Z(Z(0))]
    
    std::cout << replace_numbers_P("1P2") << std::endl;
    // 输出: Z(0)Z[Z(Z(0))]Z(0)+Z(0)
    
    std::cout << replace_numbers_P("0,1,2") << std::endl;
    // 输出: 0,Z(0),Z(0)+Z(0)
    
    std::cout << replace_numbers_P("P1P") << std::endl;
    // 输出: Z[Z(Z(0))]Z(0)Z[Z(Z(0))]
    
    return 0;
}

*/

// 总表达式中的 PrSS 

// 辅助函数：解析点号分隔的数字序列字符串
std::vector<unsigned int> parse_seq_str(const std::string& seqStr) {
    std::vector<unsigned int> result;
    std::istringstream iss(seqStr);
    std::string token;
    
    while (std::getline(iss, token, '.')) {
        try {
            unsigned int num = std::stoul(token);
            result.push_back(num);
        } catch (const std::invalid_argument&) {
            throw std::invalid_argument("无效的数字: " + token);
        } catch (const std::out_of_range&) {
            throw std::out_of_range("数字超出范围: " + token);
        }
    }
    
    return result;
}

// 辅助函数：将单个数字n替换为n个"Z(0)"相加的形式
std::string replace_single_number(unsigned int n) {
    if (n == 0) {
        return "0";
    }
    
    std::string result = "Z(0)";
    for (unsigned int i = 1; i < n; i++) {
        result += "+Z(0)";
    }
    return result;
}

// 主方法：替换字符串中的数字序列和单个数字
std::string replace_number_seq(const std::string& s) {
    std::string result;
    size_t i = 0;
    size_t n = s.size();
    
    while (i < n) {
        // 查找数字序列的开始
        if (std::isdigit(s[i])) {
            // 收集连续的数字和点号
            size_t j = i;
            bool hasDot = false;
            
            while (j < n && (std::isdigit(s[j]) || s[j] == '.')) {
                if (s[j] == '.') {
                    hasDot = true;
                }
                j++;
            }
            
            std::string numSeqStr = s.substr(i, j - i);
            
            if (hasDot) {
                // 尝试作为序列处理
                try {
                    // 解析序列
                    std::vector<unsigned int> seq = parse_seq_str(numSeqStr);
                    
                    // 验证序列格式
                    if (!seq.empty() && seq[0] == 0) {
                        bool valid = true;
                        for (size_t k = 0; k < seq.size() - 1; k++) {
                            if (seq[k+1] > seq[k] + 1) {
                                valid = false;
                                break;
                            }
                        }
                        
                        if (valid) {
                            // 调用转换函数
                            std::string converted = replace_numbers_P(convert(seq));
                            result += converted;
                            i = j;
                            continue;
                        }
                    }
                } catch (const std::exception&) {
                    // 如果转换失败，将原样保留序列
                }
            } else {
                // 单个数字，不包含点号
                try {
                    unsigned int num = std::stoul(numSeqStr);
                    result += replace_single_number(num);
                    i = j;
                    continue;
                } catch (const std::exception&) {
                    // 转换失败，保留原样
                }
            }
            
            // 如果上面的处理没有成功，将原始字符添加到结果
            result += numSeqStr;
            i = j;
        } else {
            // 非数字字符，直接添加
            result.push_back(s[i]);
            i++;
        }
    }
    
    return result;
}

/*

int main() {
    // 测试用例
    std::cout << replace_number_seq("0,1,2") << std::endl;
    // 输出: P(Z(1))
    
    std::cout << replace_number_seq("序列1: 0,1,1,1 序列2: 0,1,2,3") << std::endl;
    // 输出: 序列1: Z(3) 序列2: P(P(Z(1)))
    
    std::cout << replace_number_seq("0,1,2,1,2,0,1,2") << std::endl;
    // 输出: P(Z(1)+Z(1))+P(Z(1))
    
    std::cout << replace_number_seq("测试0,1,2,3,4结束") << std::endl;
    // 输出: 测试P(P(P(Z(1))))结束
    
    std::cout << replace_number_seq("单个数字0不转换") << std::endl;
    // 输出: 单个数字0不转换
    
    std::cout << replace_number_seq("0,1和0,1,2组合") << std::endl;
    // 输出: Z(1)和P(Z(1))组合
    
    std::cout << replace_number_seq("0,2,3无效序列") << std::endl;
    // 输出: 0,2,3无效序列（因为2>0+1，转换会失败，保留原样）
    
    return 0;
}

*/

/*

int main() {
    // 测试用例
    std::cout << replace_number_seq("0.1.2") << std::endl;
    // 输出: P(Z(1))
    
    std::cout << replace_number_seq("序列1: 0.1.1.1 序列2: 0.1.2.3") << std::endl;
    // 输出: 序列Z(0): Z(3) 序列Z(0)+Z(0): P(P(Z(1)))
    
    std::cout << replace_number_seq("0.1.2.1.2.0.1.2") << std::endl;
    // 输出: P(Z(1)+Z(1))+P(Z(1))
    
    std::cout << replace_number_seq("测试1和2和3") << std::endl;
    // 输出: 测试Z(0)和Z(0)+Z(0)和Z(0)+Z(0)+Z(0)
    
    std::cout << replace_number_seq("单个数字0不转换") << std::endl;
    // 输出: 单个数字0不转换
    
    std::cout << replace_number_seq("0.1和0.1.2组合") << std::endl;
    // 输出: Z(1)和P(Z(1))组合
    
    std::cout << replace_number_seq("无效序列0.2.3") << std::endl;
    // 输出: 无效序列0,2,3（因为2>0+1，转换会失败，保留原样）
    
    std::cout << replace_number_seq("数字10和20") << std::endl;
    
    return 0;
}

*/

// 总解析

FFFzExpPtr to_exp(const std::string& str);
static FFFzExpPtr parse_FFFzExp(const std::string& str);
static SimpleExpPtr parse_SimpleExp(const std::string& str);
static CompatyListPtr parse_CompatyList(const std::string& str);

/**
 * @brief 从字符串解析并构建对应的 FFFzExp 表达式。
 *        此函数是 to_string 方法的反函数。
 * 
 * @param str 输入字符串，应为合法的表达式表示。
 * @return FFFzExpPtr 解析得到的 FFFzExp 智能指针。
 * @throws std::runtime_error 当输入字符串无法被解析为任何合法的 FFFzExp 表达式时抛出。
 */
FFFzExpPtr to_exp(const std::string& str) {
    // 去除字符串首尾的空格
    std::string s = str;
    auto start = s.find_first_not_of(" \t\n\r");
    auto end = s.find_last_not_of(" \t\n\r");
    if (start == std::string::npos) { // 空字符串或纯空白字符串
        throw std::runtime_error("Error in to_exp: Input string is empty or contains only whitespace.");
    }
    s = s.substr(start, end - start + 1);

    // 主要解析逻辑
    return parse_FFFzExp(s);
}

// --- 内部辅助解析函数 ---

/**
 * @brief 解析 FFFzExp 类型字符串。
 *        规则：
 *        1. 若为 "0"，则返回 Text_0。
 *        2. 否则，应为一个或多个 SimpleExp 用 '+' 连接的结构。
 *        3. 递归解析 '+' 分隔的 SimpleExp 部分，并组合成 FFFzExp。
 */
static FFFzExpPtr parse_FFFzExp(const std::string& str) {
    if (str == "0") {
        return make_FFFzExp_0();
    }

    std::vector<SimpleExpPtr> simple_exps;
    size_t start_pos = 0;
    int bracket_depth = 0;        // 圆括号深度
    int square_bracket_depth = 0; // 方括号深度（改为计数器）

    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (c == '(') {
            bracket_depth++;
        } else if (c == ')') {
            if (bracket_depth <= 0) {
                throw std::runtime_error("Error in to_exp: Unmatched ')' in FFFzExp parsing.");
            }
            bracket_depth--;
        } else if (c == '[') {
            square_bracket_depth++;  // 增加方括号深度
        } else if (c == ']') {
            if (square_bracket_depth <= 0) {
                throw std::runtime_error("Error in to_exp: Unmatched ']' in FFFzExp parsing.");
            }
            square_bracket_depth--;  // 减少方括号深度
        } else if (c == '+') {
            if (bracket_depth == 0 && square_bracket_depth == 0) {
                // 发现最外层的 '+'，分割出一个 SimpleExp
                std::string part = str.substr(start_pos, i - start_pos);
                if (part.empty()) {
                    throw std::runtime_error("Error in to_exp: Empty SimpleExp segment before '+'.");
                }
                simple_exps.push_back(parse_SimpleExp(part));
                start_pos = i + 1; // 跳过 '+'
            }
        }
    }

    // 处理最后一个（或唯一的）部分
    std::string last_part = str.substr(start_pos);
    if (last_part.empty()) {
        throw std::runtime_error("Error in to_exp: Empty SimpleExp segment at the end.");
    }
    simple_exps.push_back(parse_SimpleExp(last_part));

    // 检查括号匹配
    if (bracket_depth != 0 || square_bracket_depth != 0) {
        throw std::runtime_error("Error in to_exp: Unmatched brackets in FFFzExp parsing.");
    }

    return combine_FFFzExp(simple_exps);
}

/**
 * @brief 解析 SimpleExp 类型字符串。
 *        规则：
 *        1. 必须以 "Z" 开头。
 *        2. 若格式为 "Z(FFFzExp)"，则为 Text_SEa。
 *        3. 若格式为 "Z[CompatyList](FFFzExp)"，则为 Text_SEb。
 */
static SimpleExpPtr parse_SimpleExp(const std::string& str) {
    if (str.length() < 3 || str[0] != 'Z') { // 最短为 "Z(0)"
        throw std::runtime_error("Error in to_exp: Invalid SimpleExp format, must start with 'Z'.");
    }

    size_t index = 1; // 跳过 'Z'

    if (str[index] == '(') {
        // 情况1: Text_SEa -> Z(FFFzExp)
        // 找到匹配的 ')'
        int depth = 1;
        size_t end_pos = index + 1;
        for (; end_pos < str.length() && depth > 0; ++end_pos) {
            if (str[end_pos] == '(') depth++;
            else if (str[end_pos] == ')') depth--;
        }
        if (depth != 0) {
            throw std::runtime_error("Error in to_exp: Unmatched parentheses in Text_SEa parsing.");
        }
        // end_pos 现在指向最后一个 ')' 之后的位置
        size_t content_start = index + 1;
        size_t content_len = (end_pos - 1) - content_start; // 减去最后的 ')'
        std::string inner = str.substr(content_start, content_len);
        FFFzExpPtr inner_exp = parse_FFFzExp(inner);
        return make_SimpleExp_SEa(inner_exp);

    } else if (str[index] == '[') {
        // 情况2: Text_SEb -> Z[CompatyList](FFFzExp)
        // 解析 CompatyList 部分
        int depth = 1;
        size_t list_end = index + 1;
        for (; list_end < str.length() && depth > 0; ++list_end) {
            if (str[list_end] == '[') depth++;
            else if (str[list_end] == ']') depth--;
        }
        if (depth != 0) {
            throw std::runtime_error("Error in to_exp: Unmatched '[' in Text_SEb parsing.");
        }
        list_end--; // list_end 现在指向匹配的 ']' 字符
        std::string list_str = str.substr(index + 1, list_end - (index + 1)); // 取出 [...] 内的内容
        CompatyListPtr cl = parse_CompatyList(list_str);

        // 检查 ']' 后面是否是 '('
        if (list_end + 1 >= str.length() || str[list_end + 1] != '(') {
            throw std::runtime_error("Error in to_exp: Expected '(' after ']' in Text_SEb.");
        }
        // 解析 FFFzExp 部分
        int paren_depth = 1;
        size_t exp_end = list_end + 2; // 跳过 '('
        for (; exp_end < str.length() && paren_depth > 0; ++exp_end) {
            if (str[exp_end] == '(') paren_depth++;
            else if (str[exp_end] == ')') paren_depth--;
        }
        if (paren_depth != 0) {
            throw std::runtime_error("Error in to_exp: Unmatched '(' in Text_SEb parsing.");
        }
        exp_end--; // exp_end 现在指向匹配的 ')' 字符
        std::string exp_str = str.substr(list_end + 2, exp_end - (list_end + 2)); // 取出 (...) 内的内容
        FFFzExpPtr fe = parse_FFFzExp(exp_str);

        // 检查是否有多余字符
        if (exp_end + 1 != str.length()) {
            throw std::runtime_error("Error in to_exp: Extra characters after ')' in Text_SEb.");
        }

        return make_SimpleExp_SEb(cl, fe);
    } else {
        throw std::runtime_error("Error in to_exp: Invalid SimpleExp format, expected '(' or '[' after 'Z'.");
    }
}

/**
 * @brief 解析 CompatyList 类型字符串。
 *        规则：
 *        1. 为一个或多个 FFFzExp 用 ',' 连接的结构。
 *        2. 递归解析 ',' 分隔的 FFFzExp 部分，并组合成 CompatyList。
 */
static CompatyListPtr parse_CompatyList(const std::string& str) {
    if (str.empty()) {
        throw std::runtime_error("Error in to_exp: CompatyList string cannot be empty.");
    }

    std::vector<FFFzExpPtr> fffz_exps;
    size_t start_pos = 0;
    int bracket_depth = 0;        // 圆括号深度
    int square_bracket_depth = 0; // 方括号深度（改为计数器）

    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (c == '(') {
            bracket_depth++;
        } else if (c == ')') {
            if (bracket_depth <= 0) {
                throw std::runtime_error("Error in to_exp: Unmatched ')' in CompatyList parsing.");
            }
            bracket_depth--;
        } else if (c == '[') {
            square_bracket_depth++;
        } else if (c == ']') {
            if (square_bracket_depth <= 0) {
                throw std::runtime_error("Error in to_exp: Unmatched ']' in CompatyList parsing.");
            }
            square_bracket_depth--;
        } else if (c == ',') {
            if (bracket_depth == 0 && square_bracket_depth == 0) {
                // 发现最外层的 ','，分割出一个 FFFzExp
                std::string part = str.substr(start_pos, i - start_pos);
                if (part.empty()) {
                    throw std::runtime_error("Error in to_exp: Empty segment in CompatyList before ','.");
                }
                fffz_exps.push_back(parse_FFFzExp(part));
                start_pos = i + 1; // 跳过 ','
            }
        }
    }

    // 处理最后一个（或唯一的）部分
    std::string last_part = str.substr(start_pos);
    if (last_part.empty()) {
        throw std::runtime_error("Error in to_exp: Empty segment at the end of CompatyList.");
    }
    fffz_exps.push_back(parse_FFFzExp(last_part));

    // 检查括号匹配
    if (bracket_depth != 0 || square_bracket_depth != 0) {
        throw std::runtime_error("Error in to_exp: Unmatched brackets in CompatyList parsing.");
    }

    return combine_CompatyList(fffz_exps);
}

/*

int main() {
    try {
        // 假设我们有字符串 "Z(Z(a+b)+c)+Z[d,e](f)"
        std::string test_str = "Z(Z(Z(Z(0))+Z(0))+Z(0))+Z[Z[Z(0)](0),Z(Z(Z(0)))](0)";
        // 注意：这里假设 a, b, c, d, e, f 本身是已定义的、可由 parse_FFFzExp 解析的基本表达式字符串。
        // 实际使用时，需要定义更基础的原子表达式（如变量或常量）的解析规则，这里是一个概念示例。

        FFFzExpPtr parsed_exp = to_exp(test_str);
        std::string regenerated_str = parsed_exp->to_string();
        std::cout << "Original: " << test_str << std::endl;
        std::cout << "Parsed and regenerated: " << regenerated_str << std::endl;
        // 理想情况下，如果 test_str 是合法且完全由库支持的形式，两者应相等。
    } catch (const std::runtime_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
    return 0;
}

*/

// 总解析函数
FFFzExpPtr to_FFFzExp(const std::string& input) {
    std::string s = input;
    
    // 1. 检查字符合法性
    std::string allowedChars = "pZz()[]0123456789+,. "; // 包括空格
    for (char c : s) {
        bool isValid = false;
        for (char allowed : allowedChars) {
            if (c == allowed) {
                isValid = true;
                break;
            }
        }
        if (!isValid) {
            throw std::invalid_argument("字符串包含非法字符: " + std::string(1, c));
        }
    }
    
    // 删除
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
    s.erase(std::remove(s.begin(), s.end(), 'p'), s.end());
    
    // 将所有的'z'替换为'Z'
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == 'z') {
            s[i] = 'Z';
        }
    }
    
    s = replace_number_seq(s);
    FFFzExpPtr parsed_exp = to_exp(s);
    
    return parsed_exp;
}

/*

int main() {
	std::cout << to_FFFzExp("pZ(0.1.2)")->to_string() << std::endl;
	std::cout << to_FFFzExp("pZ[0.1.2,0.1](0.1)")->to_string() << std::endl;
	return 0;
}

*/

unsigned int calc_level(FFFzExpPtr exp);
FFFzExpPtr calc_root(FFFzExpPtr exp);
FFFzExpPtr calc_core(FFFzExpPtr exp, FFFzExpPtr simp, unsigned int n);
FFFzExpPtr expand(FFFzExpPtr exp, unsigned int n);
FFFzExpPtr simplify(FFFzExpPtr exp);
bool is_exists(CompatyListPtr col);

// 计算根 

/**
 * 计算表达式 A 的根 (root)。
 *
 * @param exp 一个 FFFzExp 指针，代表表达式 A。
 * @return 一个 FFFzExp 指针，代表计算得到的根。
 * @throws std::runtime_error 如果输入表达式 exp 不是 Text_FEa 类型，或其内部 SimpleExp 不是 Text_SEa 或 Text_SEb 类型。
 */
FFFzExpPtr calc_root(FFFzExpPtr exp) {
    // 1. 输入必须是 Text_FEa 类型
    if (!exp->is_Text_FEa()) {
        throw std::runtime_error("calc_root: 输入表达式必须是 Text_FEa 类型。");
    }

    // 2. 提取内部的 SimpleExp
    SimpleExpPtr simple_exp = exp->extract_Text_FEa();

    if (simple_exp->is_Text_SEb()) {
        // 情况 1: A = ψZ[#,β](α)，其中 SimpleExp 是 Text_SEb
        // 提取 Text_SEb 的两个部分：CompatyList (#,β) 和 FFFzExp (α)
        auto [compaty_list_ptr, fffz_exp_ptr_alpha] = simple_exp->extract_Text_SEb();
        // fffz_exp_ptr_alpha 对应参数 α

        // 我们需要从 CompatyList 中提取出最后一个元素，它对应参数 β。
        // 根据文档，expand_CompatyList 函数从左到右展开列表。
        // 对于格式 Z[#,β](α)，列表部分是 `#,β`，展开后最后一个元素就是 β。
        std::vector<FFFzExpPtr> expanded_list;
        try {
            expanded_list = expand_CompatyList(compaty_list_ptr);
        } catch (const std::runtime_error& e) {
            // 如果展开失败（例如空列表），视为 [#,β] 不存在，根据规则返回 α
            // 文档未详述此点，但基于逻辑一致性，我们在此处返回 α。
            return fffz_exp_ptr_alpha;
        }

        if (expanded_list.empty()) {
            // 如果展开的列表为空，则 [#,β] 不存在，根据规则返回 α
            return fffz_exp_ptr_alpha;
        }

        // 获取列表最后一个元素，即 β
        FFFzExpPtr fffz_exp_ptr_beta = expanded_list.back();

        // 需要检查组合 [#, β, α] 是否存在。
        // 根据用户描述，`#` 代表 CompatyList 中 β 之前的部分。
        // 因此，我们需要构建一个新的 CompatyList，其内容为 [原列表内容, β, α]。
        // 原列表内容就是 `#`，我们已有 `expanded_list`，它包含了所有元素。
        // 我们需要构建 `# + β + α` 的列表。
        // 简便方法：将 expanded_list（即 #,β）与 α 合并成一个新的列表。

        std::vector<FFFzExpPtr> elements_to_check;
        // 1. 加入原列表的所有元素 (#, β)
        elements_to_check.insert(elements_to_check.end(), expanded_list.begin(), expanded_list.end());
        // 2. 加入 α
        elements_to_check.push_back(fffz_exp_ptr_alpha);

        // 使用 combine_CompatyList 构建新的 CompatyList
        CompatyListPtr new_list;
        try {
            new_list = combine_CompatyList(elements_to_check);
        } catch (const std::runtime_error& e) {
            // 组合失败（例如空向量），视为不存在，返回 α
            return fffz_exp_ptr_alpha;
        }

        // 调用 is_exists 检查新列表是否存在
        if (is_exists(new_list)) {
            // 如果 [#,β,α] 存在，则根为 β
            return fffz_exp_ptr_beta;
        } else {
            // 如果 [#,β,α] 不存在，则根为 α
            return fffz_exp_ptr_alpha;
        }

    } else if (simple_exp->is_Text_SEa()) {
        // 情况 2: A = ψZ(α)，其中 SimpleExp 是 Text_SEa
        // 直接提取内部的 FFFzExp (α) 作为根
        return simple_exp->extract_Text_SEa();
    } else {
        // SimpleExp 不是预期的 Text_SEa 或 Text_SEb 类型
        throw std::runtime_error("calc_root: Text_FEa 内部的 SimpleExp 必须是 Text_SEa 或 Text_SEb 类型。");
    }
}

// 计算序 

/**
 * 比较两个表达式 a 和 b 的大小。
 * 
 * @param a 第一个表达式
 * @param b 第二个表达式
 * @return -1 如果 a < b, 0 如果 a == b, 1 如果 a > b
 */
int compare(FFFzExpPtr a, FFFzExpPtr b) {
    // 1. 首先检查指针是否相同
    if (a == b) {
        return 0;
    }

    // 2. 检查是否为 Text_0（0）
    bool a_is_zero = a->is_Text_0();
    bool b_is_zero = b->is_Text_0();
    
    // 规则：对于非零合法表达式 α，α > 0
    if (!a_is_zero && b_is_zero) {
        return 1;  // a > b
    }
    if (a_is_zero && !b_is_zero) {
        return -1; // a < b
    }
    if (a_is_zero && b_is_zero) {
        return 0;  // 都是 0
    }

    // 3. 现在 a 和 b 都是非零合法表达式
    // 先检查特殊形式：ψZ[#,m](0)
    auto is_psi_Z_hash_m_zero = [](FFFzExpPtr exp) -> bool {
        if (!exp->is_Text_FEa()) return false;
        SimpleExpPtr se = exp->extract_Text_FEa();
        if (!se->is_Text_SEb()) return false;
        auto [cl, inner] = se->extract_Text_SEb();
        return inner->is_Text_0();
    };

    if (is_psi_Z_hash_m_zero(a)) {
        // a = ψZ[#,m](0)
        SimpleExpPtr se_a = a->extract_Text_FEa();
        auto [cl_a, zero_a] = se_a->extract_Text_SEb();
        
        // 提取 m：CompatyList 的最后一个元素
        std::vector<FFFzExpPtr> expanded_cl_a = expand_CompatyList(cl_a);
        if (expanded_cl_a.empty()) {
            throw std::runtime_error("Invalid ψZ[#,m](0) form: # cannot be empty");
        }
        FFFzExpPtr m = expanded_cl_a.back();
        
        // 构建 #（去掉最后一个元素 m）
        std::vector<FFFzExpPtr> hash_elements;
        if (expanded_cl_a.size() > 1) {
            hash_elements = std::vector<FFFzExpPtr>(expanded_cl_a.begin(), expanded_cl_a.end() - 1);
        }
        
        // 构建 ψZ[#](m[0])，其中 m[0] = expand(m, 0)
        FFFzExpPtr m_expanded = expand(m, 0);
        
        // 创建新的表达式
        FFFzExpPtr new_a;
        if (hash_elements.empty()) {
            // 如果 # 为空，则 ψZ[#](m[0]) = ψZ(m[0])
            SimpleExpPtr new_se = make_SimpleExp_SEa(m_expanded);
            new_a = make_FFFzExp_FEa(new_se);
        } else {
            // # 非空，创建 ψZ[#](m[0])
            CompatyListPtr new_cl = combine_CompatyList(hash_elements);
            SimpleExpPtr new_se = make_SimpleExp_SEb(new_cl, m_expanded);
            new_a = make_FFFzExp_FEa(new_se);
        }
        
        // 规则：α>β 当且仅当 ψZ[#](m[0])>β
        return compare(new_a, b);
    }

    if (is_psi_Z_hash_m_zero(b)) {
        // b = ψZ[#,m](0)，对称处理
        int result = compare(b, a);
        return -result; // 反转结果
    }

    // 4. 检查是否为 ψZ[#](m) 形式
    auto is_psi_Z_form = [](FFFzExpPtr exp, CompatyListPtr& hash, FFFzExpPtr& inner) -> bool {
        if (!exp->is_Text_FEa()) return false;
        SimpleExpPtr se = exp->extract_Text_FEa();
        
        if (se->is_Text_SEa()) {
            // ψZ(m) 形式，hash 为空
            hash = nullptr;
            inner = se->extract_Text_SEa();
            return true;
        } else if (se->is_Text_SEb()) {
            auto [cl, fffz] = se->extract_Text_SEb();
            hash = cl;
            inner = fffz;
            return true;
        }
        return false;
    };

    CompatyListPtr hash_a, hash_b;
    FFFzExpPtr inner_a, inner_b;
    bool a_is_psi = is_psi_Z_form(a, hash_a, inner_a);
    bool b_is_psi = is_psi_Z_form(b, hash_b, inner_b);

    if (a_is_psi && b_is_psi) {
        // 比较 hash 部分是否相同
        bool hash_equal = false;
        
        // 处理 hash 为空的特殊情况
        if (hash_a == nullptr && hash_b == nullptr) {
            hash_equal = true;
        } else if (hash_a != nullptr && hash_b != nullptr) {
            // 展开两个 hash 比较
            std::vector<FFFzExpPtr> expanded_hash_a = expand_CompatyList(hash_a);
            std::vector<FFFzExpPtr> expanded_hash_b = expand_CompatyList(hash_b);
            
            if (expanded_hash_a.size() == expanded_hash_b.size()) {
                hash_equal = true;
                for (size_t i = 0; i < expanded_hash_a.size(); i++) {
                    if (compare(expanded_hash_a[i], expanded_hash_b[i]) != 0) {
                        hash_equal = false;
                        break;
                    }
                }
            }
        }
        
        if (hash_equal) {
            // 规则：对于 α=ψZ[#](m) 和 β=ψZ[#](n)，比较 m 和 n
            return compare(inner_a, inner_b);
        } else {
            // hash 不同
            // 展开 hash
            std::vector<FFFzExpPtr> expanded_hash_a = 
                (hash_a != nullptr) ? expand_CompatyList(hash_a) : std::vector<FFFzExpPtr>();
            std::vector<FFFzExpPtr> expanded_hash_b = 
                (hash_b != nullptr) ? expand_CompatyList(hash_b) : std::vector<FFFzExpPtr>();
            
            // 找到共同前缀 #
            size_t min_len = std::min(expanded_hash_a.size(), expanded_hash_b.size());
            size_t diff_index = min_len;
            
            for (size_t i = 0; i < min_len; i++) {
                if (compare(expanded_hash_a[i], expanded_hash_b[i]) != 0) {
                    diff_index = i;
                    break;
                }
            }
            
            if (diff_index < min_len) {
                // 规则：对于 α=ψZ[#,a,&](m) 和 β=ψZ[#,b,%](n)，如果 a≠b 则比较 a 和 b
                FFFzExpPtr a_elem = expanded_hash_a[diff_index];
                FFFzExpPtr b_elem = expanded_hash_b[diff_index];
                return compare(a_elem, b_elem);
            } else {
                // 一个 hash 是另一个的前缀
                if (expanded_hash_a.size() < expanded_hash_b.size()) {
                    // a 的 hash 是 b 的 hash 的前缀
                    // 规则：对于 α=ψZ[#](m) 和 β=ψZ[#,&](n)，α > β
                    return 1;
                } else {
                    // b 的 hash 是 a 的 hash 的前缀
                    return -1;
                }
            }
        }
    }

    // 5. 处理一般情况：α = X + m + A 和 β = X + n + B
    // 获取展开序列
    std::vector<SimpleExpPtr> expanded_a = expand_FFFzExp(a);
    std::vector<SimpleExpPtr> expanded_b = expand_FFFzExp(b);
    
    // 找到第一个不同的位置
    size_t min_len = std::min(expanded_a.size(), expanded_b.size());
    for (size_t i = 0; i < min_len; i++) {
        SimpleExpPtr se_a = expanded_a[i];
        SimpleExpPtr se_b = expanded_b[i];
        
        // 比较两个简单表达式：将它们包装成 FFFzExp
        FFFzExpPtr wrapped_a = make_FFFzExp_FEa(se_a);
        FFFzExpPtr wrapped_b = make_FFFzExp_FEa(se_b);
        
        int cmp_result = compare(wrapped_a, wrapped_b);
        if (cmp_result != 0) {
            // 规则：如果 m ≠ n，则比较 m 和 n
            return cmp_result;
        }
    }
    
    // 6. 前缀相同，比较长度
    if (expanded_a.size() < expanded_b.size()) {
        // 规则：对于 α 和 β = α + m，α < β
        return -1;
    } else if (expanded_a.size() > expanded_b.size()) {
        return 1;
    }
    
    // 7. 如果所有比较都相等，则两个表达式相等
    return 0;
}

/**
 * 检查 a 是否小于 b
 * 
 * @param a 第一个表达式
 * @param b 第二个表达式
 * @return true 如果 a < b，否则 false
 */
bool is_less_than(FFFzExpPtr a, FFFzExpPtr b) {
    return compare(a, b) == -1;
}

/**
 * 检查 a 是否等于 b
 * 
 * @param a 第一个表达式
 * @param b 第二个表达式
 * @return true 如果 a == b，否则 false
 */
bool is_equal_to(FFFzExpPtr a, FFFzExpPtr b) {
    return compare(a, b) == 0;
}

/**
 * 检查 a 是否大于 b
 * 
 * @param a 第一个表达式
 * @param b 第二个表达式
 * @return true 如果 a > b，否则 false
 */
bool is_greater_than(FFFzExpPtr a, FFFzExpPtr b) {
    return compare(a, b) == 1;
}

/*

FFFzExpPtr expr1 = ...;
FFFzExpPtr expr2 = ...;

if (is_less_than(expr1, expr2)) {
    // expr1 < expr2
} else if (is_equal_to(expr1, expr2)) {
    // expr1 == expr2
} else { // is_greater_than(expr1, expr2)
    // expr1 > expr2
}

*/


// 计算核 

SimpleExpPtr apply_replacements_to_simpleexp(const SimpleExpPtr& root, 
	std::map<FFFzExpPtr, FFFzExpPtr>& repl_map, 
	std::set<void*>& visited);
FFFzExpPtr apply_replacements_to_fffzexp(const FFFzExpPtr& root, 
	std::map<FFFzExpPtr, FFFzExpPtr>& repl_map, 
	std::set<void*>& visited);

// 辅助结构：用于排序x的信息
struct XProcessingInfo {
    FFFzExpPtr x_ptr;          // 原始的x节点
    unsigned int layer;        // 末项相对于A的层数
    size_t right_index;        // 末项在A展开式中的索引
    bool index_valid;          // 索引是否有效
};

// 比较XProcessingInfo用于排序
bool compareXInfo(const XProcessingInfo& a, const XProcessingInfo& b) {
    if (a.layer != b.layer) {
        return a.layer < b.layer;  // 层数小的先执行
    } else {
        // 层数相同，按自右而左：索引大的先执行
        // 索引无效的项被认为在最左（索引为0）
        size_t idx_a = a.index_valid ? a.right_index : 0;
        size_t idx_b = b.index_valid ? b.right_index : 0;
        return idx_a > idx_b;  // 降序排序
    }
}

// 为给定的x计算替换结果
FFFzExpPtr calculate_replacement_for_x(FFFzExpPtr x, FFFzExpPtr s) {
    // 确保x是Text_FEb
    if (!x->is_Text_FEb()) {
        throw std::runtime_error("Expected x to be Text_FEb in calculate_replacement_for_x");
    }
    
    // 获取x的末项
    SimpleExpPtr last_simple = get_last_FFFzExp(x);
    FFFzExpPtr last_item_fffz = make_FFFzExp_FEa(last_simple);
    
    // 比较末项和s
    int cmp = compare(last_item_fffz, s);
    
    if (cmp >= 0) {  // 末项 ≥ s
        // 将x替换为其末项
        return last_item_fffz;
    } else {  // 末项 < s
        // 删除x中所有 ≥ s的项
        std::vector<SimpleExpPtr> expanded = expand_FFFzExp(x);
        std::vector<SimpleExpPtr> filtered;
        
        for (const auto& item : expanded) {
            FFFzExpPtr item_fffz = make_FFFzExp_FEa(item);
            if (compare(item_fffz, s) < 0) {  // 只保留 < s 的项
                filtered.push_back(item);
            }
        }
        
        if (filtered.empty()) {
            return make_FFFzExp_0();  // 如果没有剩余项，返回Text_0
        } else {
            return combine_FFFzExp(filtered);
        }
    }
}

// 递归替换函数：处理SimpleExp节点
SimpleExpPtr apply_replacements_to_simpleexp(const SimpleExpPtr& root,
                                             std::map<FFFzExpPtr, FFFzExpPtr>& repl_map,
                                             std::set<void*>& visited) {
    // 防止循环引用
    if (visited.find(root.get()) != visited.end()) {
        return root;
    }
    visited.insert(root.get());
    
    if (root->is_Text_SEa()) {
        // Text_SEa: 包含一个FFFzExp
        FFFzExpPtr inner = root->extract_Text_SEa();
        FFFzExpPtr new_inner = apply_replacements_to_fffzexp(inner, repl_map, visited);
        
        // 如果内部FFFzExp没有变化，返回原SimpleExp
        if (new_inner == inner) {
            return root;
        }
        
        // 否则创建新的SimpleExp
        return make_SimpleExp_SEa(new_inner);
    } else {  // Text_SEb
        auto [cl, fffz] = root->extract_Text_SEb();
        
        // 处理右边的FFFzExp
        FFFzExpPtr new_fffz = apply_replacements_to_fffzexp(fffz, repl_map, visited);
        
        // 处理CompatyList中的每个FFFzExp
        std::vector<FFFzExpPtr> expanded_cl;
        try {
            expanded_cl = expand_CompatyList(cl);
        } catch (const std::runtime_error&) {
            // 如果无法展开，保持原样
            expanded_cl.clear();
        }
        
        std::vector<FFFzExpPtr> new_expanded_cl;
        bool cl_changed = false;
        
        for (const auto& item : expanded_cl) {
            FFFzExpPtr new_item = apply_replacements_to_fffzexp(item, repl_map, visited);
            new_expanded_cl.push_back(new_item);
            if (new_item != item) {
                cl_changed = true;
            }
        }
        
        // 重构CompatyList
        CompatyListPtr new_cl = cl;
        if (cl_changed) {
            try {
                new_cl = combine_CompatyList(new_expanded_cl);
            } catch (const std::runtime_error&) {
                // 如果无法组合，保持原CompatyList
                new_cl = cl;
            }
        }
        
        // 如果两部分都没有变化，返回原SimpleExp
        if (new_fffz == fffz && !cl_changed) {
            return root;
        }
        
        // 否则创建新的SimpleExp
        return make_SimpleExp_SEb(new_cl, new_fffz);
    }
}

// 递归替换函数：处理FFFzExp节点
FFFzExpPtr apply_replacements_to_fffzexp(const FFFzExpPtr& root, 
                                         std::map<FFFzExpPtr, FFFzExpPtr>& repl_map,
                                         std::set<void*>& visited) {
    // 防止循环引用
    if (visited.find(root.get()) != visited.end()) {
        return root;
    }
    visited.insert(root.get());
    
    // 检查当前节点是否需要被替换
    auto it = repl_map.find(root);
    if (it != repl_map.end()) {
        // 注意：这里我们应用替换，但不从map中删除
        // 因为同一个原始节点可能在树中出现多次（共享子树）
        return it->second;
    }
    
    // 否则递归处理子节点
    if (root->is_Text_0()) {
        return root;
    } else if (root->is_Text_FEa()) {
        SimpleExpPtr inner = root->extract_Text_FEa();
        SimpleExpPtr new_inner = apply_replacements_to_simpleexp(inner, repl_map, visited);
        if (new_inner == inner) {
            return root;
        }
        return make_FFFzExp_FEa(new_inner);
    } else {  // Text_FEb
        auto [left_se, right_ffe] = root->extract_Text_FEb();
        SimpleExpPtr new_left = apply_replacements_to_simpleexp(left_se, repl_map, visited);
        FFFzExpPtr new_right = apply_replacements_to_fffzexp(right_ffe, repl_map, visited);
        
        if (new_left == left_se && new_right == right_ffe) {
            return root;
        }
        return make_FFFzExp_FEb(new_left, new_right);
    }
}

// 主函数：calc_core
FFFzExpPtr calc_core(FFFzExpPtr A, FFFzExpPtr s, unsigned int n) {
    // 1. 参数验证
    if (!A->is_Text_FEa()) {
        throw std::runtime_error("A must be a legal simple expression (Text_FEa)");
    }
    if (!s->is_Text_FEa()) {
        throw std::runtime_error("s must be a non-zero simple expression (Text_FEa)");
    }
    
    // 2. 提取A中的SimpleExp用于层数计算
    SimpleExpPtr A_simple = A->extract_Text_FEa();
    
    // 3. 收集A中所有被包含的Text_FEb节点
    std::set<void*> visited_for_collection;
    std::queue<FFFzExpPtr> queue;
    std::vector<FFFzExpPtr> all_text_feb_nodes;
    
    queue.push(A);
    visited_for_collection.insert(A.get());
    
    while (!queue.empty()) {
        FFFzExpPtr current = queue.front();
        queue.pop();
        
        // 如果是Text_FEb且被A包含，则记录下来
        if (current->is_Text_FEb() && is_include(A, current)) {
            all_text_feb_nodes.push_back(current);
        }
        
        // 获取子节点继续遍历
        auto children_set = get_children(current, visited_for_collection);
        for (const auto& child : children_set) {
            if (visited_for_collection.find(child.get()) == visited_for_collection.end()) {
                visited_for_collection.insert(child.get());
                queue.push(child);
            }
        }
    }
    
    // 4. 展开A，建立SimpleExp到索引的映射
    std::vector<SimpleExpPtr> expanded_A = expand_FFFzExp(A);
    std::map<SimpleExp*, size_t> simple_exp_to_index;
    for (size_t i = 0; i < expanded_A.size(); ++i) {
        simple_exp_to_index[expanded_A[i].get()] = i;
    }
    
    // 5. 筛选符合条件的x（层数 ≤ n）
    std::vector<XProcessingInfo> candidate_infos;
    
    for (FFFzExpPtr x : all_text_feb_nodes) {
        try {
            // 计算末项关于A的层数
            SimpleExpPtr last_simple = get_last_FFFzExp(x);
            unsigned int relative_layer = calc_relative_layer(A_simple, last_simple);
            
            if (relative_layer <= n) {
                XProcessingInfo info;
                info.x_ptr = x;
                info.layer = relative_layer;
                info.index_valid = false;
                info.right_index = 0;
                
                // 获取末项在A展开式中的索引
                auto it = simple_exp_to_index.find(last_simple.get());
                if (it != simple_exp_to_index.end()) {
                    info.right_index = it->second;
                    info.index_valid = true;
                }
                
                candidate_infos.push_back(info);
            }
        } catch (const std::runtime_error&) {
            // 如果无法获取末项或计算层数，跳过此x
            continue;
        }
    }
    
    // 6. 按规则排序
    std::sort(candidate_infos.begin(), candidate_infos.end(), compareXInfo);
    
    // 7. 为每个x预先计算替换结果
    std::map<FFFzExpPtr, FFFzExpPtr> replacement_map;
    for (const auto& info : candidate_infos) {
        FFFzExpPtr new_node = calculate_replacement_for_x(info.x_ptr, s);
        replacement_map[info.x_ptr] = new_node;
    }
    
    // 8. 应用替换
    std::set<void*> visited_for_replacement;
    FFFzExpPtr result = apply_replacements_to_fffzexp(A, replacement_map, visited_for_replacement);
    
    return result;
}

/*

FFFzExpPtr calc_core(FFFzExpPtr A, FFFzExpPtr s, unsigned int n) {
    // ... 参数验证，收集并排序 sorted_x_list ...

    std::map<FFFzExpPtr, FFFzExpPtr> repl_map;
    // 步骤3：为每个x预先计算替换结果
    for (const auto& x_info : sorted_x_list) {
        FFFzExpPtr x = x_info.x_ptr;
        FFFzExpPtr new_node = calculate_replacement_for_x(x, s); // 封装上述比较、过滤、重构逻辑
        repl_map[x] = new_node;
    }

    // 步骤4：应用替换
    FFFzExpPtr result = apply_replacements_recursive(A, repl_map);

    // 步骤5：验证所有x都被处理了
    if (!repl_map.empty()) { // 注意：apply_replacements_recursive 应在每次成功应用替换后从map中删除该项
        throw std::runtime_error("Some candidate x were not found in the expression tree during replacement.");
    }
    return result;
}

FFFzExpPtr apply_replacements_recursive(const FFFzExpPtr& root, std::map<FFFzExpPtr, FFFzExpPtr>& repl_map) {
    // 检查是否需被整体替换
    auto it = repl_map.find(root);
    if (it != repl_map.end()) {
        repl_map.erase(it); // 标记此替换已应用
        return it->second;
    }

    // 否则，递归处理子节点并重建
    if (root->is_Text_0()) {
        return root;
    } else if (root->is_Text_FEa()) {
        SimpleExpPtr inner = root->extract_Text_FEa();
        SimpleExpPtr new_inner = apply_replacements_to_simple_recursive(inner, repl_map);
        if (new_inner == inner) return root;
        return make_FFFzExp_FEa(new_inner);
    } else { // Text_FEb
        auto [left_se, right_ffe] = root->extract_Text_FEb();
        SimpleExpPtr new_left = apply_replacements_to_simple_recursive(left_se, repl_map);
        FFFzExpPtr new_right = apply_replacements_recursive(right_ffe, repl_map);
        if (new_left == left_se && new_right == right_ffe) return root;
        return make_FFFzExp_FEb(new_left, new_right);
    }
}
// 需类似实现 `apply_replacements_to_simple_recursive` 处理 SimpleExp 节点。

*/

// 计算极限等级 

unsigned int calc_level(FFFzExpPtr exp) {
    if (!exp->is_Text_FEa()) {
        throw std::runtime_error("calc_level: Input exp must be a non-zero simple expression (Text_FEa).");
    }

    SimpleExpPtr simple_exp = exp->extract_Text_FEa();

    FFFzExpPtr alpha; // 表达式 α
    CompatyListPtr compat_list; // 列表 [#]，可能为空

    if (simple_exp->is_Text_SEa()) {
        // 情况：ψZ(α)，等价于 ψZ[](α)。此时 # 为空列表。
        alpha = simple_exp->extract_Text_SEa();
        compat_list = nullptr;
    } else if (simple_exp->is_Text_SEb()) {
        // 情况：ψZ[#](α)，其中 # 非空。
        auto extracted = simple_exp->extract_Text_SEb();
        compat_list = extracted.first;  // [#]
        alpha = extracted.second;      // α
    } else {
        // 理论上不会发生，因为 SimpleExp 只有两种变体。
        throw std::runtime_error("calc_level: Internal error - SimpleExp is of unknown type.");
    }

    // 判断 α 是否为“后继表达式”。
    static FFFzExpPtr Text_1 = []() -> FFFzExpPtr {
        FFFzExpPtr text_0 = std::make_shared<FFFzExp>(); // 创建 Text_0
        SimpleExpPtr se = std::make_shared<SimpleExp>(text_0); // 创建 Text_SEa，即 Z(0)
        return std::make_shared<FFFzExp>(se); // 创建 Text_FEa，即 ψZ(0)
    }();

    // 获取 α 的最后一个 FFFzExp。
    FFFzExpPtr last_of_alpha = make_FFFzExp_FEa(get_last_FFFzExp(alpha));

    bool is_successor = (compare(last_of_alpha, Text_1) == 0);

    if (is_successor) {
        // 情况 1: α 是后继表达式，等级为 0。
        return 0;
    }

    // 6. α 不是后继表达式。需要判断 [#, α] 是否存在。
    //    构造 [#, α] 这个 CompatyList。

    CompatyListPtr combined_list;
    if (compat_list == nullptr) {
        std::vector<FFFzExpPtr> vec = {alpha};
        combined_list = combine_CompatyList(vec);
    } else {
        std::vector<FFFzExpPtr> expanded = expand_CompatyList(compat_list);
        expanded.push_back(alpha); // 在末尾添加 α
        combined_list = combine_CompatyList(expanded);
    }

    // 7. 调用 is_exists(combined_list) 判断是否存在。
    bool exists = is_exists(combined_list);

    if (!exists) {
        // 情况 2: [#, α] 不存在，等级为 1。
        return 1;
    } else {
        // 情况 3: [#, α] 存在，等级为 2。
        return 2;
    }
}

FFFzExpPtr expand(FFFzExpPtr exp, unsigned int n) {
    // 全局常量 Text_1 = ψZ(0)
    static FFFzExpPtr Text_1 = []() -> FFFzExpPtr {
        FFFzExpPtr text_0 = make_FFFzExp_0();
        SimpleExpPtr se = make_SimpleExp_SEa(text_0);
        return make_FFFzExp_FEa(se);
    }();

    // 1. 处理 Text_0
    if (exp->is_Text_0()) {
        throw std::runtime_error("Cannot expand Text_0");
    }

    // 2. 处理 Text_FEb - 只展开末项
    if (exp->is_Text_FEb()) {
        // 展开整个表达式
        std::vector<SimpleExpPtr> parts = expand_FFFzExp(exp);
        
        // 对最后一个部分进行展开
        SimpleExpPtr last_simple = parts.back();
        FFFzExpPtr last_exp = make_FFFzExp_FEa(last_simple);
        FFFzExpPtr expanded_last = expand(last_exp, n);
        
        // 将展开后的部分与前面部分重新组合
        parts.pop_back(); // 移除原最后一个部分
        
        // 展开 expanded_last
        std::vector<SimpleExpPtr> last_parts = expand_FFFzExp(expanded_last);
        
        // 合并所有部分
        parts.insert(parts.end(), last_parts.begin(), last_parts.end());
        
        return combine_FFFzExp(parts);
    }

    // 3. 处理 Text_FEa
    if (!exp->is_Text_FEa()) {
        throw std::runtime_error("Unexpected expression type in expand");
    }

    // 提取 SimpleExp
    SimpleExpPtr simple_exp = exp->extract_Text_FEa();
    
    // 提取兼容列表和主参数
    CompatyListPtr compat_list = nullptr;
    FFFzExpPtr alpha = nullptr;
    
    if (simple_exp->is_Text_SEa()) {
        // ψZ(α) 形式
        alpha = simple_exp->extract_Text_SEa();
        // 空列表用 nullptr 表示
    } else if (simple_exp->is_Text_SEb()) {
        // ψZ[#](α) 形式
        auto extracted = simple_exp->extract_Text_SEb();
        compat_list = extracted.first;
        alpha = extracted.second;
    } else {
        throw std::runtime_error("Unexpected SimpleExp type");
    }

    // 检查 α 是否为后继表达式
    FFFzExpPtr last_of_alpha = make_FFFzExp_FEa(get_last_FFFzExp(alpha));
    bool is_alpha_successor = (compare(last_of_alpha, Text_1) == 0);

    // 情况 1: α 是后继表达式
    if (is_alpha_successor) {
        // 计算 α 的前驱 β
        std::vector<SimpleExpPtr> alpha_parts = expand_FFFzExp(alpha);
        FFFzExpPtr beta = nullptr;
        
        if (alpha_parts.empty()) {
            beta = make_FFFzExp_0(); // 理论上不会发生
        } else {
            alpha_parts.pop_back(); // 去掉最后一个 Text_1
            if (alpha_parts.empty()) {
                beta = make_FFFzExp_0(); // α 就是 Text_1
            } else {
                beta = combine_FFFzExp(alpha_parts);
            }
        }
        
        // ψZ[#](α+1) 展开规则
        if (n == 0) {
            return make_FFFzExp_0(); // ψZ[#](α+1)[0] = 0
        } else {
            // ψZ[#](α+1)[s+1] = ψZ[#](α+1)[s] + ψZ[#](α)
            FFFzExpPtr prev = expand(exp, n-1);
            
            // 构造 ψZ[#](α)
            SimpleExpPtr term2_simple = nullptr;
            if (compat_list == nullptr) {
                term2_simple = make_SimpleExp_SEa(beta);
            } else {
                term2_simple = make_SimpleExp_SEb(compat_list, beta);
            }
            FFFzExpPtr term2 = make_FFFzExp_FEa(term2_simple);
            
            // 组合两个部分
            std::vector<SimpleExpPtr> prev_parts = expand_FFFzExp(prev);
            std::vector<SimpleExpPtr> term2_parts = expand_FFFzExp(term2);
            
            std::vector<SimpleExpPtr> all_parts;
            all_parts.reserve(prev_parts.size() + term2_parts.size());
            all_parts.insert(all_parts.end(), prev_parts.begin(), prev_parts.end());
            all_parts.insert(all_parts.end(), term2_parts.begin(), term2_parts.end());
            
            return combine_FFFzExp(all_parts);
        }
    }
    
    // α 不是后继表达式，构造 [#,α]
    CompatyListPtr combined_list = nullptr;
    if (compat_list == nullptr) {
        // [#,α] = [α]
        std::vector<FFFzExpPtr> vec = {alpha};
        combined_list = combine_CompatyList(vec);
    } else {
        // 展开 compat_list，添加 α
        std::vector<FFFzExpPtr> vec = expand_CompatyList(compat_list);
        vec.push_back(alpha);
        combined_list = combine_CompatyList(vec);
    }
    
    bool exists = is_exists(combined_list);
    
    // 情况 2: [#,α] 存在
    if (exists) {
        if (n == 0) {
            return alpha; // ψZ[#](α)[0] = α
        } else {
            // ψZ[#](α)[s+1] = ψZ[#,α](ψZ[#](α)[s])
            FFFzExpPtr prev = expand(exp, n-1);
            
            // 构造 ψZ[#,α](prev)
            SimpleExpPtr new_simple = make_SimpleExp_SEb(combined_list, prev);
            return make_FFFzExp_FEa(new_simple);
        }
    }
    
    // 情况 3: [#,α] 不存在
    // 检查 compat_list 是否为空
    if (compat_list == nullptr) {
        throw std::runtime_error("Cannot apply rule: compat_list is empty and [#,α] does not exist");
    }
    
    // 提取 # 和 β
    std::vector<FFFzExpPtr> compat_parts = expand_CompatyList(compat_list);
    if (compat_parts.empty()) {
        throw std::runtime_error("compat_list cannot be empty in this context");
    }
    
    FFFzExpPtr beta = compat_parts.back();
    compat_parts.pop_back(); // 移除 β，得到 #
    
    // 构造 [#,β,α] 检查是否存在
    CompatyListPtr triple_list = nullptr;
    {
        std::vector<FFFzExpPtr> triple_vec = compat_parts; // #
        triple_vec.push_back(beta); // β
        triple_vec.push_back(alpha); // α
        triple_list = combine_CompatyList(triple_vec);
    }
    
    bool triple_exists = is_exists(triple_list);
    
    if (triple_exists) {
        throw std::runtime_error("Unexpected: [#,β,α] exists");
    }
    
    // 构造新的兼容列表 [#,β]
    CompatyListPtr new_compat = nullptr;
    if (compat_parts.empty()) {
        // 如果 # 为空，[#,β] = [β]
        std::vector<FFFzExpPtr> beta_vec = {beta};
        new_compat = combine_CompatyList(beta_vec);
    } else {
        // 重新组合 [#,β]
        std::vector<FFFzExpPtr> new_vec = compat_parts;
        new_vec.push_back(beta);
        new_compat = combine_CompatyList(new_vec);
    }
    
    // 比较 β 和 α
    int cmp = compare(beta, alpha);
    
    if (cmp >= 0) { // β ≥ α
        // ψZ[#,β](α)[s] = ψZ[#,β](α[s])
        FFFzExpPtr alpha_expanded = expand(alpha, n);
        SimpleExpPtr new_simple = make_SimpleExp_SEb(new_compat, alpha_expanded);
        return make_FFFzExp_FEa(new_simple);
    } else { // β < α
        // 寻找最小的 k 使得 α[k] ≥ β
        unsigned int k = 0;
        while (true) {
            FFFzExpPtr alpha_k = expand(alpha, k);
            if (compare(alpha_k, beta) >= 0) {
                break;
            }
            k++;
        }
        
        // ψZ[#,β](α)[s] = ψZ[#,β](α[s+k])
        FFFzExpPtr alpha_expanded = expand(alpha, n + k);
        SimpleExpPtr new_simple = make_SimpleExp_SEb(new_compat, alpha_expanded);
        return make_FFFzExp_FEa(new_simple);
    }
}

// 简化

SimpleExpPtr simplify_simple_exp(SimpleExpPtr se);
CompatyListPtr simplify_compat_list(CompatyListPtr cl);

FFFzExpPtr simplify(FFFzExpPtr exp) {
    // 规则1: 处理表达式 + 0 的情况
    if (exp->is_Text_FEb()) {
        auto [left_simple, right_exp] = exp->extract_Text_FEb();
        
        // 简化左表达式
        SimpleExpPtr simplified_left = simplify_simple_exp(left_simple);
        
        // 简化右表达式
        FFFzExpPtr simplified_right = simplify(right_exp);
        
        // 检查简化后的右表达式是否为 0
        if (simplified_right->is_Text_0()) {
            // 规则: %+0 = %，所以返回左表达式
            return make_FFFzExp_FEa(simplified_left);
        }
        
        // 否则重新组合
        return make_FFFzExp_FEb(simplified_left, simplified_right);
    }
    
    // 规则2: 处理简单表达式
    if (exp->is_Text_FEa()) {
        SimpleExpPtr simple_exp = exp->extract_Text_FEa();
        SimpleExpPtr simplified_simple = simplify_simple_exp(simple_exp);
        
        // 如果简化后的简单表达式与原来不同，重新构造
        if (simplified_simple != simple_exp) {
            return make_FFFzExp_FEa(simplified_simple);
        }
        return exp;  // 没有变化
    }
    
    // Text_0 保持不变
    if (exp->is_Text_0()) {
        return exp;
    }
    
    // 不应该到达这里
    throw std::runtime_error("simplify: Unexpected expression type");
}

// 辅助函数：简化简单表达式
SimpleExpPtr simplify_simple_exp(SimpleExpPtr se) {
    if (se->is_Text_SEa()) {
        // Text_SEa: 简化内部的 FFFzExp
        FFFzExpPtr inner = se->extract_Text_SEa();
        FFFzExpPtr simplified_inner = simplify(inner);
        
        if (simplified_inner != inner) {
            return make_SimpleExp_SEa(simplified_inner);
        }
        return se;  // 没有变化
    }
    
    if (se->is_Text_SEb()) {
        auto [compat_list, alpha] = se->extract_Text_SEb();
        
        // 简化 alpha
        FFFzExpPtr simplified_alpha = simplify(alpha);
        
        // 简化兼容列表
        CompatyListPtr simplified_compat_list = simplify_compat_list(compat_list);
        
        // 提取简化后的兼容列表的最后一个元素（如果存在）
        FFFzExpPtr beta = nullptr;
        CompatyListPtr new_compat_list = simplified_compat_list;
        
        if (simplified_compat_list != nullptr) {
            // 展开兼容列表
            std::vector<FFFzExpPtr> compat_parts = expand_CompatyList(simplified_compat_list);
            
            if (!compat_parts.empty()) {
                beta = compat_parts.back();  // 最后一个元素是 β
                
                // 检查是否满足简化条件: β > α
                if (compare(beta, simplified_alpha) > 0) {
                    // 规则: ψZ[#,β](α) = ψZ[#](α) 当 β > α
                    
                    // 移除 β，得到 #
                    compat_parts.pop_back();
                    
                    if (compat_parts.empty()) {
                        // 如果 # 为空，构造 Text_SEa
                        return make_SimpleExp_SEa(simplified_alpha);
                    } else {
                        // 重新组合 #
                        new_compat_list = combine_CompatyList(compat_parts);
                        return make_SimpleExp_SEb(new_compat_list, simplified_alpha);
                    }
                }
            }
        }
        
        // 如果没有简化规则适用，但内部表达式有变化，重新构造
        if (simplified_alpha != alpha || simplified_compat_list != compat_list) {
            return make_SimpleExp_SEb(simplified_compat_list, simplified_alpha);
        }
        
        return se;  // 没有变化
    }
    
    // 不应该到达这里
    throw std::runtime_error("simplify_simple_exp: Unexpected SimpleExp type");
}

// 辅助函数：简化兼容列表
CompatyListPtr simplify_compat_list(CompatyListPtr cl) {
    if (cl == nullptr) {
        return nullptr;
    }
    
    if (cl->is_Text_CLa()) {
        // 单元素列表
        FFFzExpPtr elem = cl->extract_Text_CLa();
        FFFzExpPtr simplified_elem = simplify(elem);
        
        if (simplified_elem != elem) {
            return make_CompatyList_CLa(simplified_elem);
        }
        return cl;  // 没有变化
    }
    
    if (cl->is_Text_CLb()) {
        auto [sub_list, elem] = cl->extract_Text_CLb();
        
        // 递归简化子列表和元素
        CompatyListPtr simplified_sub_list = simplify_compat_list(sub_list);
        FFFzExpPtr simplified_elem = simplify(elem);
        
        if (simplified_sub_list != sub_list || simplified_elem != elem) {
            return make_CompatyList_CLb(simplified_sub_list, simplified_elem);
        }
        return cl;  // 没有变化
    }
    
    // 不应该到达这里
    throw std::runtime_error("simplify_compat_list: Unexpected CompatyList type");
}

