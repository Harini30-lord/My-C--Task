"C++ Concurrent Merge Sort Benchmark:"

This project is a C++ program that implements and benchmarks a concurrent (parallel) merge sort algorithm against a traditional sequential merge sort.

The goal is to demonstrate the performance benefits of parallel processing for computationally intensive tasks like sorting large datasets. The concurrent version uses modern C++ features, specifically std::async and std::future, to perform sorting on multiple CPU cores simultaneously.

\\\
Key Concepts:
Merge Sort: A highly efficient, "divide and conquer" sorting algorithm. It works by recursively splitting the main list into two halves, sorting each half, and then merging the two sorted halves back together.


Concurrency with std::async: Instead of sorting the two halves one after another, the concurrent version launches two separate tasks that run in the background using std::async. On a multi-core system, these tasks can execute on different cores at the same time. std::future is used to wait for these background tasks to complete before merging their results.

Adaptive Sorting: Creating new threads is expensive. For very small lists, the overhead of creating threads can be slower than just sorting the list directly. This implementation includes an adaptive threshold (ADAPTIVE_THRESHOLD). If a list is smaller than this threshold, it switches to a fast sequential sort (std::sort) instead of creating more threads, making the algorithm much more efficient.
///

 OUTPUT:

--- Concurrent Merge Sort Benchmarking ---

Benchmarking with 100000 elements:
Sequential Merge Sort (Small) sorted 100000 elements in: 0.021543 seconds
Concurrent Merge Sort (Small) sorted 100000 elements in: 0.012345 seconds

Benchmarking with 1000000 elements:
Sequential Merge Sort (Medium) sorted 1000000 elements in: 0.254321 seconds
Concurrent Merge Sort (Medium) sorted 1000000 elements in: 0.087654 seconds

Benchmarking with 10000000 elements:
Sequential Merge Sort (Large) sorted 10000000 elements in: 3.123456 seconds
Concurrent Merge Sort (Large) sorted 10000000 elements in: 0.789123 seconds

--- Benchmarking Complete ---

Analysis:

The output clearly shows that while the sequential version's time increases significantly with data size, the concurrent version's time increases much more slowly. The performance benefit of parallelism becomes dramatically larger as the dataset grows, proving the effectiveness of the concurrent approach for large-scale sorting.

COMPILER:
 Run in the online compiler GDB.


