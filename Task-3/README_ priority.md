"Advanced C++ Thread Pool":

  The project is a C++ implementation of an advanced, multi-threaded task processing system. 

The design is based on a Producer-Consumer pattern, where a central ThreadPool object manages a set of worker threads that consume tasks from a shared, prioritized queue.


Key Features:

1. Priority-Based Task Scheduling
Instead of a simple first-in, first-out (FIFO) queue, this thread pool uses a std::priority_queue.

2. Task Dependencies and Synchronization (std::future)
The enqueue method returns a std::shared_future<void>.

3.xception Propagation
If a task running on a worker thread throws an exception, the program won't crash.

How it works: The thread pool's worker loop has a try...catch block. It catches any exception thrown by a task and forwards it to the std::promise associated with that task.
Handling Errors: Later, on the main thread, when you call .get() on the task's future, the original exception will be re-thrown. This allows you to handle errors from background threads cleanly and safely in a centralized location.

\\\
How he Output is Generated:

The output you see is a series of std::cout statements from the main function and the test functions it calls (print_message, task_with_exception). The std::this_thread::sleep_for() calls are used to simulate work and to make the order of execution clear. The output from the different demos will be interleaved, as all 4 threads are running in the background and picking up tasks as they become available according to their priority.

///
 COMPILER:
 Run the code in online C++ GDB.










