#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <ctime>
#include <atomic>
#include <mutex>

using namespace std;

/*解决方案5
template<typename T>
class Lock
{
public:
	Lock(T& mutex) : m_mutex(mutex) { m_mutex.lock(); }
	~Lock() { m_mutex.unlock(); }
private:
	T& m_mutex;
};*/

class Counter
{
public:
	Counter() : m_count(0) {}
	//void addCount() { m_count++; }
	/*解决方案4
	void addCountWithMutex()
	{
		m_mutex.lock();
		addCount();
		m_mutex.unlock();
	}
	int count() const
	{
		m_mutex.lock(); 
		auto r = m_count; 
		m_mutex.unlock(); 
		return r; 
	}*/
	
	/* 解决方案3 
	void lockMutex() { m_mutex.lock(); }
	void unlockMutex() { m_mutex.unlock(); }
	*/
	
	/*解决方案5
	void addCountWithMutex()
	{
		Lock<mutex> lock(m_mutex);
		addCount();
	}
	int count() const
	{
		Lock<mutex> lock(m_mutex);
		return m_count;
	}*/
	
	//解决方案6
	void addCountWithMutex()
	{
		lock_guard<mutex> lock(m_mutex);
		addCount();
	}
	int count() const
	{
		lock_guard<mutex> lock(m_mutex);
		return m_count;
	}
private:
	/* 解决方案2，原子操作
	atomic_int m_count;
	atomic<int> m_count;
	*/
	
	int m_count;
	mutable mutex m_mutex;
	void addCount() { m_count++; }
};

int work(int a)
{
	return a + a;
}

template<class Iter>
void realWork(Counter& c, double& totalValue, Iter b, Iter e)
{
	for(; b != e; ++b)
	{
		totalValue += work(*b);
		/* 解决方案3，添加临界区锁
		c.lockMutex();
		c.addCount();
		c.unlockMutex();
		*/
		
		c.addCountWithMutex();
	}
}

int main()
{
	vector<int> vec;
	double totalValue = 0.0;
	for(int i = 0; i < 10000000; ++i)
	{
		vec.push_back(rand() % 100);
	}
	
	Counter counter;
	
	for(auto v : vec)
	{
		totalValue += work(v);
		counter.addCountWithMutex();
	}
	
	cout << counter.count() << endl;

	auto iter = vec.begin() + (vec.size() / 3);
	auto iter2 = vec.begin() + (vec.size() / 3 * 2);
	
	/* 共享资源会出现错误
	totalValue = 0.0;
	Counter counter2;
	thread b([&counter2, &totalValue, iter, iter2]()
		{
			realWork(counter2, totalValue, iter, iter2);
		});
		
	auto end = vec.end();
	thread c([&counter2, &totalValue, iter2, end]()
		{
			realWork(counter2, totalValue, iter2, end);
		});
	
	realWork(counter2, totalValue, vec.begin(), iter);
	*/
	
	/* 解决方法1，不共享资源
	totalValue = 0.0;
	Counter counter2;
	thread b([&counter2, &totalValue, iter, iter2]()
		{
			realWork(counter2, totalValue, iter, iter2);
		});
	double totalC = 0.0;
	Counter c3;
	auto end = vec.end();
	thread c([&c3, &totalC, iter2, end]()
		{
			realWork(c3, totalC, iter2, end);
		});
	
	double totalM = 0.0;
	Counter c4;
	realWork(c4, totalM, vec.begin(), iter);	
	*/
	
	/* 解决方案2，原子操作，适合简单应用 
	   单个变量同步
	   tip：在类定义中有改变 
	totalValue = 0.0;
	Counter counter2;
	thread b([&counter2, &totalValue, iter, iter2]()
		{
			realWork(counter2, totalValue, iter, iter2);
		});
	double totalC = 0.0;
	auto end = vec.end();
	thread c([&counter2, &totalC, iter2, end]()
		{
			realWork(counter2, totalC, iter2, end);
		});
	double totalM = 0.0;
	realWork(counter2, totalM, vec.begin(), iter);
	*/
	
	/* 解决方案3，临界体 
	totalValue = 0.0;
	Counter counter2;
	thread b([&counter2, &totalValue, iter, iter2]()
		{
			realWork(counter2, totalValue, iter, iter2);
		});
	double totalC = 0.0;
	auto end = vec.end();
	thread c([&counter2, &totalC, iter2, end]()
		{
			realWork(counter2, totalC, iter2, end);
		});
	double totalM = 0.0;
	realWork(counter2, totalM, vec.begin(), iter);
	*/
	
	/* 解决方案4，临界区升级版 
	totalValue = 0.0;
	Counter counter2;
	thread b([&counter2, &totalValue, iter, iter2]()
		{
			realWork(counter2, totalValue, iter, iter2);
		});
	double totalC = 0.0;
	auto end = vec.end();
	thread c([&counter2, &totalC, iter2, end]()
		{
			realWork(counter2, totalC, iter2, end);
		});
	double totalM = 0.0;
	realWork(counter2, totalM, vec.begin(), iter);
	*/
	
	/* 解决方案5， 临界区再升级 
	totalValue = 0.0;
	Counter counter2;
	thread b([&counter2, &totalValue, iter, iter2]()
		{
			realWork(counter2, totalValue, iter, iter2);
		});
	double totalC = 0.0;
	auto end = vec.end();
	thread c([&counter2, &totalC, iter2, end]()
		{
			realWork(counter2, totalC, iter2, end);
		});
	double totalM = 0.0;
	realWork(counter2, totalM, vec.begin(), iter);
	*/
	
	/* 解决方案6， STL临界区锁 */
	totalValue = 0.0;
	Counter counter2;
	thread b([&counter2, &totalValue, iter, iter2]()
		{
			realWork(counter2, totalValue, iter, iter2);
		});
	double totalC = 0.0;
	auto end = vec.end();
	thread c([&counter2, &totalC, iter2, end]()
		{
			realWork(counter2, totalC, iter2, end);
		});
	double totalM = 0.0;
	realWork(counter2, totalM, vec.begin(), iter);
	
	b.join();
	c.join();
	cout << counter2.count() << endl;
	return 0;
}
