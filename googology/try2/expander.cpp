#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

// 主接口
class IBase {
public:
    virtual ~IBase() = default;
    virtual void process() = 0;
    virtual bool hasSpecialMethod() const = 0;
    virtual void specialMethod() {} // 默认空实现
    virtual const std::string& getClassName() const = 0;
};

// 自动注册模板基类
template<typename Base>
class AutoRegister {
public:
    // 工厂类
    class Factory {
    public:
        static Factory& instance() {
            static Factory factory;
            return factory;
        }
        
        void register_creator(const std::string& name, 
                           std::function<std::unique_ptr<Base>()> creator) {
            creators[name] = creator;
        }
        
        std::unique_ptr<Base> create(const std::string& name) {
            if (auto it = creators.find(name); it != creators.end()) {
                return it->second();
            }
            return nullptr;
        }
        
        std::vector<std::string> get_class_names() const {
            std::vector<std::string> names;
            for (const auto& [name, _] : creators) {
                names.push_back(name);
            }
            return names;
        }
        
    private:
        std::unordered_map<std::string, 
                          std::function<std::unique_ptr<Base>()>> creators;
    };
};

// 具体类1：实现了特殊方法
class ConcreteA : public IBase, public AutoRegister<IBase> {
public:
    static constexpr const char* CLASS_NAME = "ConcreteA";
    
    void process() override {
        std::cout << "ConcreteA::process() - 处理A类型数据\n";
    }
    
    bool hasSpecialMethod() const override { return true; }
    
    void specialMethod() override {
        std::cout << "ConcreteA::specialMethod() - 执行A的特殊方法\n";
    }
    
    const std::string& getClassName() const override {
        static std::string name = CLASS_NAME;
        return name;
    }
    
private:
    // 静态注册器
    static bool registerClass() {
        Factory::instance().register_creator(CLASS_NAME, []() {
            return std::make_unique<ConcreteA>();
        });
        return true;
    }
    
    static bool registered;
};

// 静态成员初始化
bool ConcreteA::registered = ConcreteA::registerClass();

// 具体类2：没有实现特殊方法
class ConcreteB : public IBase, public AutoRegister<IBase> {
public:
    static constexpr const char* CLASS_NAME = "ConcreteB";
    
    void process() override {
        std::cout << "ConcreteB::process() - 处理B类型数据\n";
    }
    
    bool hasSpecialMethod() const override { return false; }
    
    const std::string& getClassName() const override {
        static std::string name = CLASS_NAME;
        return name;
    }
    
private:
    static bool registerClass() {
        Factory::instance().register_creator(CLASS_NAME, []() {
            return std::make_unique<ConcreteB>();
        });
        return true;
    }
    
    static bool registered;
};

bool ConcreteB::registered = ConcreteB::registerClass();

// 具体类3：实现了特殊方法且有额外功能
class ConcreteC : public IBase, public AutoRegister<IBase> {
public:
    static constexpr const char* CLASS_NAME = "ConcreteC";
    
    void process() override {
        std::cout << "ConcreteC::process() - 处理C类型数据\n";
    }
    
    bool hasSpecialMethod() const override { return true; }
    
    void specialMethod() override {
        std::cout << "ConcreteC::specialMethod() - 执行C的特殊方法（增强版）\n";
    }
    
    const std::string& getClassName() const override {
        static std::string name = CLASS_NAME;
        return name;
    }
    
private:
    static bool registerClass() {
        Factory::instance().register_creator(CLASS_NAME, []() {
            return std::make_unique<ConcreteC>();
        });
        return true;
    }
    
    static bool registered;
};

bool ConcreteC::registered = ConcreteC::registerClass();

// 主处理器
class Processor {
private:
    std::unique_ptr<IBase> currentHandler;
    
public:
    void setHandler(const std::string& name) {
        currentHandler = AutoRegister<IBase>::Factory::instance().create(name);
        if (currentHandler) {
            std::cout << "切换到处理器: " << name << std::endl;
        } else {
            std::cout << "未找到处理器: " << name << std::endl;
        }
    }
    
    void execute() {
        if (!currentHandler) {
            std::cout << "未选择处理器，请先使用setHandler设置处理器\n";
            return;
        }
        
        std::cout << "\n=== 开始处理 (" << currentHandler->getClassName() << ") ===\n";
        
        // 一些预处理
        std::cout << "步骤1: 初始化处理环境\n";
        
        // 主处理
        currentHandler->process();
        
        // 条件处理
        if (currentHandler->hasSpecialMethod()) {
            std::cout << "步骤2: 处理器支持特殊方法，执行中...\n";
            currentHandler->specialMethod();
        } else {
            std::cout << "步骤2: 处理器不支持特殊方法，使用标准流程\n";
            // 其他处理逻辑
            std::cout << "执行标准备用流程...\n";
        }
        
        // 一些后处理
        std::cout << "步骤3: 清理和结束处理\n";
        std::cout << "=== 处理完成 (" << currentHandler->getClassName() << ") ===\n\n";
    }
    
    void list_available_handlers() const {
        auto names = AutoRegister<IBase>::Factory::instance().get_class_names();
        std::cout << "可用的处理器:\n";
        for (const auto& name : names) {
            std::cout << "  - " << name << "\n";
        }
    }
};

// 主函数
int main() {
    Processor processor;
    
    // 列出所有可用处理器
    std::cout << "=== 可用处理器列表 ===\n";
    processor.list_available_handlers();
    
    // 测试处理器A
    std::cout << "\n=== 测试处理器A ===\n";
    processor.setHandler("ConcreteA");
    processor.execute();
    
    // 测试处理器B
    std::cout << "\n=== 测试处理器B ===\n";
    processor.setHandler("ConcreteB");
    processor.execute();
    
    // 测试处理器C
    std::cout << "\n=== 测试处理器C ===\n";
    processor.setHandler("ConcreteC");
    processor.execute();
    
    // 测试不存在的处理器
    std::cout << "\n=== 测试不存在的处理器 ===\n";
    processor.setHandler("NonExistent");
    
    // 动态切换处理器
    std::cout << "\n=== 动态切换演示 ===\n";
    processor.setHandler("ConcreteA");
    processor.execute();
    
    std::cout << "运行时切换到处理器C...\n";
    processor.setHandler("ConcreteC");
    processor.execute();
    
    return 0;
}
