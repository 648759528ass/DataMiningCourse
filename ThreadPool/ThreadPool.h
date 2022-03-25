//
// Created by jhas on 2022/3/25.
//

#ifndef DATAMININGCOURSE_THREADPOOL_H
#define DATAMININGCOURSE_THREADPOOL_H

#include <vector>
#include <atomic>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
const int THREADPOOL_MAX_NUM = 16;
class ThreadPool{
public:
    std::atomic<int> _idlThrNum;
    using Task = std::function<void()>;
    std::vector<std::thread> _pool;
    std::queue<Task> _tasks;
    std::mutex _lock;
    std::condition_variable _task_cv;

    std::atomic<bool> _run;
public:
    inline ThreadPool(unsigned short size = 4) { addThread(size); }
    inline ~ThreadPool()
    {
        _run=false;
        _task_cv.notify_all();
        for (std::thread& thread : _pool) {
            if(thread.joinable())
                thread.join();
        }
    }
    template <typename F,typename ... Args>
    auto commit(F&& f,Args&&... args){
        if (!_run)    // stoped ??
            throw ("commit on ThreadPool is stopped.");
        auto bd = std::bind(std::forward<F>(f),std::forward<Args>(args)...);
        {
            std::lock_guard<std::mutex> lock{_lock};
            _tasks.emplace([&bd]{
                bd();
            });
        }
        _task_cv.notify_one();
    }
    void addThread(unsigned short size)
    {
        for (; _pool.size() < THREADPOOL_MAX_NUM && size > 0; --size)
        {
            _pool.emplace_back( [this]{
                while (_run)
                {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock{ _lock };
                        _task_cv.wait(lock, [this]{
                            return !_run || !_tasks.empty();
                        });
                        if (!_run && _tasks.empty())
                            return;
                        task = move(_tasks.front());
                        _tasks.pop();
                    }
                    _idlThrNum--;
                    task();
                    _idlThrNum++;
                }
            });
            _idlThrNum++;
        }
    }
};
#endif //DATAMININGCOURSE_THREADPOOL_H
