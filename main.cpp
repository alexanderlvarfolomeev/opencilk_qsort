#include <chrono>
#include <random>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <limits>
#include "qsort.cpp"

int main() {
    std::default_random_engine generator(time(nullptr));
    std::uniform_int_distribution<int32_t> elements_distribution(-100000, 100000);
    uint32_t sz = 100000000;
    uint32_t reps = 5;

    std::vector<int32_t *> x(reps);
    for (int i = 0; i < reps; ++i) {
        x[i] = static_cast<int32_t *>(::operator new(sizeof(int32_t) * sz));
        for (uint32_t j = 0; j < sz; ++j) {
            x[i][j] = elements_distribution(generator);
        }
    }

    double seq = 0;
    uint64_t par = std::numeric_limits<uint64_t>::max();
    uint32_t block = sz;
    for (uint32_t i = 1; i <= 1024; i *= 4) {
        uint64_t sum = 0;
        uint32_t block_size = sz / i;

        for (uint32_t j = 0; j < reps; ++j) {
            auto *x_copy = static_cast<int32_t *>(::operator new(sizeof(int32_t) * sz));
            for (uint32_t k = 0; k < sz; ++k) {
                x_copy[k] = x[j][k];
            }
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            if (i == 1) {
                qsort_sequential(x_copy, sz);
            } else {
                qsort_parallel(x_copy, sz, block_size);
            }
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            sum += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            ::operator delete(x_copy);
        }
        if (i == 1) {
            seq = sum;
            par = sum;
        } else {
            if (par > sum) {
                par = sum;
                block = block_size;
            }
        }
        std::cout << block_size << " block size, elapsed " << sum / reps << " milliseconds" << std::endl;
    }
    std::cout << "Best coefficient " << seq / par << " with block size " << block << std::endl;
    return 0;
}
