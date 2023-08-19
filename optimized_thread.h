#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include<functional>
#include <condition_variable>
#include <future>
#include <iostream>
#include <chrono>

using namespace std;

typedef function<void()> task_type;
typedef void (*FuncType) (vector<int>&, promise<void> p, int, int);

template<class T> class BlockedQueue
{
public:
	void push(T& item)
	{
		lock_guard<mutex> l(m_locker);
		m_task_queue.push(item);
		m_event_holder.notify_one();
	}

	void pop(T& item)
	{
		unique_lock<mutex> l(m_locker);
		if (m_task_queue.empty())
			m_event_holder.wait(l, [this] {return !m_task_queue.empty(); });

		item = m_task_queue.front();
		m_task_queue.pop();
	}

	bool fast_pop(T& item)
	{
		unique_lock<mutex> l(m_locker);
		if (m_task_queue.empty())
			return false;
		item = m_task_queue.front();
		m_task_queue.pop();
		return true;
	}
private:
	queue<T> m_task_queue;
	mutex m_locker;
	condition_variable m_event_holder;
};

class OptimizeThreadPool
{
public:
	OptimizeThreadPool();
	void start();
	void stop();
	void push_task(FuncType f, vector<int>& vector, promise<void> p,int arg1, int arg2);
	void threadFunc(int qindex);
private:
	int m_thread_count;
	vector<thread> m_threads;
	vector<BlockedQueue<task_type>> m_thread_queues;
	unsigned m_qindex;
};

class RequestHandler_2
{
public:
	RequestHandler_2();
	~RequestHandler_2();
	void push_task(FuncType f, vector<int>& vector, promise<void> p, int arg1, int arg2);
private:
	OptimizeThreadPool m_tpool;
};