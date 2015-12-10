#include <iostream>
#include <exception>
#include <memory>
#include <mutex>
#include <stack>
#include <thread>
#include <vector>

struct empty_stack : std::exception
{
	const char* what() const //noexcept	//用于抛出异常
	{
		return "empty stack.";
	}
};

template <typename anytype>
class ThreadSafeStack
{
private:
	std::stack<anytype> m_data;
	mutable std::mutex m_mutex;	//mutable表示可修改的，即使是在被const修饰的函数里面

public:

	mutable int count;

	//默认构造函数
	ThreadSafeStack(){}

	//复制构造函数
	ThreadSafeStack(const ThreadSafeStack &stack)
	{
		//在构造函数体重进行复制而不是初始化列表，确保互次元被整个副本持有
		std::lock_guard<std::mutex> lock(stack.m_mutex);
		m_data = stack.m_data;
		count = 0;
	}

	//赋值运算符
	ThreadSafeStack& operator=(const ThreadSafeStack&) = delete;

	//入栈
	void push(anytype new_value)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_data.push(new_value);
		++count;
	}

	//出栈
	std::shared_ptr<anytype> pop()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_data.empty())
		{
			throw empty_stack();
		}

		std::shared_ptr<anytype> const res(std::make_shared<anytype>(m_data.top()));	//修改栈之前分配返回值
		m_data.pop();
		--count;
		return res;
	}

	//删除栈中元素
	void pop(anytype &value)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_data.empty())
		{
			throw emptr_stack();
		}

		value = m_data.top();
		m_data.pop();
		--count;
	}

	//判断是否为空
	bool empty() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_data.empty();
	}
};

int main(int argc, char **argv)
{
	ThreadSafeStack<int> str_safe_stack;
	std::vector<std::thread> my_vector(100);

	for (auto it = my_vector.begin(); it != my_vector.end(); ++it)
		*it = std::thread(&ThreadSafeStack<int>::push, &str_safe_stack, str_safe_stack.count);

	for (auto it = my_vector.begin(); it != my_vector.end(); ++it)
		it->join();

	while (!str_safe_stack.empty())
		std::cout << *str_safe_stack.pop() << std::endl;
	

	return 0;
}
