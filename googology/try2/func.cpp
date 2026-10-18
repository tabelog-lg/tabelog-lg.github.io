// 兼容 

bool is_exists_binary(FFFzExpPtr a, FFFzExpPtr b);
bool is_exists_multiary(const std::vector<FFFzExpPtr>& vec);

// is_exists 主函数
bool is_exists(CompatyListPtr col) {
    if (col == nullptr) {
        throw std::runtime_error("is_exists: CompatyList pointer is null");
    }
    
    // 展开 CompatibleList 为向量
    std::vector<FFFzExpPtr> elements = expand_CompatyList(col);
    
    // 检查向量是否为空
    if (elements.empty()) {
        throw std::runtime_error("is_exists: Expanded vector is empty");
    }
    
    // 根据元素个数分派到不同的处理方法
    if (elements.size() == 1) {
        // 单个元素的情况：直接返回 true
        return true;
    } else if (elements.size() == 2) {
        // 两个元素的情况：调用二元兼容性检查
        return is_exists_binary(elements[0], elements[1]);
    } else {
        // 多个元素的情况：调用多元兼容性检查
        return is_exists_multiary(elements);
    }
}
