/*************************************************************************
    > File Name: singleton.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Thu Apr 14 20:35:11 2016
 ************************************************************************/
#include <iostream>
#include <pthread.h>

template <typename T>
class Singleton
{
public:
	static T& instance()
	{
		pthread_once(&once_, &Singleton::init);
		return value_;
	}

private:
	Singleton();
	~Singleton();

	static void init()
	{
		value_ = new T();
	}

private:
	static pthread_once_t once_;
	static T* value_;
};

template <typename T>
pthread_once_t Singleton::once_ = PTHREAD_ONCE_INIT;

template <typename T>
T* Singleton<T>::value_ = nullptr;
