"C++ Thread Pool with Work-Stealing"

This project is a C++ implementation of a high-performance, multi-threaded task processing system. It demonstrates the Producer-Consumer pattern using a Thread Pool where each thread has its own task queue.

The key feature of this project is the Work-Stealing mechanism, which is designed to improve performance by efficiently balancing the workload across all available threads.
///
Key Concepts :

Thread Pool: A collection of worker threads that are created once and then wait to execute tasks. This avoids the expensive overhead of creating and destroying threads for each new task.

Work Queue: In this model, each thread has its own dedicated queue where it receives tasks. A thread always pulls tasks from its own queue first.

The Problem:
 Load Imbalance: Imagine one thread is assigned 100 heavy tasks, while all other threads are assigned only 1 light task each. The other threads will finish quickly and become idle, while the one busy thread does all the work. This is inefficient.

The Solution: Work-Stealing: When a thread's own queue becomes empty, instead of sitting idle, it will look at the queues of other threads and "steal" work from them. This allows idle threads to help out busy threads, leading to better load balancing and faster overall completion time.
\\\


Main Benchmarks:
\\\
The program runs two main types of tests to demonstrate the effectiveness of work-stealing:

<<Balanced Load Test: In this test, heavy tasks are distributed evenly among all thread queues. The program runs this scenario twice—once with work-stealing disabled and once with it enabled—to show the baseline performance.>>

<<Aggressive Imbalance Test: This is the key demonstration. All the long, heavy tasks are deliberately pushed to a single thread's queue, while all other threads receive very little work. This test clearly shows how idle threads can "steal" work from the overloaded thread, resulting in a dramatic performance improvement>>
\\\
The output :
-- The most important comparison is in the "AggressiveImbalance Test".


=== Aggressive Imbalance Test (Work Stealing Benefit) ===
--- Initial Imbalance (No Stealing) ---
No Stealing Time: 10.1032 seconds (Throughput: 98 tasks/sec)
----------------------------------------------------

--- Initial Imbalance (With Stealing) ---
With Stealing Time: 1.3521 seconds (Throughput: 739 tasks/sec)
----------------------------------------------------

As you can see, when the work was imbalanced, the version with work-stealing was dramatically faster (e.g., ~7.5x faster in this sample run) and had a much higher throughput.



\\\
    
Compiler:
Online C++ GDB 


