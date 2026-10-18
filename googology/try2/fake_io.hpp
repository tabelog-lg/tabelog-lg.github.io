#ifndef FAKE_IO_HPP
#define FAKE_IO_HPP

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cctype>
#include <algorithm>
#include "fake_basis.hpp"

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

#endif /* FAKE_IO_HPP */
