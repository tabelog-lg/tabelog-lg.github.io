#include <string>
#include <vector>
#include <cctype>

using namespace std;

// 前置声明
bool isEntity(const string& s);
bool isMultiEntity(const string& s);
bool isSequence(const string& s);
bool isPosition(const string& s);

// 检查是否全为数字
bool isDigits(const string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit(c)) return false;
    return true;
}

// 按外层分隔符分割（忽略括号内的分隔符）
vector<string> splitByOuter(const string& s, char delim) {
    vector<string> parts;
    int paren = 0;
    size_t start = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '(') ++paren;
        else if (s[i] == ')') --paren;
        else if (s[i] == delim && paren == 0) {
            parts.push_back(s.substr(start, i - start));
            start = i + 1;
        }
    }
    if (start <= s.size())
        parts.push_back(s.substr(start));
    return parts;
}

// 实体串
bool isEntity(const string& s) {
    // 纯数字
    if (isDigits(s)) return true;

    // 形如 "ph(aaa)"
    if (s.size() >= 5 && s[0] == 'p' && s[1] == 'h' && s[2] == '(' && s.back() == ')') {
        int count = 1;
        for (size_t i = 3; i < s.size(); ++i) {
            if (s[i] == '(') ++count;
            else if (s[i] == ')') {
                --count;
                if (count == 0) {
                    if (i == s.size() - 1) {
                        string inner = s.substr(3, i - 3);
                        return isSequence(inner) || isEntity(inner) || isMultiEntity(inner);
                    }
                    return false;
                }
            }
        }
        return false; // 括号不匹配
    }
    return false;
}

// 复实体串
bool isMultiEntity(const string& s) {
    if (s.empty()) return false;
    vector<string> parts = splitByOuter(s, '+');
    if (parts.size() < 2) return false;
    for (const string& part : parts) {
        if (part.empty() || !isEntity(part))
            return false;
    }
    return true;
}

// 序列串
bool isSequence(const string& s) {
    // 纯数字
    if (isDigits(s)) return true;

    vector<string> parts = splitByOuter(s, ',');
    for (const string& part : parts)
        if (part.empty()) return false;

    // 判断是否含有 '@'
    bool hasAt = false;
    for (const string& part : parts) {
        if (part.find('@') != string::npos) {
            hasAt = true;
            break;
        }
    }

    if (!hasAt) {
        // 情况 b: 逗号分隔的实体串或复实体串
        for (const string& part : parts) {
            if (!isEntity(part) && !isMultiEntity(part))
                return false;
        }
        return true;
    } else {
        // 情况 c: 逗号分隔的 aaa@bbb
        for (const string& part : parts) {
            // 找到顶层的 '@'
            int paren = 0;
            size_t at_pos = string::npos;
            for (size_t i = 0; i < part.size(); ++i) {
                if (part[i] == '(') ++paren;
                else if (part[i] == ')') --paren;
                else if (part[i] == '@' && paren == 0) {
                    at_pos = i;
                    break;
                }
            }
            if (at_pos == string::npos || at_pos == 0 || at_pos == part.size() - 1)
                return false;
            string aaa = part.substr(0, at_pos);
            string bbb = part.substr(at_pos + 1);
            if (!isEntity(aaa) || !isPosition(bbb))
                return false;
        }
        return true;
    }
}

// 位置串
bool isPosition(const string& s) {
    // 纯数字
    if (isDigits(s)) return true;

    // 形如 "(aaa)"
    if (s.size() >= 3 && s[0] == '(' && s.back() == ')') {
        int count = 1;
        for (size_t i = 1; i < s.size(); ++i) {
            if (s[i] == '(') ++count;
            else if (s[i] == ')') {
                --count;
                if (count == 0) {
                    if (i == s.size() - 1) {
                        string inner = s.substr(1, i - 1);
                        return isSequence(inner) || isMultiEntity(inner);
                    }
                    return false;
                }
            }
        }
        return false; // 括号不匹配
    }

    // 实体串
    return isEntity(s);
}

// 主函数
bool isValid(const string& s) {
    // 移除所有空格
    string t;
    for (char c : s)
        if (!isspace(c)) t.push_back(c);

    if (t == "limit") return true;
    return isEntity(t) || isMultiEntity(t);
}

#include <iostream>

void test(const string& str, bool expected) {
    bool result = isValid(str);
    cout << "\"" << str << "\" -> " << (result ? "true" : "false");
    if (result == expected) {
        cout << " ?" << endl;
    } else {
        cout << " ? (期望 " << (expected ? "true" : "false") << ")" << endl;
    }
}

int main() {
    cout << "=== 合法字符串测试 ===" << endl;

    cout << "\n1. 实体串测试:" << endl;
    test("123", true);
    test("ph(123)", true);
    test("ph(ph(123))", true);
    test("ph(123,456)", true);
    test("ph(123+456)", true);
    test("ph(ph(123,456))", true);
    test("ph(ph(123)+ph(456))", true);

    cout << "\n2. 复实体串测试:" << endl;
    test("123+456", true);
    test("ph(123)+456", true);
    test("123+ph(456)", true);
    test("ph(123)+ph(456)", true);
    test("ph(123)+ph(456)+789", true);

    cout << "\n3. limit测试:" << endl;
    test("limit", true);

    cout << "\n=== 非法字符串测试 ===" << endl;

    cout << "\n4. 序列串测试 (不应合法):" << endl;
    test("123,456", false);
    test("123,456,789", false);
    test("ph(123)+456,789", false);
    test("123@456", false);

    cout << "\n5. 位置串测试 (不应合法):" << endl;
    test("(123)", false);
    test("(123,456)", false);
    test("(123+456)", false);

    cout << "\n6. 格式错误测试:" << endl;
    test("abc", false);
    test("ph(123", false);
    test("123+", false);
    test("123,", false);
    test("ph()", false);
    test("", false);

    cout << "\n7. 复杂嵌套测试:" << endl;
    test("ph(123@456,789@(123))", true);
    test("ph((123))", true);
    test("ph(ph(123@456))", true);

    return 0;
}
