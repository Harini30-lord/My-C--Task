"Thread-Safe Atomic Shared Pointer in C++:"
\\\
This project is a C++ implementation of a custom smart pointer, MyAtomicSharedPtr. It is designed to manage dynamic memory automatically and safely across multiple threads.

The core feature is its use of std::atomic for reference counting, which allows it to be thread-safe without needing explicit locks (like a mutex) for basic copy and destruction operations. The program demonstrates the correctness of this smart pointer in a single-threaded context and then stress-tests its performance against the standard library's std::shared_ptr in a multi-threaded benchmark.
///

Key Concepts :

\\\
Smart Pointer: An object that acts like a pointer but automatically manages memory. When the last smart pointer to an object is destroyed, it automatically deletes the object, preventing memory leaks.
///

\\\
Shared Ownership (shared_ptr): A type of smart pointer where multiple pointers can "own" the same object. It keeps track of how many owners there are using a reference count.

///

\\\
The Challenge (Thread Safety): When multiple threads copy and destroy shared pointers at the same time, the reference count itself can be corrupted by a race condition.
///

\\\
The Solution (std::atomic): MyAtomicSharedPtr solves this by using std::atomic<long> for its reference counter. Atomic operations are guaranteed to be indivisible, even when accessed from multiple threads simultaneously, thus ensuring the reference count is always correct.
///



OUTPUT:

--- Lock-Free Smart Pointer (Simplified) Demonstration ---

=== Correctness (Single-Threaded) ===
Ptr1 use_count: 1
Ptr1 use_count: 2, Ptr2 use_count: 2
Ptr1 use_count: 3, Ptr3 use_count: 3
Ptr1 use_count: 1 (new obj), Others use_count: 2
Ptr2 reset. Ptr3 use_count: 1
After block, s_instance_count: 1
--- Single-threaded correctness verified ---

=== Benchmarking and Multi-threaded Stress Tests ===
Running with 8 threads, 100000 iterations per thread.
Total pointer copies/resets/deletions affecting shared pointer: Approx 8000
Total pointer operations (copies/resets) on local pointers: Approx 1600000

MyAtomicSharedPtr with 8 threads, 100000 ops/thread: 0.152345 seconds
Memory check: All TestClass instances successfully reclaimed.

std::shared_ptr with 8 threads, 100000 ops/thread: 0.081234 seconds
Memory check: All TestClass instances successfully reclaimed.

COMPILER:
 run the online C++ compiler GDB.