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

std::string convert_sub_seq(const std::vector<unsigned int>& seq, size_t start, size_t end);
std::string convert_full_seq(const std::vector<unsigned int>& seq);

std::string convert_sub_seq(const std::vector<unsigned int>& seq, size_t start, size_t end) {
    if (end - start == 1) {
        return "Z(0)";
    }
    
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
    
    std::vector<unsigned int> newSeq;
    for (size_t i = start + 1; i < end; i++) {
        newSeq.push_back(seq[i] - 1);
    }
    
    std::string inner = convert_full_seq(newSeq);
    return "P(" + inner + ")";
}

std::string convert_full_seq(const std::vector<unsigned int>& seq) {
    if (seq.empty()) {
        return "";
    }
    
    if (seq[0] != 0) {
        throw std::invalid_argument("序列必须以0开头");
    }
    
    for (size_t i = 0; i < seq.size() - 1; i++) {
        if (seq[i + 1] > seq[i] + 1) {
            throw std::invalid_argument("序列不满足条件：后一位必须≤前一位+1");
        }
    }
    
    std::vector<std::string> parts;
    size_t start = 0;
    
    for (size_t i = 1; i <= seq.size(); i++) {
        if (i == seq.size() || seq[i] == 0) {
            if (i > start) {
                parts.push_back(convert_sub_seq(seq, start, i));
            }
            start = i;
        }
    }
    
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


std::string replace_numbers_P(const std::string& s) {
    std::string step1;
    size_t i = 0;
    while (i < s.size()) {
        if (std::isdigit(s[i])) {
            size_t j = i;
            while (j < s.size() && std::isdigit(s[j])) {
                j++;
            }
            std::string numStr = s.substr(i, j - i);
            unsigned long long num = std::stoull(numStr);
            
            if (num == 0) {
                step1 += numStr;
            } else {
                if (num > 0) {
                    step1 += "Z(0)";
                    for (unsigned long long k = 1; k < num; k++) {
                        step1 += "+Z(0)";
                    }
                }
            }
            i = j;
        } else {
            step1.push_back(s[i]);
            i++;
        }
    }
    
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

std::string replace_number_seq(const std::string& s) {
    std::string result;
    size_t i = 0;
    size_t n = s.size();
    
    while (i < n) {
        if (std::isdigit(s[i])) {
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
                try {
                    std::vector<unsigned int> seq = parse_seq_str(numSeqStr);
                    
                    if (!seq.empty() && seq[0] == 0) {
                        bool valid = true;
                        for (size_t k = 0; k < seq.size() - 1; k++) {
                            if (seq[k+1] > seq[k] + 1) {
                                valid = false;
                                break;
                            }
                        }
                        
                        if (valid) {
                            std::string converted = replace_numbers_P(convert(seq));
                            result += converted;
                            i = j;
                            continue;
                        }
                    }
                } catch (const std::exception&) { }
            } else {
                try {
                    unsigned int num = std::stoul(numSeqStr);
                    result += replace_single_number(num);
                    i = j;
                    continue;
                } catch (const std::exception&) { }
            }
            
            result += numSeqStr;
            i = j;
        } else {
            result.push_back(s[i]);
            i++;
        }
    }
    
    return result;
}

FFFzExpPtr to_exp(const std::string& str);
static FFFzExpPtr parse_FFFzExp(const std::string& str);
static SimpleExpPtr parse_SimpleExp(const std::string& str);
static CompatyListPtr parse_CompatyList(const std::string& str);

FFFzExpPtr to_exp(const std::string& str) {
    std::string s = str;
    auto start = s.find_first_not_of(" \t\n\r");
    auto end = s.find_last_not_of(" \t\n\r");
    if (start == std::string::npos) {
        throw std::runtime_error("Error in to_exp: Input string is empty or contains only whitespace.");
    }
    s = s.substr(start, end - start + 1);

    return parse_FFFzExp(s);
}

static FFFzExpPtr parse_FFFzExp(const std::string& str) {
    if (str == "0") {
        return make_FFFzExp_0();
    }

    std::vector<SimpleExpPtr> simple_exps;
    size_t start_pos = 0;
    int bracket_depth = 0;
    int square_bracket_depth = 0;

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
            square_bracket_depth++;
        } else if (c == ']') {
            if (square_bracket_depth <= 0) {
                throw std::runtime_error("Error in to_exp: Unmatched ']' in FFFzExp parsing.");
            }
            square_bracket_depth--;
        } else if (c == '+') {
            if (bracket_depth == 0 && square_bracket_depth == 0) {
                std::string part = str.substr(start_pos, i - start_pos);
                if (part.empty()) {
                    throw std::runtime_error("Error in to_exp: Empty SimpleExp segment before '+'.");
                }
                simple_exps.push_back(parse_SimpleExp(part));
                start_pos = i + 1;
            }
        }
    }

    std::string last_part = str.substr(start_pos);
    if (last_part.empty()) {
        throw std::runtime_error("Error in to_exp: Empty SimpleExp segment at the end.");
    }
    simple_exps.push_back(parse_SimpleExp(last_part));

    if (bracket_depth != 0 || square_bracket_depth != 0) {
        throw std::runtime_error("Error in to_exp: Unmatched brackets in FFFzExp parsing.");
    }

    return combine_FFFzExp(simple_exps);
}

static SimpleExpPtr parse_SimpleExp(const std::string& str) {
    if (str.length() < 3 || str[0] != 'Z') {
        throw std::runtime_error("Error in to_exp: Invalid SimpleExp format, must start with 'Z'.");
    }

    size_t index = 1;

    if (str[index] == '(') {
        int depth = 1;
        size_t end_pos = index + 1;
        for (; end_pos < str.length() && depth > 0; ++end_pos) {
            if (str[end_pos] == '(') depth++;
            else if (str[end_pos] == ')') depth--;
        }
        if (depth != 0) {
            throw std::runtime_error("Error in to_exp: Unmatched parentheses in Text_SEa parsing.");
        }
        size_t content_start = index + 1;
        size_t content_len = (end_pos - 1) - content_start;
        std::string inner = str.substr(content_start, content_len);
        FFFzExpPtr inner_exp = parse_FFFzExp(inner);
        return make_SimpleExp_SEa(inner_exp);

    } else if (str[index] == '[') {
        int depth = 1;
        size_t list_end = index + 1;
        for (; list_end < str.length() && depth > 0; ++list_end) {
            if (str[list_end] == '[') depth++;
            else if (str[list_end] == ']') depth--;
        }
        if (depth != 0) {
            throw std::runtime_error("Error in to_exp: Unmatched '[' in Text_SEb parsing.");
        }
        list_end--;
        std::string list_str = str.substr(index + 1, list_end - (index + 1));
        CompatyListPtr cl = parse_CompatyList(list_str);

        if (list_end + 1 >= str.length() || str[list_end + 1] != '(') {
            throw std::runtime_error("Error in to_exp: Expected '(' after ']' in Text_SEb.");
        }
        int paren_depth = 1;
        size_t exp_end = list_end + 2;
        for (; exp_end < str.length() && paren_depth > 0; ++exp_end) {
            if (str[exp_end] == '(') paren_depth++;
            else if (str[exp_end] == ')') paren_depth--;
        }
        if (paren_depth != 0) {
            throw std::runtime_error("Error in to_exp: Unmatched '(' in Text_SEb parsing.");
        }
        exp_end--;
        std::string exp_str = str.substr(list_end + 2, exp_end - (list_end + 2));
        FFFzExpPtr fe = parse_FFFzExp(exp_str);

        if (exp_end + 1 != str.length()) {
            throw std::runtime_error("Error in to_exp: Extra characters after ')' in Text_SEb.");
        }

        return make_SimpleExp_SEb(cl, fe);
    } else {
        throw std::runtime_error("Error in to_exp: Invalid SimpleExp format, expected '(' or '[' after 'Z'.");
    }
}

static CompatyListPtr parse_CompatyList(const std::string& str) {
    if (str.empty()) {
        throw std::runtime_error("Error in to_exp: CompatyList string cannot be empty.");
    }

    std::vector<FFFzExpPtr> fffz_exps;
    size_t start_pos = 0;
    int bracket_depth = 0;
    int square_bracket_depth = 0;

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
                std::string part = str.substr(start_pos, i - start_pos);
                if (part.empty()) {
                    throw std::runtime_error("Error in to_exp: Empty segment in CompatyList before ','.");
                }
                fffz_exps.push_back(parse_FFFzExp(part));
                start_pos = i + 1;
            }
        }
    }

    std::string last_part = str.substr(start_pos);
    if (last_part.empty()) {
        throw std::runtime_error("Error in to_exp: Empty segment at the end of CompatyList.");
    }
    fffz_exps.push_back(parse_FFFzExp(last_part));

    if (bracket_depth != 0 || square_bracket_depth != 0) {
        throw std::runtime_error("Error in to_exp: Unmatched brackets in CompatyList parsing.");
    }

    return combine_CompatyList(fffz_exps);
}

FFFzExpPtr to_FFFzExp(const std::string& input) {
    std::string s = input;
    
    std::string allowedChars = "pZz()[]0123456789+,. ";
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

    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
    s.erase(std::remove(s.begin(), s.end(), 'p'), s.end());

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
