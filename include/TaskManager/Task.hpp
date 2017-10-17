#include <memory>
#include <iostream>
#pragma once
struct Task {
public:
  int id;
  std::string task_name;

  Task(void) {}
  virtual ~Task(void) {}

  friend std::ostream& operator<<(std::ostream &os, Task const &t) { os << t.task_name << std::endl; return os; }
  virtual void Execute() = 0;
};

struct TestTask : public Task{
public:
  TestTask(void) {}
  void Execute(void) { std::cout << "task executing" << std::endl; }
};
