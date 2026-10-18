#include <iostream>
#include <vector>

int main() {
    // 常数 C = 70375592 * 83
    const long long C = 70375592LL * 83;
    // 目标区间 [8e15, 1e16]
    const long long LOWER = 8000000000000000LL;
    const long long UPPER = 10000000000000000LL;

    // 计算 n 的理论范围
    long long low = 1548500;   // 向上取整
    long long high = 1548525;            // 向下取整

    std::cout << "搜索范围: n ∈ [" << low << ", " << high << "]" << std::endl;

    // 所有小于100的质数
    int small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37,
                          41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
    int prime_count = sizeof(small_primes) / sizeof(small_primes[0]);

    std::vector<long long> results;

    for (long long n = low; n <= high; ++n) {
        bool has_small_factor = false;
        for (int i = 0; i < prime_count; ++i) {
            if (n % small_primes[i] == 0) {
                has_small_factor = true;
                break;
            }
        }
        if (!has_small_factor) {
            long long product = n * C;
            // 再次确认乘积在目标区间内（理论上必然成立，但保留检查）
            if (product >= LOWER && product <= UPPER) {
                results.push_back(n);
                std::cout << "找到 n = " << n << "，乘积 = " << product << std::endl;
            }
        }
    }

    std::cout << "总计找到 " << results.size() << " 个符合条件的数字。" << std::endl;
    return 0;
}
