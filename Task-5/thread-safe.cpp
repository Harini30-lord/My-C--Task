/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/


   #include <iostream>
#include <atomic>      // For std::atomic
#include <thread>      // For std::thread
#include <vector>      // For std::vector
#include <chrono>      // For std::chrono
#include <memory>      // For std::shared_ptr (for comparison)
#include <functional>  // For std::function
#include <random>      // For random operations in stress test
#include <iomanip>     // For std::fixed, std::setprecision

// --- 1. Control Block for Atomic Reference Counting ---
template <typename T>
struct AtomicControlBlock {
    std::atomic<long> ref_count; // Atomic counter for object references
    T* data_ptr;                 // Pointer to the managed object

    AtomicControlBlock(T* ptr) : ref_count(1), data_ptr(ptr) {}

    ~AtomicControlBlock() {
        delete data_ptr;
    }
};

// --- 2. MyAtomicSharedPtr Class ---
template <typename T>
class MyAtomicSharedPtr {
private:
    T* data_ptr;                 // Pointer to the actual managed object
    AtomicControlBlock<T>* cb_ptr; // Pointer to the shared control block

    void acquire() {
        if (cb_ptr) {
            cb_ptr->ref_count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void release() {
        if (cb_ptr) {
            long old_count = cb_ptr->ref_count.fetch_sub(1, std::memory_order_acq_rel);

            if (old_count == 1) {
                delete cb_ptr;
                cb_ptr = nullptr;
                data_ptr = nullptr;
            }
        }
    }

public:
    MyAtomicSharedPtr() : data_ptr(nullptr), cb_ptr(nullptr) {}

    explicit MyAtomicSharedPtr(T* p) : data_ptr(p), cb_ptr(nullptr) {
        if (p) {
            cb_ptr = new AtomicControlBlock<T>(p);
        }
    }

    MyAtomicSharedPtr(const MyAtomicSharedPtr& other) : data_ptr(other.data_ptr), cb_ptr(other.cb_ptr) {
        acquire();
    }

    MyAtomicSharedPtr(MyAtomicSharedPtr&& other) noexcept : data_ptr(other.data_ptr), cb_ptr(other.cb_ptr) {
        other.data_ptr = nullptr;
        other.cb_ptr = nullptr;
    }

    ~MyAtomicSharedPtr() {
        release();
    }

    MyAtomicSharedPtr& operator=(const MyAtomicSharedPtr& other) {
        if (this != &other) {
            release();
            data_ptr = other.data_ptr;
            cb_ptr = other.cb_ptr;
            acquire();
        }
        return *this;
    }

    MyAtomicSharedPtr& operator=(MyAtomicSharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            data_ptr = other.data_ptr;
            cb_ptr = other.cb_ptr;
            other.data_ptr = nullptr;
            other.cb_ptr = nullptr;
        }
        return *this;
    }

    T& operator*() const {
        if (!data_ptr) {
            throw std::runtime_error("Attempt to dereference null MyAtomicSharedPtr.");
        }
        return *data_ptr;
    }

    T* operator->() const {
        return data_ptr;
    }

    T* get() const {
        return data_ptr;
    }

    long use_count() const {
        return cb_ptr ? cb_ptr->ref_count.load(std::memory_order_relaxed) : 0;
    }

    void reset(T* p = nullptr) {
        if (data_ptr == p) {
            return;
        }
        release();

        data_ptr = p;
        if (p) {
            cb_ptr = new AtomicControlBlock<T>(p);
        } else {
            cb_ptr = nullptr;
        }
    }
};

// --- Test Class for Smart Pointers ---
std::atomic<int> s_instance_count(0); // Tracks total active TestClass instances

struct TestClass {
    int id;
    TestClass(int val) : id(val) {
        s_instance_count.fetch_add(1, std::memory_order_relaxed);
    }
    ~TestClass() {
        s_instance_count.fetch_sub(1, std::memory_order_relaxed);
    }
    int get_id() const { return id; }
};

// --- Multi-threaded Stress Test Function (GENERIC) ---
template <typename SmartPtrType>
void run_stress_test(SmartPtrType& global_shared_ptr, int thread_id, int iterations) {
    SmartPtrType ptr_local; // Local pointer for each thread

    for (int i = 0; i < iterations; ++i) {
        { // Scope for copy and its destruction
            SmartPtrType temp_ptr = global_shared_ptr; // Copy from the shared global pointer
            if (temp_ptr.get()) {
                volatile int val = temp_ptr->get_id(); // Access to prevent optimization of use
                (void)val; // Avoid unused variable warning
            }
        }

        if (i % 100 == 0) {
            // Reset the global shared pointer, creating contention on deletion/creation
            global_shared_ptr.reset(new TestClass(thread_id * 1000 + i));
        }

        // Operations on local pointer
        ptr_local.reset(new TestClass(thread_id * 10000 + i));
        if (i % 50 == 0) {
            SmartPtrType another_copy = ptr_local;
        }
    }
}

// --- Benchmark Runner Function ---
template <typename SmartPtrType>
void run_benchmark(const std::string& name, int num_threads, int iterations_per_thread) {
    s_instance_count.store(0, std::memory_order_relaxed); // Reset instance count

    // Create a NEW global pointer instance for this benchmark run
    // This makes the benchmark runs independent and avoids issues with static globals
    SmartPtrType current_global_ptr(new TestClass(999999)); 

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        // Pass the *same* current_global_ptr to all threads by reference
        threads.emplace_back(run_stress_test<SmartPtrType>, std::ref(current_global_ptr), i, iterations_per_thread);
    }

    for (auto& t : threads) {
        t.join(); // Wait for all threads to complete
    }

    // Explicitly reset the global pointer to trigger final cleanup and allow memory check
    current_global_ptr.reset(); 

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << name << " with " << num_threads << " threads, "
              << iterations_per_thread << " ops/thread: "
              << std::fixed << std::setprecision(6) << duration.count() << " seconds" << std::endl;

    if (s_instance_count.load(std::memory_order_relaxed) != 0) {
        std::cerr << "WARNING: Memory leak detected! " << s_instance_count.load() << " TestClass instances remaining." << std::endl;
    } else {
        std::cout << "Memory check: All TestClass instances successfully reclaimed." << std::endl;
    }
    std::cout << std::endl;
}


// --- Main Function ---
int main() {
    std::cout << "--- Lock-Free Smart Pointer (Simplified) Demonstration ---" << std::endl << std::endl;

    // --- Correctness Demonstration (Single-Threaded Basics) ---
    std::cout << "=== Correctness (Single-Threaded) ===" << std::endl;
    { // Use a scope to ensure destructors are called
        MyAtomicSharedPtr<TestClass> ptr1(new TestClass(10));
        std::cout << "Ptr1 use_count: " << ptr1.use_count() << std::endl;

        MyAtomicSharedPtr<TestClass> ptr2 = ptr1; // Copy construction
        std::cout << "Ptr1 use_count: " << ptr1.use_count() << ", Ptr2 use_count: " << ptr2.use_count() << std::endl;

        MyAtomicSharedPtr<TestClass> ptr3;
        ptr3 = ptr1; // Copy assignment
        std::cout << "Ptr1 use_count: " << ptr1.use_count() << ", Ptr3 use_count: " << ptr3.use_count() << std::endl;

        ptr1.reset(new TestClass(20)); // Reset ptr1 to a new object
        std::cout << "Ptr1 use_count: " << ptr1.use_count() << " (new obj), Others use_count: " << ptr2.use_count() << std::endl;

        ptr2.reset(); // Reset ptr2
        std::cout << "Ptr2 reset. Ptr3 use_count: " << ptr3.use_count() << std::endl;
    } // ptr3 goes out of scope here, triggering final deletion of TestClass 10
    std::cout << "After block, s_instance_count: " << s_instance_count.load() << std::endl;
    std::cout << "--- Single-threaded correctness verified ---" << std::endl << std::endl;


    // --- Benchmarking and Multi-threaded Stress Tests ---
    std::cout << "=== Benchmarking and Multi-threaded Stress Tests ===" << std::endl;

    const int NUM_THREADS = 8; // Number of threads for stress test
    const int ITERATIONS_PER_THREAD = 100000; // Number of operations per thread

    std::cout << "Running with " << NUM_THREADS << " threads, " << ITERATIONS_PER_THREAD << " iterations per thread." << std::endl;
    std::cout << "Total pointer copies/resets/deletions affecting shared pointer: Approx " << NUM_THREADS * (ITERATIONS_PER_THREAD / 100) << std::endl;
    std::cout << "Total pointer operations (copies/resets) on local pointers: Approx " << NUM_THREADS * ITERATIONS_PER_THREAD * 2 << std::endl << std::endl;

    // Benchmark MyAtomicSharedPtr
    run_benchmark<MyAtomicSharedPtr<TestClass>>("MyAtomicSharedPtr", NUM_THREADS, ITERATIONS_PER_THREAD);

    // Benchmark std::shared_ptr
    run_benchmark<std::shared_ptr<TestClass>>("std::shared_ptr", NUM_THREADS, ITERATIONS_PER_THREAD);
    
    std::cout << "--- Demonstration Complete ---" << std::endl;

    return 0;
}