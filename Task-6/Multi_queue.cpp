/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <random>
#include <iomanip>
#include <memory> // Required for std::unique_ptr

void TaskWork(int task_id, int complexity_ms) {
    if (complexity_ms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(complexity_ms));
    }
}

using Task = std::function<void()>;

class WorkStealingQueue {
private:
    std::deque<Task> m_deque;
    mutable std::mutex m_mutex;
    std::condition_variable m_cond_var;

public:
    void push(Task task) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_deque.push_back(std::move(task));
        m_cond_var.notify_one();
    }

    bool try_pop_local(Task& task) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_deque.empty()) {
            return false;
        }
        task = std::move(m_deque.front());
        m_deque.pop_front();
        return true;
    }

    bool pop_blocking(Task& task, const std::atomic<bool>& stop_flag) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond_var.wait(lock, [&]{ return stop_flag.load(std::memory_order_acquire) || !m_deque.empty(); });
        if (stop_flag.load(std::memory_order_acquire) && m_deque.empty()) {
            return false;
        }
        task = std::move(m_deque.front());
        m_deque.pop_front();
        return true;
    }

    bool try_steal_many(std::vector<Task>& stolen_tasks_buffer) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_deque.empty()) {
            return false;
        }
        size_t tasks_to_steal = m_deque.size() / 2;
        if (tasks_to_steal == 0 && m_deque.size() > 0) {
            tasks_to_steal = 1;
        }
        if (tasks_to_steal == 0) return false;

        for (size_t i = 0; i < tasks_to_steal; ++i) {
            stolen_tasks_buffer.push_back(std::move(m_deque.back()));
            m_deque.pop_back();
        }
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_deque.empty();
    }
};

// Forward declaration of run_benchmark for the friend declaration
void run_benchmark(const std::string& name, size_t num_threads, size_t total_tasks, bool enable_stealing,
                   size_t heavy_task_count_per_queue, int heavy_task_complexity_ms, int light_task_complexity_ms);


class ThreadPool {
private:
    std::vector<std::unique_ptr<WorkStealingQueue>> m_queues;
    std::vector<std::thread> m_threads;
    std::atomic<bool> m_stop;
    size_t m_num_threads;
    bool m_enable_stealing;

    std::atomic<long long> m_completed_tasks;

    void consumer_loop(size_t my_id) {
        WorkStealingQueue* my_queue = m_queues[my_id].get();
        std::vector<Task> stolen_tasks_buffer;
        Task task_to_execute;

        while (!m_stop.load(std::memory_order_acquire)) {
            if (my_queue->try_pop_local(task_to_execute)) {
                task_to_execute();
                m_completed_tasks.fetch_add(1, std::memory_order_relaxed);
                continue;
            }

            if (m_enable_stealing) {
                bool stolen_any_this_round = false;
                for (size_t i = 0; i < m_num_threads; ++i) {
                    if (m_stop.load(std::memory_order_acquire)) break;
                    size_t victim_id = (my_id + i + 1) % m_num_threads;
                    
                    if (m_queues[victim_id]->try_steal_many(stolen_tasks_buffer)) {
                        stolen_any_this_round = true;
                        break;
                    }
                }

                if (stolen_any_this_round) {
                    for (Task& stolen_task : stolen_tasks_buffer) {
                        if (m_stop.load(std::memory_order_acquire)) break;
                        stolen_task();
                        m_completed_tasks.fetch_add(1, std::memory_order_relaxed);
                    }
                    stolen_tasks_buffer.clear();
                    continue;
                }
            }

            if (!my_queue->pop_blocking(task_to_execute, m_stop)) {
                break;
            }
            task_to_execute();
            m_completed_tasks.fetch_add(1, std::memory_order_relaxed);
        }
    }

public:
    ThreadPool(size_t num_threads, bool enable_stealing = true)
        : m_num_threads(num_threads), m_enable_stealing(enable_stealing), m_stop(false), m_completed_tasks(0) {
        
        for (size_t i = 0; i < m_num_threads; ++i) {
            m_queues.push_back(std::unique_ptr<WorkStealingQueue>(new WorkStealingQueue()));
        }

        for (size_t i = 0; i < m_num_threads; ++i) {
            m_threads.emplace_back(&ThreadPool::consumer_loop, this, i);
        }
    }

    ~ThreadPool() {
        m_stop.store(true, std::memory_order_release);
        for (size_t i = 0; i < m_num_threads; ++i) {
            m_queues[i]->push([]{}); // Wake up blocking threads
        }

        for (std::thread& thread : m_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    // Public method for producers to enqueue a task to a specific queue
    // This addresses the private access error in main()
    void enqueue_task_to_specific_queue(size_t queue_index, Task task) {
        if (queue_index >= m_num_threads) {
            std::cerr << "Error: Queue index out of bounds!" << std::endl;
            return;
        }
        m_queues[queue_index]->push(std::move(task));
    }

    // Public method for producers to enqueue a task using round-robin distribution
    void enqueue_task_round_robin(Task task) {
        static std::atomic<size_t> next_queue_idx(0);
        size_t target_queue = next_queue_idx.fetch_add(1, std::memory_order_relaxed) % m_num_threads;
        m_queues[target_queue]->push(std::move(task));
    }

    long long get_completed_tasks_count() const {
        return m_completed_tasks.load(std::memory_order_acquire);
    }

    bool all_queues_empty() const {
        for(const auto& q_ptr : m_queues) {
            if (!q_ptr->empty()) return false;
        }
        return true;
    }
    
    // FRIEND DECLARATION (no changes needed here, as run_benchmark is already declared)
    friend void run_benchmark(const std::string& name, size_t num_threads, size_t total_tasks, bool enable_stealing,
                               size_t heavy_task_count_per_queue, int heavy_task_complexity_ms, int light_task_complexity_ms);
};

void run_benchmark(const std::string& name, size_t num_threads, size_t total_tasks, bool enable_stealing,
                   size_t heavy_task_count_per_queue, int heavy_task_complexity_ms, int light_task_complexity_ms) {
    
    std::cout << "--- " << name << " (" << (enable_stealing ? "Work Stealing ON" : "Work Stealing OFF") << ") ---" << std::endl;
    std::cout << "Threads: " << num_threads << ", Total Tasks: " << total_tasks << std::endl;
    std::cout << "Heavy Tasks per queue: " << heavy_task_count_per_queue << " (Complexity: " << heavy_task_complexity_ms << "ms)" << std::endl;
    std::cout << "Light Tasks: " << total_tasks - (heavy_task_count_per_queue * num_threads) << " (Complexity: " << light_task_complexity_ms << "ms)" << std::endl;

    ThreadPool pool(num_threads, enable_stealing);

    size_t heavy_tasks_produced = 0;
    
    for(size_t i = 0; i < heavy_task_count_per_queue * num_threads; ++i) {
        int task_id = 1000000 + i;
        // Accessing m_queues directly via friend access (this is fine inside run_benchmark)
        pool.m_queues[i % num_threads]->push(std::bind(TaskWork, task_id, heavy_task_complexity_ms));
        heavy_tasks_produced++;
    }

    for (size_t i = 0; i < total_tasks - heavy_tasks_produced; ++i) {
        int task_id = i;
        pool.enqueue_task_round_robin(std::bind(TaskWork, task_id, light_task_complexity_ms));
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    while (pool.get_completed_tasks_count() < total_tasks) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;

    std::cout << "Total tasks completed: " << pool.get_completed_tasks_count() << std::endl;
    std::cout << "Execution time: " << std::fixed << std::setprecision(4) << duration.count() << " seconds" << std::endl;
    std::cout << "Throughput: " << static_cast<int>(total_tasks / duration.count()) << " tasks/sec" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl << std::endl;
}

int main() {
    std::cout << "=== Concurrent Multi-Queue Producer-Consumer with Work-Stealing ===" << std::endl << std::endl;

    const size_t NUM_THREADS = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4;
    const size_t TOTAL_TASKS = 2000;
    const size_t HEAVY_TASKS_PER_QUEUE = 50;
    const int HEAVY_TASK_COMPLEXITY = 20;
    const int LIGHT_TASK_COMPLEXITY = 1;

    run_benchmark("Benchmark without Work Stealing", NUM_THREADS, TOTAL_TASKS,
                  false, HEAVY_TASKS_PER_QUEUE, HEAVY_TASK_COMPLEXITY, LIGHT_TASK_COMPLEXITY);

    run_benchmark("Benchmark with Work Stealing", NUM_THREADS, TOTAL_TASKS,
                  true, HEAVY_TASKS_PER_QUEUE, HEAVY_TASK_COMPLEXITY, LIGHT_TASK_COMPLEXITY);

    std::cout << "\n=== Aggressive Imbalance Test (Work Stealing Benefit) ===" << std::endl;
    const size_t AGGRESSIVE_TOTAL_TASKS = 1000;
    const size_t AGGRESSIVE_HEAVY_TASKS = 200;
    const int AGGRESSIVE_HEAVY_COMPLEXITY = 50;
    const int AGGRESSIVE_LIGHT_COMPLEXITY = 0;

    std::cout << "--- Initial Imbalance (No Stealing) ---" << std::endl;
    ThreadPool pool_imbalance_no_steal(NUM_THREADS, false);
    // FIX: Corrected variable name from 'start_' to 'start_imbalance_no_steal'
    pool_imbalance_no_steal.enqueue_task_to_specific_queue(0, std::bind(TaskWork, 9000, AGGRESSIVE_HEAVY_COMPLEXITY));
    for(size_t i = 0; i < AGGRESSIVE_HEAVY_TASKS - 1; ++i) {
        pool_imbalance_no_steal.enqueue_task_to_specific_queue(0, std::bind(TaskWork, 9001 + i, AGGRESSIVE_HEAVY_COMPLEXITY));
    }
    for(size_t i = 0; i < AGGRESSIVE_TOTAL_TASKS - AGGRESSIVE_HEAVY_TASKS; ++i) {
        pool_imbalance_no_steal.enqueue_task_round_robin(std::bind(TaskWork, i, AGGRESSIVE_LIGHT_COMPLEXITY));
    }
    auto start_imbalance_no_steal = std::chrono::high_resolution_clock::now();
    while (pool_imbalance_no_steal.get_completed_tasks_count() < AGGRESSIVE_TOTAL_TASKS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto end_imbalance_no_steal = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_imbalance_no_steal = end_imbalance_no_steal - start_imbalance_no_steal;
    std::cout << "No Stealing Time: " << std::fixed << std::setprecision(4) << duration_imbalance_no_steal.count() << " seconds (Throughput: " << static_cast<int>(AGGRESSIVE_TOTAL_TASKS / duration_imbalance_no_steal.count()) << " tasks/sec)" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl << std::endl;


    std::cout << "--- Initial Imbalance (With Stealing) ---" << std::endl;
    ThreadPool pool_imbalance_with_steal(NUM_THREADS, true);
    // FIX: Corrected variable name from 'start_' to 'start_imbalance_with_steal'
    pool_imbalance_with_steal.enqueue_task_to_specific_queue(0, std::bind(TaskWork, 9000, AGGRESSIVE_HEAVY_COMPLEXITY));
    for(size_t i = 0; i < AGGRESSIVE_HEAVY_TASKS - 1; ++i) {
        pool_imbalance_with_steal.enqueue_task_to_specific_queue(0, std::bind(TaskWork, 9001 + i, AGGRESSIVE_HEAVY_COMPLEXITY));
    }
    for(size_t i = 0; i < AGGRESSIVE_TOTAL_TASKS - AGGRESSIVE_HEAVY_TASKS; ++i) {
        pool_imbalance_with_steal.enqueue_task_round_robin(std::bind(TaskWork, i, AGGRESSIVE_LIGHT_COMPLEXITY));
    }
    auto start_imbalance_with_steal = std::chrono::high_resolution_clock::now();
    while (pool_imbalance_with_steal.get_completed_tasks_count() < AGGRESSIVE_TOTAL_TASKS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto end_imbalance_with_steal = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_imbalance_with_steal = end_imbalance_with_steal - start_imbalance_with_steal;
    std::cout << "With Stealing Time: " << std::fixed << std::setprecision(4) << duration_imbalance_with_steal.count() << " seconds (Throughput: " << static_cast<int>(AGGRESSIVE_TOTAL_TASKS / duration_imbalance_with_steal.count()) << " tasks/sec)" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl << std::endl;


    std::cout << "=== All Demos Complete ===" << std::endl;

    return 0;
}

    