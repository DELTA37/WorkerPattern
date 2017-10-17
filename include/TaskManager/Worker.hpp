#include <iostream>
#include <mutex>
#include <memory>
#include <utility>
#include <atomic>
#include <deque>
#include <vector>
#include <pthread.h>
#include "Task.hpp"

class TaskManager {
  std::deque<std::unique_ptr<Task>> tasks;
  int max_workers;
  int num_workers;
  std::atomic<bool> running;
  std::mutex tasks_mutex;
  std::vector<pthread_t> workers;
  std::atomic<int> in_use;

private:
  void WorkerBody(void);
  static void* WorkerBodyProxy(void*); 

public:
  TaskManager(void) : max_workers(4) {}
  TaskManager(int _max_workers) : max_workers(_max_workers) {}
  ~TaskManager(void) = default;

  TaskManager& operator=(TaskManager const &Q) = delete; 
  TaskManager& operator=(TaskManager const &&Q) = delete;

  void addTask(std::unique_ptr<Task> task);
  bool ready(void);
  void start(void);
  void stop(void);
  void join(void);
};
