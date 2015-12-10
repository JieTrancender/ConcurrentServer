#include <iostream>
#include <thread>

class Wrapper
{
public:
	void member1()
	{
		std::cout << "i am member1" << std::endl;
	}

	void member2(const char *arg1, unsigned arg2)
	{
		std::cout << "i am member2 and first arg is (" << arg1 << "and second arg is " << arg2 << std::endl;
	}

	std::thread member1Thread()
	{
		return std::thread([=]{member1(); }); //lambda
	}

	std::thread member2Thread(const char *arg1, unsigned arg2)
	{
		return std::thread([=]{member2(arg1, arg2); });
	}
};

int main(int argc, char **argv)
{
	Wrapper *w = new Wrapper();
	std::thread w1 = w->member1Thread();
	std::thread w2 = w->member2Thread("hello", 100);
	std::thread w3(&Wrapper::member2,w,"world",0);

	w1.join();
	w2.join();
	w3.join();

	return 0;
}
