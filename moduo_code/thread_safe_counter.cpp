/*************************************************************************
    > File Name: thread_safe_counter.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Thu Apr 14 17:12:44 2016
 ************************************************************************/
#include <iostream>
#include <thread>
#include <mutex>

class Counter
{
	//复制构造函数和赋值函数被设置为私有的
public:
	Counter():value_(0){}
	int64_t value() const;
	int64_t getAndIncrease();

private:
	Counter operator=(const Counter &other){}
	Counter(const Counter &other){}

	int64_t value_;
	mutable std::mutex  mutex_;
};

int64_t Counter::value() const
{
	std::lock_guard<std::mutex> lock(mutex_);
	return value_;
}

int64_t Counter::getAndIncrease()
{
	std::lock_guard<std::mutex> lock(mutex_);
	int64_t ret = value_++;
	return ret;
}

int main(int argc, char** argv)
{
	Counter count;
	std::cout << count.getAndIncrease() << std::endl;
	std::cout << count.value() << std::endl;
	
	return 0;
}
