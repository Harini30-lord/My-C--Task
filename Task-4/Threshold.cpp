/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <vector>
#include <algorithm>
#include <future>
#include <thread>
#include <chrono>
#include <random>
#include <functional>
#include <iomanip> // For std::fixed, std::setprecision

const size_t ADAPTIVE_THRESHOLD = 2000;

template <typename Iterator, typename Compare>
void merge(Iterator begin, Iterator mid, Iterator end, Compare comp) {
    std::inplace_merge(begin, mid, end, comp);
}

template <typename Iterator, typename Compare>
void sequential_merge_sort(Iterator begin, Iterator end, Compare comp) {
    long long size = std::distance(begin, end);
    if (size <= 1) {
        return;
    }

    Iterator mid = begin;
    std::advance(mid, size / 2);

    sequential_merge_sort(begin, mid, comp);
    sequential_merge_sort(mid, end, comp);

    merge(begin, mid, end, comp);
}

template <typename Iterator, typename Compare>
void concurrent_merge_sort(Iterator begin, Iterator end, Compare comp) {
    long long size = std::distance(begin, end);

    if (size <= ADAPTIVE_THRESHOLD) {
        std::sort(begin, end, comp);
        return;
    }

    Iterator mid = begin;
    std::advance(mid, size / 2);

    std::future<void> future1 = std::async(std::launch::async, concurrent_merge_sort<Iterator, Compare>, begin, mid, comp);
    std::future<void> future2 = std::async(std::launch::async, concurrent_merge_sort<Iterator, Compare>, mid, end, comp);

    future1.get();
    future2.get();

    merge(begin, mid, end, comp);
}

template <typename SortFunction, typename Container>
void benchmark_sort(const std::string& name, SortFunction sort_func, Container& data_copy) {
    auto start = std::chrono::high_resolution_clock::now();
    sort_func(data_copy.begin(), data_copy.end(), std::less<typename Container::value_type>());
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << name << " sorted " << data_copy.size() << " elements in: "
              << std::fixed << std::setprecision(6) << duration.count() << " seconds" << std::endl;
}

int main() {
    std::cout << "--- Concurrent Merge Sort Benchmarking ---" << std::endl << std::endl;

    const size_t DATA_SIZE_SMALL = 100000;
    const size_t DATA_SIZE_MEDIUM = 1000000;
    const size_t DATA_SIZE_LARGE = 10000000;

    using DataType = int;
    using ContainerType = std::vector<DataType>;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<DataType> distrib(0, 1000000);

    std::cout << "Benchmarking with " << DATA_SIZE_SMALL << " elements:" << std::endl;
    ContainerType data_small(DATA_SIZE_SMALL);
    for (size_t i = 0; i < DATA_SIZE_SMALL; ++i) data_small[i] = distrib(gen);

    ContainerType data_small_seq = data_small;
    ContainerType data_small_conc = data_small;

    benchmark_sort("Sequential Merge Sort (Small)", sequential_merge_sort<ContainerType::iterator, std::less<DataType>>, data_small_seq);
    benchmark_sort("Concurrent Merge Sort (Small)", concurrent_merge_sort<ContainerType::iterator, std::less<DataType>>, data_small_conc);
    std::cout << std::endl;

    std::cout << "Benchmarking with " << DATA_SIZE_MEDIUM << " elements:" << std::endl;
    ContainerType data_medium(DATA_SIZE_MEDIUM);
    for (size_t i = 0; i < DATA_SIZE_MEDIUM; ++i) data_medium[i] = distrib(gen);

    ContainerType data_medium_seq = data_medium;
    ContainerType data_medium_conc = data_medium;

    benchmark_sort("Sequential Merge Sort (Medium)", sequential_merge_sort<ContainerType::iterator, std::less<DataType>>, data_medium_seq);
    benchmark_sort("Concurrent Merge Sort (Medium)", concurrent_merge_sort<ContainerType::iterator, std::less<DataType>>, data_medium_conc);
    std::cout << std::endl;

    std::cout << "Benchmarking with " << DATA_SIZE_LARGE << " elements:" << std::endl;
    ContainerType data_large(DATA_SIZE_LARGE);
    for (size_t i = 0; i < DATA_SIZE_LARGE; ++i) data_large[i] = distrib(gen);

    ContainerType data_large_seq = data_large;
    ContainerType data_large_conc = data_large;

    // CORRECTED LINES START HERE
    benchmark_sort("Sequential Merge Sort (Large)", sequential_merge_sort<ContainerType::iterator, std::less<DataType>>, data_large_seq);
    benchmark_sort("Concurrent Merge Sort (Large)", concurrent_merge_sort<ContainerType::iterator, std::less<DataType>>, data_large_conc);
    // CORRECTED LINES END HERE

    std::cout << std::endl;

    std::cout << "--- Benchmarking Complete ---" << std::endl;

    return 0;
}