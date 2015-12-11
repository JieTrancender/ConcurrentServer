#ifndef HIERARCHICAL_MUTEX_HPP
#define HIERARCHICAL_MUTEX_HPP

#include <iostream>
#include <mutex>
#include <thread>

//简单的分层次互次元
class HierarchicalMutex
{
	std::mutex internal_mutex;	//内部互次元
	unsigned long const hierarchy_value;	//层次值
	unsigned long previous_hierarchy_value;
	static unsigned long this_thread_hierarchy_value;	//thread_loal的值来表示当前线程的层次值

	//检查是否违背层次规则
	void check_for_hierarchy_violation()
	{
		if (this_thread_hierarchy_value <= hierarchy_value)
		{
			throw std::logic_error("Mutex hierarchy violated");
		}
	}

	//更新层次值
	void update_hierarchy_value()
	{
		previous_hierarchy_value = this_thread_hierarchy_value;
		this_thread_hierarchy_value = hierarchy_value;
	}

public:
	explicit HierarchicalMutex(unsigned long value)
		:hierarchy_value(value), previous_hierarchy_value(0)
	{}

	//锁定互次元
	void lock()
	{
		check_for_hierarchy_violation();
		internal_mutex.lock();
		update_hierarchy_value();
	}

	//解锁互次元
	void unlock()
	{
		this_thread_hierarchy_value = previous_hierarchy_value;
		internal_mutex.unlock();
	}

	bool try_lock()
	{
		check_for_hierarchy_violation();
		if (!internal_mutex.try_lock())
		{
			return false;
		}

		update_hierarchy_value();
		return true;
	}
};

unsigned long HierarchicalMutex::this_thread_hierarchy_value(ULONG_MAX);	//当前线程层次值初始化为最大值

#endif	//hierarical_mutex.hpp
