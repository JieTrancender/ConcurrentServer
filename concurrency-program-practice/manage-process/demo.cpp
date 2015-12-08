#include <iostream>
#include <thread>

class thread_guard
{
	std::thread &t;

public:
	
	//构造函数
	explicit thread_guard(std::thread &t_) :t(t_){}	//explicit避免隐式转换

	//析构函数
	~thread_guard()
	{
		if (t.joinable()) //在析构函数中判断是否可联接，如果可联接则等待线程结束，避免了对临时变量的使用先于线程结束
		{
			t.join();
		}
	}

	//删除复制构造函数
	thread_guard(thread_guard const&) = delete;

	//删除赋值运算符
	thread_guard& operator=(thread_guard const&) = delete;
};

class scoped_thread
{
	std::thread t;
public:
	//构造函数
	explicit scoped_thread(std::thread t_) :t(std::move(t_))  //直接将线程传递给类，对原有线程进行了分离
	{
		if (!t.joinable())
			throw std::logic_error("No thread");
	}

	//析构函数
	~scoped_thread()
	{
		t.join();
	}

	//拷贝构造函数
	scoped_thread(scoped_thread const&) = delete;

	//赋值运算符
	scoped_thread& operator=(scoped_thread const&) = delete;

};

struct func
{
	int &i;
	func(int &i_) :i(i_){}

	void operator()()
	{
		for (unsigned i = 0; i < 100; ++i)
		{
			std::cout << "i = " << i << std::endl;
		}
	}
};

void f()
{
	int data;
	scoped_thread t(std::thread(func(data)));
	for (int i = 0; i < 50; ++i)
	{
		std::cout << "i = " << i << std::endl;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	f();
	std::cout << std::thread::hardware_concurrency << std::endl;
	return 0;
}
