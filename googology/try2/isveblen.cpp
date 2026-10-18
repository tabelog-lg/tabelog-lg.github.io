#include <string>
#include <vector>
#include <cctype>

using namespace std;

bool is_ent(const string& s);
bool is_mulent(const string& s);
bool is_seq(const string& s);
bool is_pos(const string& s);

bool is_nums(const string& s) {
    if (s.empty()) return 0;
    for (char c : s)
        if (!isdigit(c)) return 0;
    return 1;
}

vector<string> spl(const string& s, char c) {
    vector<string> pt;
    int pr = 0;
    size_t st = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '(') ++pr;
        else if (s[i] == ')') --pr;
        else if (s[i] == c && pr == 0) {
            pt.push_back(s.substr(st, i - st));
            st = i + 1;
        }
    }
    if (st <= s.size())
        pt.push_back(s.substr(st));
    return pt;
}

bool is_ent(const string& s) {
    if (is_nums(s)) return 1;
    if (s.size() >= 5 && s[0] == 'p' && s[1] == 'h' && s[2] == '(' && s.back() == ')') {
        int cnt = 1;
        for (size_t i = 3; i < s.size(); ++i) {
            if (s[i] == '(') ++cnt;
            else if (s[i] == ')') {
                --cnt;
                if (cnt == 0) {
                    if (i == s.size() - 1) {
                        string inn = s.substr(3, i - 3);
                        return is_seq(inn) || is_ent(inn) || is_mulent(inn);
                    }
                    return 0;
                }
            }
        }
        return 0;
    }
    return 0;
}

bool is_mulent(const string& s) {
    if (s.empty()) return 0;
    vector<string> pt = spl(s, '+');
    if (pt.size() < 2) return 0;
    for (const string& p : pt) {
        if (p.empty() || !is_ent(p))
            return 0;
    }
    return 1;
}

bool is_seq(const string& s) {
    if (is_nums(s)) return 1;
    vector<string> pt = spl(s, ',');
    for (const string& p : pt)
        if (p.empty()) return 0;
    bool at = 0;
    for (const string& p : pt) {
        if (p.find('@') != string::npos) {
            at = 1;
            break;
        }
    }
    if (!at) {
        for (const string& p : pt) {
            if (!is_ent(p) && !is_mulent(p))
                return 0;
        }
        return 1;
    } else {
        for (const string& p : pt) {
            int pr = 0;
            size_t at_pos = string::npos;
            for (size_t i = 0; i < p.size(); ++i) {
                if (p[i] == '(') ++pr;
                else if (p[i] == ')') --pr;
                else if (p[i] == '@' && pr == 0) {
                    at_pos = i;
                    break;
                }
            }
            if (at_pos == string::npos || at_pos == 0 || at_pos == p.size() - 1)
                return 0;
            string ta = p.substr(0, at_pos);
            string tb = p.substr(at_pos + 1);
            if (!is_ent(ta) || !is_pos(tb))
                return 0;
        }
        return 1;
    }
}

bool is_pos(const string& s) {
    if (is_nums(s)) return 1;
    if (s.size() >= 3 && s[0] == '(' && s.back() == ')') {
        int cnt = 1;
        for (size_t i = 1; i < s.size(); ++i) {
            if (s[i] == '(') ++cnt;
            else if (s[i] == ')') {
                --cnt;
                if (cnt == 0) {
                    if (i == s.size() - 1) {
                        string inn = s.substr(1, i - 1);
                        return is_seq(inn) || is_mulent(inn);
                    }
                    return 0;
                }
            }
        }
        return 0;
    }
    return is_ent(s);
}

bool is_valid(const string& s) {
    string t;
    for (char c : s)
        if (!isspace(c)) t.push_back(c);
    if (t == "limit") return 1;
    return is_ent(t) || is_mulent(t);
}

#include <iostream>

void test(const string& str, bool exp) {
    bool res = is_valid(str);
    cout << "\"" << str << "\" -> " << (res ? "true" : "false");
    if (res == exp) {
        cout << " ?" << endl;
    } else {
        cout << " ? (expectation " << (exp ? "true" : "false") << ")" << endl;
    }
}

int main() {
    cout << "test" << endl;

    cout << "\n(1)" << endl;
    test("123", true);
    test("ph(123)", true);
    test("ph(ph(123))", true);
    test("ph(123,456)", true);
    test("ph(123+456)", true);
    test("ph(ph(123,456))", true);
    test("ph(ph(123)+ph(456))", true);

    cout << "\n(2)" << endl;
    test("123+456", true);
    test("ph(123)+456", true);
    test("123+ph(456)", true);
    test("ph(123)+ph(456)", true);
    test("ph(123)+ph(456)+789", true);

    cout << "\n(3)" << endl;
    test("limit", true);

    cout << "\n(4)" << endl;
    test("123,456", false);
    test("123,456,789", false);
    test("ph(123)+456,789", false);
    test("123@456", false);

    cout << "\n(5)" << endl;
    test("(123)", false);
    test("(123,456)", false);
    test("(123+456)", false);

    cout << "\n(6)" << endl;
    test("abc", false);
    test("ph(123", false);
    test("123+", false);
    test("123,", false);
    test("ph()", false);
    test("", false);

    cout << "\n(7)" << endl;
    test("ph(123@456,789@(123))", true);
    test("ph((123))", false);
    test("ph(ph(123@456))", true);
    
    return 0;
}
