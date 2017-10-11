#include <bits/stdc++.h>

using namespace std;

struct ThreadPool {
  typedef std::function<void(void)> Task;
  ThreadPool(int threadNumber);
  ~ThreadPool();

  bool add(Task task);
  bool start();
  bool stop();

  void threadWork();    
  mutex mutex_;
  condition_variable_any condition_empty_; 
  list<Task> tasks_;           
  bool running_;                   
  int threadNumber_;           
  vector<shared_ptr<thread>> threads_; 
};
ThreadPool::ThreadPool(int threadNumber){
  threadNumber_ = threadNumber;
  running_ = true;
  threads_.resize(threadNumber_);
}

ThreadPool::~ThreadPool() {
  if(running_)
    stop();
}

bool ThreadPool::start() {
  for(int i = 0; i < threadNumber_; i++) {
    threads_.push_back(make_shared<thread>(bind(&ThreadPool::threadWork,this)));
  }
  //printf("threadpool start!\n");
  return true;   
}
bool ThreadPool::stop(void) {
  if(running_) {
    running_= false;
    for(auto t : threads_) {
      t->join(); 
    }
  }
  return true;
}
bool ThreadPool::add(Task task) {
  lock_guard<mutex> guard(mutex_);
  tasks_.push_front(task);   
  condition_empty_.notify_one();
  return true;
}

void ThreadPool::threadWork(void) {
  Task task = NULL;
  while(running_) {   
    {
      lock_guard<mutex> guard(mutex_);
      if(tasks_.empty()) {
        condition_empty_.wait(mutex_);
      }
      if(!tasks_.empty()) {
        task = tasks_.front();
        tasks_.pop_front(); 
      }
      else
        continue;
    }
    task();
  }
}

