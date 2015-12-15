#include <iostream>
#include <list>
#include <mutex>
#include <algorithm>
#include <thread>
#include <vector>

const int c_max_size = 100;

std::list<int> my_list;
std::mutex my_mutex;

void add_to_list(int new_value)
{
	std::lock_guard<std::mutex> guard(my_mutex);	//意味着对这些函数的访问时互斥的
	my_list.push_back(new_value);
}

bool list_contains(int value_to_find)
{
	std::lock_guard<std::mutex> guard(my_mutex);
	return std::find(my_list.begin(), my_list.end(), value_to_find) != my_list.end();
}

int main(int argc, char **argv)
{
	for (int i = 0; i < 100; ++i)
	{
		add_to_list(i);
	}

	int data;
	std::cin >> data;

	if (true == list_contains(data))
	{
		std::cout << "Right, it is " << data << std::endl;
	}
	return 0;
}
