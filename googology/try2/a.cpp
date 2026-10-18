#include <vector>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <sstream>

// 主接口
class IBase {
public:
    virtual ~IBase() = default;
    virtual std::string expand(std::string expr, int n) = 0;
    virtual bool hasSpecialMethod() const { return false; }
    virtual void specialMethod() {} // 默认空实现
};

/*

// 具体类1：实现了特殊方法
class ConcreteA : public IBase {
public:
    void process() override {
        std::cout << "ConcreteA::process()\n";
    }
    
    bool hasSpecialMethod() const override { return true; }
    
    void specialMethod() override {
        std::cout << "ConcreteA::specialMethod()\n";
    }
};

// 具体类2：没有实现特殊方法
class ConcreteB : public IBase {
public:
    void process() override {
        std::cout << "ConcreteB::process()\n";
    }
    
    // 不重写 hasSpecialMethod 和 specialMethod，使用基类默认实现
};

*/

// vector<int> -> "1,2,3,4"
std::string seq_to_str(const std::vector<int>& vec) {
    if (vec.empty()) return "";
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) oss << ",";
        oss << vec[i];
    }
    return oss.str();
}

// "1,2,3,4" -> vector<int>
std::vector<int> str_to_seq(const std::string& str) {
    std::vector<int> result;
    if (str.empty()) return result;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, ',')) {
        if (!token.empty()) {          // 忽略空字段（如连续逗号）
            result.push_back(stoi(token));
        }
    }
    return result;
}

class PrSS : public IBase {
public:
	std::string expand(std::string expr, int n);
private:
	std::vector<int> expand_i(std::vector<int> expr, int n);
};

std::vector<int> PrSS::expand_i(std::vector<int> expr, int n) {
	int r = -1;
    if (!expr.empty()) {
        int last = expr.back();
        for (int i = 1; i <= (int)expr.size(); ++i) {
            if (expr[expr.size() - i] < last) {
                r = expr.size() - i;
                break;
            }
        }
    }

    std::vector<int> bad_part;
    if (r != -1) {
        for (int i = r; i < (int)expr.size() - 1; ++i) {
            bad_part.push_back(expr[i]);
        }
    }

    std::vector<int> badr;
    for (int t = 0; t < n; ++t) {
        badr.insert(badr.end(), bad_part.begin(), bad_part.end());
    }

    std::vector<int> good_part;
    if (r != -1 && r != 0) {
        for (int i = 0; i < r; ++i) {
            good_part.push_back(expr[i]);
        }
    }

    if (!good_part.empty()) {
        good_part.insert(good_part.end(), badr.begin(), badr.end());
        return good_part;
    } else {
        return badr;
    }
}

std::string PrSS::expand(std::string expr, int n) {
	return seq_to_str(expand_i(str_to_seq(expr),n));
}

// 主处理器
class Processor {
private:
    std::unique_ptr<IBase> cur_handler;
    std::unordered_map<std::string, std::unique_ptr<IBase>> handlers;
    
public:
    Processor() {
        // 注册所有可用处理器
        handlers["PrSS"] = std::make_unique<PrSS>();
        //handlers["B"] = std::make_unique<ConcreteB>();
    }
    
    void set_handler(const std::string& name) {
        if (handlers.find(name) != handlers.end()) {
            // 创建新实例，避免状态共享
            cur_handler = std::move(handlers[name]);
            handlers[name] = create_handler(name);
        }
    }
    
    void execute() {
        if (!cur_handler) {
            std::cout << "No handler selected\n";
            return;
        }
        
        // 主处理流程
        std::cout << "=== Starting processing ===\n";
        
        /*
        
        // 一些预处理
        cur_handler->process();
        
        // 条件处理
        if (cur_handler->hasSpecialMethod()) {
            std::cout << "Handler has special method, executing...\n";
            cur_handler->specialMethod();
        } else {
            std::cout << "No special method available, using alternative\n";
            // 其他处理逻辑
        }
        
        // 一些后处理
        std::cout << "=== Processing complete ===\n";
        
        */
        std::string s = "1,2,3,4,5";
        std::cout << cur_handler->expand(s,3);
    }
    
    // 获取所有可用处理器类型
    void list_handler() const {
        std::cout << "Available handlers:\n";
        for (const auto& [name, _] : handlers) {
            std::cout << "  - " << name << "\n";
        }
    }
    
private:
    std::unique_ptr<IBase> create_handler(const std::string& name) {
        if (name == "PrSS") return std::make_unique<PrSS>();
        //if (name == "B") return std::make_unique<ConcreteB>();
        return nullptr;
    }
};

int main() {
    // 创建处理器实例
    Processor processor;
    
    // 显示所有可用的处理器类型
    processor.list_handler();
    
    processor.set_handler("PrSS");
    processor.execute();
    /*
    std::cout << "\n=== 测试处理器A ===\n";
    // 切换到处理器A
    processor.set_handler("A");
    // 执行处理
    processor.execute();
    
    std::cout << "\n=== 测试处理器B ===\n";
    // 切换到处理器B
    processor.set_handler("B");
    // 执行处理
    processor.execute();
    
    std::cout << "\n=== 运行时切换测试 ===\n";
    // 模拟在一轮运行结束后切换处理器
    processor.set_handler("A");
    processor.execute();
    
    std::cout << "\n切换处理器...\n";
    processor.set_handler("B");
    processor.execute();
    
    std::cout << "\n再次切换回A...\n";
    processor.set_handler("A");
    processor.execute();
    */
    return 0;
}
