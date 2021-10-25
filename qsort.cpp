#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <functional>
#include <utility>
#include <cassert>

void swap(int32_t &x1, int32_t &x2) {
    int32_t x = x1;
    x1 = x2;
    x2 = x;
}

int32_t &median(int32_t &x1, int32_t &x2, int32_t &x3) {
    if (x1 <= x2 && x2 <= x3 || x3 <= x2 && x2 <= x1) {
        return x2;
    } else if (x1 <= x3 && x3 <= x2 || x2 <= x3 && x3 <= x1) {
        return x3;
    } else {
        return x1;
    }
}

void qsort_sequential_interval(int32_t *x, uint32_t l, uint32_t r) {
    if (r - l <= 1) {
        return;
    }
    int32_t v = median(x[l], x[(r - l) / 2], x[r - 1]);
    uint32_t j = l;
    for (uint32_t i = l; i < r; ++i) {
        if (x[i] < v) {
            swap(x[j], x[i]);
            j++;
        }
    }
    uint32_t m1 = j;
    for (uint32_t i = m1; i < r; ++i) {
        if (x[i] == v) {
            swap(x[j], x[i]);
            j++;
        }
    }
    uint32_t m2 = j;
    qsort_sequential_interval(x, l, m1);
    qsort_sequential_interval(x, m2, r);
}

void qsort_parallel_interval(int32_t *x, uint32_t block_size, uint32_t l, uint32_t r) {
    if (r - l <= block_size) {
        qsort_sequential_interval(x, l, r);
        return;
    }
    int32_t v = median(x[l], x[(r - l) / 2], x[r - 1]);
    uint32_t j = l;
    for (uint32_t i = l; i < r; ++i) {
        if (x[i] < v) {
            swap(x[j], x[i]);
            j++;
        }
    }
    uint32_t m1 = j;
    cilk_spawn qsort_parallel_interval(x, block_size, l, m1);

    for (uint32_t i = m1; i < r; ++i) {
        if (x[i] == v) {
            swap(x[j], x[i]);
            j++;
        }
    }
    uint32_t m2 = j;
    qsort_parallel_interval(x, block_size, m2, r);
    cilk_sync;
}

void qsort_sequential(int32_t *x, uint32_t size) {
    qsort_sequential_interval(x, 0, size);
}

void qsort_parallel(int32_t *x, uint32_t size, uint32_t block_size) {
    qsort_parallel_interval(x, block_size, 0, size);
}

