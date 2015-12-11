#include "hierarchical_mutex.hpp"
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

HierarchicalMutex high_level_mutex(10000);
HierarchicalMutex low_level_mutex(5000);

int do_low_level_stuff();

int low_level_func()
{
	std::lock_guard<HierarchicalMutex> lock(low_level_mutex);
	return do_low_level_stuff();
}

void high_level_stuff(int some_param);

void high_level_func()
{
	std::lock_guard<HierarchicalMutex> lock(high_level_mutex);
	high_level_stuff(low_level_func());
}

//调用high_level_func()，锁定了high_level_mutex(层次值10000),并在这函数中调用low_level_func()函数作为high_level_stuff函数的参数
//这是可行的，因为后者的层次值低于前者，满足规则
void thread_a()
{
	high_level_func();
}

HierarchicalMutex other_mutex(100);
void do_other_stuff();

void other_stuff()
{
	high_level_func();
	do_other_stuff();
}

//调用other_stuff()，锁定了other_mutex(层次值100)，在这函数中调用other_stuff()然后此函数中调用high_level_func()
//锁定high_level_mutex(层次值为10000)，这是不可行的，因为后者的层次值高于前者，不满足规则
void thread_b()
{
	std::lock_guard<HierarchicalMutex> lock(other_mutex);
	other_stuff();
}

int main(int argc, char **argv)
{
	//std::thread thread_first(thread_a);
	//std::thread thread_second(thread_b);//thread_b执行失败，

	//thread_first.join();
	//thread_second.join();

	std::vector<std::thread> my_vector(100);
	for (auto it = my_vector.begin(); it != my_vector.end(); ++it)
		*it = std::thread(low_level_func);
	//for (auto &iter : my_vector)
	//	iter.join();
	for (auto it = my_vector.begin(); it != my_vector.end(); ++it)
	{
		//if (it->joinable())
			it->join();
	}
	return 0;
}

int do_low_level_stuff()
{
	std::cout << "The is low level stuff, named " << __func__ << ",and line is " << __LINE__ << std::endl;
	return __LINE__;
}

void high_level_stuff(int some_param)
{
	std::cout << "This is high stuff, param is " << some_param << ",line is " << __LINE__ << ",and named is " << __func__ << std::endl;
}

void do_other_stuff()
{
	std::cout << "This is other stuff, line is " << __LINE__ << ", and named " << __func__ << std::endl;
}
