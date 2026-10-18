#ifndef FAKE_CALC_HPP
#define FAKE_CALC_HPP

#include "fake_basis.hpp"
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <string>

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

#endif /* FAKE_DEFINE_HPP */
