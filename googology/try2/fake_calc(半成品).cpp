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

FFFzExpPtr calc_root(FFFzExpPtr exp) {
    if (!exp->is_Text_FEa()) {
        throw std::runtime_error("calc_root: 输入表达式必须是 Text_FEa 类型。");
    }

    SimpleExpPtr simple_exp = exp->extract_Text_FEa();

    if (simple_exp->is_Text_SEb()) {
        auto [compaty_list_ptr, fffz_exp_ptr_alpha] = simple_exp->extract_Text_SEb();
        
        std::vector<FFFzExpPtr> expanded_list;
        try {
            expanded_list = expand_CompatyList(compaty_list_ptr);
        } catch (const std::runtime_error& e) {
            return fffz_exp_ptr_alpha;
        }

        if (expanded_list.empty()) {
            return fffz_exp_ptr_alpha;
        }

        FFFzExpPtr fffz_exp_ptr_beta = expanded_list.back();

        std::vector<FFFzExpPtr> elements_to_check;
        elements_to_check.insert(elements_to_check.end(), expanded_list.begin(), expanded_list.end());
        elements_to_check.push_back(fffz_exp_ptr_alpha);

        CompatyListPtr new_list;
        try {
            new_list = combine_CompatyList(elements_to_check);
        } catch (const std::runtime_error& e) {
            return fffz_exp_ptr_alpha;
        }

        if (is_exists(new_list)) {
            return fffz_exp_ptr_beta;
        } else {
            return fffz_exp_ptr_alpha;
        }

    } else if (simple_exp->is_Text_SEa()) {
        return simple_exp->extract_Text_SEa();
    } else {
        throw std::runtime_error("calc_root: Text_FEa 内部的 SimpleExp 必须是 Text_SEa 或 Text_SEb 类型。");
    }
}

int compare(FFFzExpPtr a, FFFzExpPtr b) {
    if (a == b) {
        return 0;
    }

    bool a_is_zero = a->is_Text_0();
    bool b_is_zero = b->is_Text_0();
    
    if (!a_is_zero && b_is_zero) {
        return 1;
    }
    if (a_is_zero && !b_is_zero) {
        return -1;
    }
    if (a_is_zero && b_is_zero) {
        return 0;
    }

    auto is_psi_Z_hash_m_zero = [](FFFzExpPtr exp) -> bool {
        if (!exp->is_Text_FEa()) return false;
        SimpleExpPtr se = exp->extract_Text_FEa();
        if (!se->is_Text_SEb()) return false;
        auto [cl, inner] = se->extract_Text_SEb();
        return inner->is_Text_0();
    };

    if (is_psi_Z_hash_m_zero(a)) {
        SimpleExpPtr se_a = a->extract_Text_FEa();
        auto [cl_a, zero_a] = se_a->extract_Text_SEb();
        
        std::vector<FFFzExpPtr> expanded_cl_a = expand_CompatyList(cl_a);
        if (expanded_cl_a.empty()) {
            throw std::runtime_error("Invalid ψZ[#,m](0) form: # cannot be empty");
        }
        FFFzExpPtr m = expanded_cl_a.back();
        
        std::vector<FFFzExpPtr> hash_elements;
        if (expanded_cl_a.size() > 1) {
            hash_elements = std::vector<FFFzExpPtr>(expanded_cl_a.begin(), expanded_cl_a.end() - 1);
        }
        
        FFFzExpPtr m_expanded = expand(m, 0);
        
        FFFzExpPtr new_a;
        if (hash_elements.empty()) {
            SimpleExpPtr new_se = make_SimpleExp_SEa(m_expanded);
            new_a = make_FFFzExp_FEa(new_se);
        } else {
            CompatyListPtr new_cl = combine_CompatyList(hash_elements);
            SimpleExpPtr new_se = make_SimpleExp_SEb(new_cl, m_expanded);
            new_a = make_FFFzExp_FEa(new_se);
        }
        
        return compare(new_a, b);
    }

    if (is_psi_Z_hash_m_zero(b)) {
        int result = compare(b, a);
        return -result;
    }

    auto is_psi_Z_form = [](FFFzExpPtr exp, CompatyListPtr& hash, FFFzExpPtr& inner) -> bool {
        if (!exp->is_Text_FEa()) return false;
        SimpleExpPtr se = exp->extract_Text_FEa();
        
        if (se->is_Text_SEa()) {
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
        bool hash_equal = false;
        
        if (hash_a == nullptr && hash_b == nullptr) {
            hash_equal = true;
        } else if (hash_a != nullptr && hash_b != nullptr) {
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
            return compare(inner_a, inner_b);
        } else {
            std::vector<FFFzExpPtr> expanded_hash_a = 
                (hash_a != nullptr) ? expand_CompatyList(hash_a) : std::vector<FFFzExpPtr>();
            std::vector<FFFzExpPtr> expanded_hash_b = 
                (hash_b != nullptr) ? expand_CompatyList(hash_b) : std::vector<FFFzExpPtr>();
            
            size_t min_len = std::min(expanded_hash_a.size(), expanded_hash_b.size());
            size_t diff_index = min_len;
            
            for (size_t i = 0; i < min_len; i++) {
                if (compare(expanded_hash_a[i], expanded_hash_b[i]) != 0) {
                    diff_index = i;
                    break;
                }
            }
            
            if (diff_index < min_len) {
                FFFzExpPtr a_elem = expanded_hash_a[diff_index];
                FFFzExpPtr b_elem = expanded_hash_b[diff_index];
                return compare(a_elem, b_elem);
            } else {
                if (expanded_hash_a.size() < expanded_hash_b.size()) {
                    return 1;
                } else {
                    return -1;
                }
            }
        }
    }

    std::vector<SimpleExpPtr> expanded_a = expand_FFFzExp(a);
    std::vector<SimpleExpPtr> expanded_b = expand_FFFzExp(b);

    size_t min_len = std::min(expanded_a.size(), expanded_b.size());
    for (size_t i = 0; i < min_len; i++) {
        SimpleExpPtr se_a = expanded_a[i];
        SimpleExpPtr se_b = expanded_b[i];
        
        FFFzExpPtr wrapped_a = make_FFFzExp_FEa(se_a);
        FFFzExpPtr wrapped_b = make_FFFzExp_FEa(se_b);
        
        int cmp_result = compare(wrapped_a, wrapped_b);
        if (cmp_result != 0) {
            return cmp_result;
        }
    }
    
    if (expanded_a.size() < expanded_b.size()) {
        return -1;
    } else if (expanded_a.size() > expanded_b.size()) {
        return 1;
    }
    
    return 0;
}

bool is_less_than(FFFzExpPtr a, FFFzExpPtr b) {
    return compare(a, b) == -1;
}

bool is_equal_to(FFFzExpPtr a, FFFzExpPtr b) {
    return compare(a, b) == 0;
}

bool is_greater_than(FFFzExpPtr a, FFFzExpPtr b) {
    return compare(a, b) == 1;
}

SimpleExpPtr apply_replacements_to_simpleexp(const SimpleExpPtr& root, 
	std::map<FFFzExpPtr, FFFzExpPtr>& repl_map, 
	std::set<void*>& visited);
FFFzExpPtr apply_replacements_to_fffzexp(const FFFzExpPtr& root, 
	std::map<FFFzExpPtr, FFFzExpPtr>& repl_map, 
	std::set<void*>& visited);

struct XProcessingInfo {
    FFFzExpPtr x_ptr;
    unsigned int layer;
    size_t right_index;
    bool index_valid;
};

bool compareXInfo(const XProcessingInfo& a, const XProcessingInfo& b) {
    if (a.layer != b.layer) {
        return a.layer < b.layer;
    } else {
        size_t idx_a = a.index_valid ? a.right_index : 0;
        size_t idx_b = b.index_valid ? b.right_index : 0;
        return idx_a > idx_b;
    }
}

FFFzExpPtr calculate_replacement_for_x(FFFzExpPtr x, FFFzExpPtr s) {
    if (!x->is_Text_FEb()) {
        throw std::runtime_error("Expected x to be Text_FEb in calculate_replacement_for_x");
    }
    
    SimpleExpPtr last_simple = get_last_FFFzExp(x);
    FFFzExpPtr last_item_fffz = make_FFFzExp_FEa(last_simple);

    int cmp = compare(last_item_fffz, s);
    
    if (cmp >= 0) {
        return last_item_fffz;
    } else {
        std::vector<SimpleExpPtr> expanded = expand_FFFzExp(x);
        std::vector<SimpleExpPtr> filtered;
        
        for (const auto& item : expanded) {
            FFFzExpPtr item_fffz = make_FFFzExp_FEa(item);
            if (compare(item_fffz, s) < 0) {
                filtered.push_back(item);
            }
        }
        
        if (filtered.empty()) {
            return make_FFFzExp_0();
        } else {
            return combine_FFFzExp(filtered);
        }
    }
}

SimpleExpPtr apply_replacements_to_simpleexp(const SimpleExpPtr& root,
                                             std::map<FFFzExpPtr, FFFzExpPtr>& repl_map,
                                             std::set<void*>& visited) {
    if (visited.find(root.get()) != visited.end()) {
        return root;
    }
    visited.insert(root.get());
    
    if (root->is_Text_SEa()) {
        FFFzExpPtr inner = root->extract_Text_SEa();
        FFFzExpPtr new_inner = apply_replacements_to_fffzexp(inner, repl_map, visited);
        
        if (new_inner == inner) {
            return root;
        }
        
        return make_SimpleExp_SEa(new_inner);
    } else {
        auto [cl, fffz] = root->extract_Text_SEb();
        
        FFFzExpPtr new_fffz = apply_replacements_to_fffzexp(fffz, repl_map, visited);
        
        std::vector<FFFzExpPtr> expanded_cl;
        try {
            expanded_cl = expand_CompatyList(cl);
        } catch (const std::runtime_error&) {
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

        CompatyListPtr new_cl = cl;
        if (cl_changed) {
            try {
                new_cl = combine_CompatyList(new_expanded_cl);
            } catch (const std::runtime_error&) {
                new_cl = cl;
            }
        }

        if (new_fffz == fffz && !cl_changed) {
            return root;
        }

        return make_SimpleExp_SEb(new_cl, new_fffz);
    }
}

FFFzExpPtr apply_replacements_to_fffzexp(const FFFzExpPtr& root, 
                                         std::map<FFFzExpPtr, FFFzExpPtr>& repl_map,
                                         std::set<void*>& visited) {
    if (visited.find(root.get()) != visited.end()) {
        return root;
    }
    visited.insert(root.get());

    auto it = repl_map.find(root);
    if (it != repl_map.end()) {
        return it->second;
    }
    
    if (root->is_Text_0()) {
        return root;
    } else if (root->is_Text_FEa()) {
        SimpleExpPtr inner = root->extract_Text_FEa();
        SimpleExpPtr new_inner = apply_replacements_to_simpleexp(inner, repl_map, visited);
        if (new_inner == inner) {
            return root;
        }
        return make_FFFzExp_FEa(new_inner);
    } else {
        auto [left_se, right_ffe] = root->extract_Text_FEb();
        SimpleExpPtr new_left = apply_replacements_to_simpleexp(left_se, repl_map, visited);
        FFFzExpPtr new_right = apply_replacements_to_fffzexp(right_ffe, repl_map, visited);
        
        if (new_left == left_se && new_right == right_ffe) {
            return root;
        }
        return make_FFFzExp_FEb(new_left, new_right);
    }
}

FFFzExpPtr calc_core(FFFzExpPtr A, FFFzExpPtr s, unsigned int n) {
    if (!A->is_Text_FEa()) {
        throw std::runtime_error("A must be a legal simple expression (Text_FEa)");
    }
    if (!s->is_Text_FEa()) {
        throw std::runtime_error("s must be a non-zero simple expression (Text_FEa)");
    }
    
    SimpleExpPtr A_simple = A->extract_Text_FEa();
    
    std::set<void*> visited_for_collection;
    std::queue<FFFzExpPtr> queue;
    std::vector<FFFzExpPtr> all_text_feb_nodes;
    
    queue.push(A);
    visited_for_collection.insert(A.get());
    
    while (!queue.empty()) {
        FFFzExpPtr current = queue.front();
        queue.pop();
        
        if (current->is_Text_FEb() && is_include(A, current)) {
            all_text_feb_nodes.push_back(current);
        }
        
        auto children_set = get_children(current, visited_for_collection);
        for (const auto& child : children_set) {
            if (visited_for_collection.find(child.get()) == visited_for_collection.end()) {
                visited_for_collection.insert(child.get());
                queue.push(child);
            }
        }
    }
    
    std::vector<SimpleExpPtr> expanded_A = expand_FFFzExp(A);
    std::map<SimpleExp*, size_t> simple_exp_to_index;
    for (size_t i = 0; i < expanded_A.size(); ++i) {
        simple_exp_to_index[expanded_A[i].get()] = i;
    }
    
    std::vector<XProcessingInfo> candidate_infos;
    
    for (FFFzExpPtr x : all_text_feb_nodes) {
        try {
            SimpleExpPtr last_simple = get_last_FFFzExp(x);
            unsigned int relative_layer = calc_relative_layer(A_simple, last_simple);
            
            if (relative_layer <= n) {
                XProcessingInfo info;
                info.x_ptr = x;
                info.layer = relative_layer;
                info.index_valid = false;
                info.right_index = 0;
                
                auto it = simple_exp_to_index.find(last_simple.get());
                if (it != simple_exp_to_index.end()) {
                    info.right_index = it->second;
                    info.index_valid = true;
                }
                
                candidate_infos.push_back(info);
            }
        } catch (const std::runtime_error&) {
            continue;
        }
    }
    
    std::sort(candidate_infos.begin(), candidate_infos.end(), compareXInfo);
    
    std::map<FFFzExpPtr, FFFzExpPtr> replacement_map;
    for (const auto& info : candidate_infos) {
        FFFzExpPtr new_node = calculate_replacement_for_x(info.x_ptr, s);
        replacement_map[info.x_ptr] = new_node;
    }
    
    std::set<void*> visited_for_replacement;
    FFFzExpPtr result = apply_replacements_to_fffzexp(A, replacement_map, visited_for_replacement);
    
    return result;
}

unsigned int calc_level(FFFzExpPtr exp) {
    if (!exp->is_Text_FEa()) {
        throw std::runtime_error("calc_level: Input exp must be a non-zero simple expression (Text_FEa).");
    }

    SimpleExpPtr simple_exp = exp->extract_Text_FEa();

    FFFzExpPtr alpha;
    CompatyListPtr compat_list;

    if (simple_exp->is_Text_SEa()) {
        alpha = simple_exp->extract_Text_SEa();
        compat_list = nullptr;
    } else if (simple_exp->is_Text_SEb()) {
        auto extracted = simple_exp->extract_Text_SEb();
        compat_list = extracted.first;
        alpha = extracted.second;
    } else {
        throw std::runtime_error("calc_level: Internal error - SimpleExp is of unknown type.");
    }

    static FFFzExpPtr Text_1 = []() -> FFFzExpPtr {
        FFFzExpPtr text_0 = std::make_shared<FFFzExp>();
        SimpleExpPtr se = std::make_shared<SimpleExp>(text_0);
        return std::make_shared<FFFzExp>(se);
    }();

    FFFzExpPtr last_of_alpha = make_FFFzExp_FEa(get_last_FFFzExp(alpha));

    bool is_successor = (compare(last_of_alpha, Text_1) == 0);

    if (is_successor) {
        return 0;
    }

    CompatyListPtr combined_list;
    if (compat_list == nullptr) {
        std::vector<FFFzExpPtr> vec = {alpha};
        combined_list = combine_CompatyList(vec);
    } else {
        std::vector<FFFzExpPtr> expanded = expand_CompatyList(compat_list);
        expanded.push_back(alpha);
        combined_list = combine_CompatyList(expanded);
    }

    bool exists = is_exists(combined_list);

    if (!exists) {
        return 1;
    } else {
        return 2;
    }
}

FFFzExpPtr expand(FFFzExpPtr exp, unsigned int n) {
    static FFFzExpPtr Text_1 = []() -> FFFzExpPtr {
        FFFzExpPtr text_0 = make_FFFzExp_0();
        SimpleExpPtr se = make_SimpleExp_SEa(text_0);
        return make_FFFzExp_FEa(se);
    }();

    if (exp->is_Text_0()) {
        throw std::runtime_error("Cannot expand Text_0");
    }

    if (exp->is_Text_FEb()) {
        std::vector<SimpleExpPtr> parts = expand_FFFzExp(exp);
        
        SimpleExpPtr last_simple = parts.back();
        FFFzExpPtr last_exp = make_FFFzExp_FEa(last_simple);
        FFFzExpPtr expanded_last = expand(last_exp, n);
        
        parts.pop_back();
        
        std::vector<SimpleExpPtr> last_parts = expand_FFFzExp(expanded_last);
        
        parts.insert(parts.end(), last_parts.begin(), last_parts.end());
        
        return combine_FFFzExp(parts);
    }

    if (!exp->is_Text_FEa()) {
        throw std::runtime_error("Unexpected expression type in expand");
    }

    SimpleExpPtr simple_exp = exp->extract_Text_FEa();
    
    CompatyListPtr compat_list = nullptr;
    FFFzExpPtr alpha = nullptr;
    
    if (simple_exp->is_Text_SEa()) {
        alpha = simple_exp->extract_Text_SEa();
    } else if (simple_exp->is_Text_SEb()) {
        auto extracted = simple_exp->extract_Text_SEb();
        compat_list = extracted.first;
        alpha = extracted.second;
    } else {
        throw std::runtime_error("Unexpected SimpleExp type");
    }

    FFFzExpPtr last_of_alpha = make_FFFzExp_FEa(get_last_FFFzExp(alpha));
    bool is_alpha_successor = (compare(last_of_alpha, Text_1) == 0);

    if (is_alpha_successor) {
        std::vector<SimpleExpPtr> alpha_parts = expand_FFFzExp(alpha);
        FFFzExpPtr beta = nullptr;
        
        if (alpha_parts.empty()) {
            beta = make_FFFzExp_0();
        } else {
            alpha_parts.pop_back();
            if (alpha_parts.empty()) {
                beta = make_FFFzExp_0();
            } else {
                beta = combine_FFFzExp(alpha_parts);
            }
        }
        
        if (n == 0) {
            return make_FFFzExp_0();
        } else {
            FFFzExpPtr prev = expand(exp, n-1);
            
            SimpleExpPtr term2_simple = nullptr;
            if (compat_list == nullptr) {
                term2_simple = make_SimpleExp_SEa(beta);
            } else {
                term2_simple = make_SimpleExp_SEb(compat_list, beta);
            }
            FFFzExpPtr term2 = make_FFFzExp_FEa(term2_simple);
            
            std::vector<SimpleExpPtr> prev_parts = expand_FFFzExp(prev);
            std::vector<SimpleExpPtr> term2_parts = expand_FFFzExp(term2);
            
            std::vector<SimpleExpPtr> all_parts;
            all_parts.reserve(prev_parts.size() + term2_parts.size());
            all_parts.insert(all_parts.end(), prev_parts.begin(), prev_parts.end());
            all_parts.insert(all_parts.end(), term2_parts.begin(), term2_parts.end());
            
            return combine_FFFzExp(all_parts);
        }
    }
    
    CompatyListPtr combined_list = nullptr;
    if (compat_list == nullptr) {
        std::vector<FFFzExpPtr> vec = {alpha};
        combined_list = combine_CompatyList(vec);
    } else {
        std::vector<FFFzExpPtr> vec = expand_CompatyList(compat_list);
        vec.push_back(alpha);
        combined_list = combine_CompatyList(vec);
    }
    
    bool exists = is_exists(combined_list);
    
    if (exists) {
        if (n == 0) {
            return alpha;
        } else {
            FFFzExpPtr prev = expand(exp, n-1);
            
            SimpleExpPtr new_simple = make_SimpleExp_SEb(combined_list, prev);
            return make_FFFzExp_FEa(new_simple);
        }
    }

    if (compat_list == nullptr) {
        throw std::runtime_error("Cannot apply rule: compat_list is empty and [#,α] does not exist");
    }
    
    std::vector<FFFzExpPtr> compat_parts = expand_CompatyList(compat_list);
    if (compat_parts.empty()) {
        throw std::runtime_error("compat_list cannot be empty in this context");
    }
    
    FFFzExpPtr beta = compat_parts.back();
    compat_parts.pop_back();
    
    CompatyListPtr triple_list = nullptr;
    {
        std::vector<FFFzExpPtr> triple_vec = compat_parts;
        triple_vec.push_back(beta);
        triple_vec.push_back(alpha);
        triple_list = combine_CompatyList(triple_vec);
    }
    
    bool triple_exists = is_exists(triple_list);
    
    if (triple_exists) {
        throw std::runtime_error("Unexpected: [#,β,α] exists");
    }
    
    CompatyListPtr new_compat = nullptr;
    if (compat_parts.empty()) {
        std::vector<FFFzExpPtr> beta_vec = {beta};
        new_compat = combine_CompatyList(beta_vec);
    } else {
        std::vector<FFFzExpPtr> new_vec = compat_parts;
        new_vec.push_back(beta);
        new_compat = combine_CompatyList(new_vec);
    }
    
    int cmp = compare(beta, alpha);
    
    if (cmp >= 0) {
        FFFzExpPtr alpha_expanded = expand(alpha, n);
        SimpleExpPtr new_simple = make_SimpleExp_SEb(new_compat, alpha_expanded);
        return make_FFFzExp_FEa(new_simple);
    } else {
        unsigned int k = 0;
        while (true) {
            FFFzExpPtr alpha_k = expand(alpha, k);
            if (compare(alpha_k, beta) >= 0) {
                break;
            }
            k++;
        }
        
        FFFzExpPtr alpha_expanded = expand(alpha, n + k);
        SimpleExpPtr new_simple = make_SimpleExp_SEb(new_compat, alpha_expanded);
        return make_FFFzExp_FEa(new_simple);
    }
}

SimpleExpPtr simplify_simple_exp(SimpleExpPtr se);
CompatyListPtr simplify_compat_list(CompatyListPtr cl);

FFFzExpPtr simplify(FFFzExpPtr exp) {
    if (exp->is_Text_FEb()) {
        auto [left_simple, right_exp] = exp->extract_Text_FEb();
        
        SimpleExpPtr simplified_left = simplify_simple_exp(left_simple);
        
        FFFzExpPtr simplified_right = simplify(right_exp);
        
        if (simplified_right->is_Text_0()) {
            return make_FFFzExp_FEa(simplified_left);
        }
        
        return make_FFFzExp_FEb(simplified_left, simplified_right);
    }
    
    if (exp->is_Text_FEa()) {
        SimpleExpPtr simple_exp = exp->extract_Text_FEa();
        SimpleExpPtr simplified_simple = simplify_simple_exp(simple_exp);
        
        if (simplified_simple != simple_exp) {
            return make_FFFzExp_FEa(simplified_simple);
        }
        return exp;
    }

    if (exp->is_Text_0()) {
        return exp;
    }

    throw std::runtime_error("simplify: Unexpected expression type");
}

SimpleExpPtr simplify_simple_exp(SimpleExpPtr se) {
    if (se->is_Text_SEa()) {
        FFFzExpPtr inner = se->extract_Text_SEa();
        FFFzExpPtr simplified_inner = simplify(inner);
        
        if (simplified_inner != inner) {
            return make_SimpleExp_SEa(simplified_inner);
        }
        return se;
    }
    
    if (se->is_Text_SEb()) {
        auto [compat_list, alpha] = se->extract_Text_SEb();

        FFFzExpPtr simplified_alpha = simplify(alpha);

        CompatyListPtr simplified_compat_list = simplify_compat_list(compat_list);

        FFFzExpPtr beta = nullptr;
        CompatyListPtr new_compat_list = simplified_compat_list;
        
        if (simplified_compat_list != nullptr) {
            std::vector<FFFzExpPtr> compat_parts = expand_CompatyList(simplified_compat_list);
            
            if (!compat_parts.empty()) {
                beta = compat_parts.back();

                if (compare(beta, simplified_alpha) > 0) {

                    compat_parts.pop_back();
                    
                    if (compat_parts.empty()) {
                        return make_SimpleExp_SEa(simplified_alpha);
                    } else {
                        new_compat_list = combine_CompatyList(compat_parts);
                        return make_SimpleExp_SEb(new_compat_list, simplified_alpha);
                    }
                }
            }
        }
        
        if (simplified_alpha != alpha || simplified_compat_list != compat_list) {
            return make_SimpleExp_SEb(simplified_compat_list, simplified_alpha);
        }
        
        return se;
    }

    throw std::runtime_error("simplify_simple_exp: Unexpected SimpleExp type");
}

CompatyListPtr simplify_compat_list(CompatyListPtr cl) {
    if (cl == nullptr) {
        return nullptr;
    }
    
    if (cl->is_Text_CLa()) {
        FFFzExpPtr elem = cl->extract_Text_CLa();
        FFFzExpPtr simplified_elem = simplify(elem);
        
        if (simplified_elem != elem) {
            return make_CompatyList_CLa(simplified_elem);
        }
        return cl;
    }
    
    if (cl->is_Text_CLb()) {
        auto [sub_list, elem] = cl->extract_Text_CLb();

        CompatyListPtr simplified_sub_list = simplify_compat_list(sub_list);
        FFFzExpPtr simplified_elem = simplify(elem);
        
        if (simplified_sub_list != sub_list || simplified_elem != elem) {
            return make_CompatyList_CLb(simplified_sub_list, simplified_elem);
        }
        return cl;
    }

    throw std::runtime_error("simplify_compat_list: Unexpected CompatyList type");
}

#endif /* FAKE_DEFINE_HPP */
