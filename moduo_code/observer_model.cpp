/*************************************************************************
    > File Name: observer_model.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Thu Apr 14 19:07:20 2016
 ************************************************************************/
#include <iostream>
#include <memory>
#include <mutex>

class Observer
{
public:
	virtual ~Observer()
	{
		subject_->unregister(this);
	}
	virtual void update() = 0;
	void observe(Observable *s)
	{
		s->register(this);
		subject_ = s;
	}
private:
	Observable *subject_;
}

class Observable
{
public:
	void register(cosnt std::weak_ptr<Observer> &x);
	void notifyObservers();

private:
	mutable std::mutex mutex_;
	std::vector<std::weak_ptr<Observer> > observers_;  //用std::weak_ptr，因为该类型对象只销毁一次
	typedef std::vector<std::weak_ptr<Observer> >::iterator Iterator;
};

void Observable::notifyObservers()
{
	std::lock_guard<std::mutex> lock(mutex_);
	Iterator it = ovservers_.begin();
	while (it != observers_.end())
	{
		std::shared_ptr<Observer> obj(it->lock());  //尝试提升，线程安全的
		if (obj)
		{
			//提升成功，引用计数至少为2
			obj->update();  //没有竞态条件，因为obj在栈上，对象不可能在本作用域内销毁
			++it;
		}
		else
		{
			//对象已经销毁，从容器中拿掉weak_ptr
			it = observers_erase(it);
		}
	}
}
