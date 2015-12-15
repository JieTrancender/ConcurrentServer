#include "thread_safe_queue.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <functional>

int main(int argc, char **argv)
{
	std::string str;
	ThreadSafeQueue<std::string>  my_queue;
	std::vector<std::thread> my_vector(10);

	for (auto iter = my_vector.begin(); iter != my_vector.end(); ++iter)
	{
		*iter = std::thread(&ThreadSafeQueue<std::string>::wait_and_pop, &my_queue, str);
	}

	for (auto iter = 0; iter != 10; ++iter)
		my_queue.push("张三");
	
	for (auto iter = my_vector.begin(); iter != my_vector.end(); ++iter)
		iter->join();

	return 0;
}
