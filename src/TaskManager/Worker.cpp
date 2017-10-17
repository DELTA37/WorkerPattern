#include "TaskManager/Worker.hpp"

void TaskManager::addTask(std::unique_ptr<Task> t) {
  this->tasks_mutex.lock();
  this->tasks.push_back(std::move(t));
  this->tasks_mutex.unlock();
  in_use.fetch_add(1);
}

void TaskManager::start(void) {
  this->running.store(true);
  for (int i = 0; i < this->max_workers; i++) {
    pthread_t worker_thread;
    if (pthread_create(&worker_thread, NULL, TaskManager::WorkerBodyProxy, this) != 0) {
      std::cout << "cannot create pthread" << std::endl;
    }
    this->workers.push_back(worker_thread);
  }
  in_use.store(0);
}

void TaskManager::stop(void) {
  this->running.store(false);
  for (int i = 0; i < this->max_workers; i++) {
    pthread_join(this->workers[i], NULL);
  }
}

void TaskManager::join(void) {
  while (this->in_use.load() > 0) {}
}

void* TaskManager::WorkerBodyProxy(void* args) {
  TaskManager* tm = reinterpret_cast<TaskManager*>(args);
  tm->WorkerBody();
  return NULL;
}

void TaskManager::WorkerBody(void) {
  while(this->running.load()) {
    this->tasks_mutex.lock();
    if (this->tasks.size() > 0) {
      std::unique_ptr<Task> task = std::move(this->tasks.front());
      this->tasks.pop_front();
      this->tasks_mutex.unlock();
      task->Execute();
      in_use.fetch_sub(1);
    } else {
      this->tasks_mutex.unlock();
    }
  }
}

bool TaskManager::ready(void) {
  return (in_use.load() == 0);
}
