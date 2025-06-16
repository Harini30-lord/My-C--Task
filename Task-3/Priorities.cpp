/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <vector>
#include <queue>
#include <functional> // For std::function, std::bind
#include <future>     // For std::future, std::promise, std::shared_future
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>     // For std::chrono::milliseconds

struct PrioritizedTask {
    int priority;
    std::function<void()> func;
    std::shared_ptr<std::promise<void>> completion_promise;

    PrioritizedTask(int p, std::function<void()> f)
        : priority(p), func(std::move(f)), completion_promise(std::make_shared<std::promise<void>>()) {}
    
    // Default constructor needed by std::priority_queue if elements are retrieved by value
    PrioritizedTask() : priority(0) {} 
};

struct TaskComparator {
    bool operator()(const PrioritizedTask& a, const PrioritizedTask& b) {
        return a.priority > b.priority; // Lower number = higher priority (min-heap)
    }
};

class ThreadPool {
public:
    ThreadPool(size_t num_threads) : stop(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    PrioritizedTask task_wrapper;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });
                        if (this->stop && this->tasks.empty()) {
                            return;
                        }
                        task_wrapper = std::move(this->tasks.top());
                        this->tasks.pop();
                    }
                    try {
                        task_wrapper.func();
                        task_wrapper.completion_promise->set_value();
                    } catch (...) {
                        task_wrapper.completion_promise->set_exception(std::current_exception());
                    }
                }
            });
        }
    }

    template<class F, class... Args>
    std::shared_future<void> enqueue(int priority, F&& f, Args&&... args) {
        // Use std::bind to create the std::function<void()>
        // All arguments must be explicitly bound.
        auto task_func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        PrioritizedTask new_task(priority, task_func);
        std::shared_future<void> future = new_task.completion_promise->get_future().share();

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.push(std::move(new_task));
        }
        condition.notify_one();
        return future;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::priority_queue<PrioritizedTask, std::vector<PrioritizedTask>, TaskComparator> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// Test Functions (Tasks)
void print_message(const std::string& msg, int id, int delay_ms = 0) {
    if (delay_ms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    std::cout << "Task " << id << ": " << msg << " (Thread ID: " << std::this_thread::get_id() << ")" << std::endl;
}

void task_with_exception(int id) {
    std::cout << "Task " << id << ": Attempting to throw exception..." << std::endl;
    throw std::runtime_error("Error from Task " + std::to_string(id));
}

int main() {
    std::cout << "--- Advanced Thread Pool Demonstration ---" << std::endl << std::endl;

    ThreadPool pool(4);

    // Demo 1: Priority-based Scheduling
    std::cout << "=== Priority Scheduling Demo ===" << std::endl;
    // Explicitly bind the third argument (delay_ms) for C++11 std::bind compatibility
    pool.enqueue(2, std::bind(print_message, "Low Priority Task (P=2)", 101, 100)); 
    pool.enqueue(0, std::bind(print_message, "HIGH PRIORITY TASK (P=0)", 100, 0)); // No delay
    pool.enqueue(1, std::bind(print_message, "Medium Priority Task (P=1)", 102, 0)); // No delay
    pool.enqueue(0, std::bind(print_message, "ANOTHER HIGH PRIORITY TASK (P=0)", 103, 0)); // No delay
    pool.enqueue(2, std::bind(print_message, "Another Low Priority Task (P=2)", 104, 0)); // No delay

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "\n--------------------------------\n" << std::endl;

    // Demo 2: Task Dependencies
    std::cout << "=== Task Dependencies Demo ===" << std::endl;

    // Task A: Dependency source
    std::shared_future<void> future_A = pool.enqueue(0, std::bind(print_message, "Task A (Dependency Source)", 200, 300));

    // Task B: Depends on Task A. Capture future_A by value, pass id as a regular argument to lambda
    // In C++11, `[future_A, id=201]` is not allowed. Instead, we capture `future_A` and use a literal or
    // pass the ID as an argument to the lambda if it varies for each task.
    // For simplicity with this fixed ID for each lambda, we'll use literal string for ID in output.
    std::shared_future<void> future_B = pool.enqueue(0, [future_A]() {
        const int id_B = 201; // Declare inside lambda for C++11
        std::cout << "Task " << id_B << ": Waiting for Task A to complete..." << std::endl;
        future_A.wait();
        print_message("Task B (Dependent on A) - Task A Completed!", id_B);
    });

    pool.enqueue(5, std::bind(print_message, "Task C (Independent, Low Priority)", 202, 0));

    // Task D: Depends on Task B. Similar C++11 lambda capture adjustment.
    std::shared_future<void> future_D = pool.enqueue(1, [future_B]() {
        const int id_D = 203; // Declare inside lambda for C++11
        std::cout << "Task " << id_D << ": Waiting for Task B to complete..." << std::endl;
        future_B.wait();
        print_message("Task D (Dependent on B) - Task B Completed!", id_D);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::cout << "\n--------------------------------\n" << std::endl;

    // Demo 3: Error Propagation
    std::cout << "=== Error Propagation Demo ===" << std::endl;

    pool.enqueue(0, std::bind(task_with_exception, 300)); // Still works if task takes one arg
    std::shared_future<void> error_future_1 = pool.enqueue(0, std::bind(task_with_exception, 300));
    std::shared_future<void> error_future_2 = pool.enqueue(0, std::bind(print_message, "Task 301 (Will run)", 301, 0));

    try {
        error_future_1.get();
        std::cout << "Error Future 1: Succeeded (THIS SHOULD NOT HAPPEN)" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Caught expected exception from error_future_1: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Caught an unexpected exception from error_future_1." << std::endl;
    }

    try {
        error_future_2.get();
        std::cout << "Error Future 2: Succeeded as expected." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Caught unexpected exception from error_future_2: " << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "\n--------------------------------\n" << std::endl;

    std::cout << "--- All Demos Completed. Pool will shut down ---" << std::endl;

    return 0;
}