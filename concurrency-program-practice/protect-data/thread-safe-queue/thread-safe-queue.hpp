#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <iostream>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template <typename anytype>
class ThreadSafeQueue
{
private:
	mutable std::mutex m_mutex;
	std::queue<anytype> m_queue;
	std::condition_variable m_condition;

public:
	//默认构造函数
	ThreadSafeQueue() {}

	//拷贝构造函数
	ThreadSafeQueue(ThreadSafeQueue const &other)
	{
		std::lock_guard<std::mutex> lock(other.m_mutex);
		m_queue = other.m_queue;
	}

	//入队列
	void push(anytype value)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push(value);
		m_condition.notify_one();	//通知等待中的线程(如果有)
	}

	//等待 - 出队列
	void wait_and_pop(anytype &value)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock, [this] {return !m_queue.empty(); });
		value = m_queue.front();
		m_queue.pop();
	}

	//等待 - 出队列 - 重载(伪重载，因为对线程传参的时候参数推断不出来，并且std::bind不能绑定重载函数，所以改名解决这一问题)
	std::shared_ptr<anytype> wait_and_pop_1()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock, [this] {return !m_queue.empty(); });
		std::shared_ptr<anytype> res(std::make_shared<anytype>(m_queue.front()));
		m_queue.pop();
		return res;
	}

	//异常 - 出队列
	bool try_pop(anytype &value)
	{
		std::lock_guard<std::mutex> lock(mutex);
		if (m_queue.empty())
			return false;
		value = m_queue.front();
		m_queue.pop();
		return true;
	}

	//异常 - 出队列 - 重载(伪重载，因为对线程传参的时候参数推断不出来，并且std::bind不能绑定重载函数，所以改名解决这一问题)
	std::shared_ptr<anytype> try_pop_1()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_queue.empty())
			return std::shared_ptr<anytype>();
		std::shared_ptr<anytype> res(std::make_shared<anytype>(m_queue.front()));
		m_queue.pop();
		return res;
	}

	bool emptr() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}

};

#endif	//thread_safe_queue.hpp
