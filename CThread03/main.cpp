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
#include <list>
#include <cstring>
#include <condition_variable>

using namespace std;

class Message
{
public:
	const string& data() const { return m_data; }
	Message() {}
	Message(string d) : m_data(move(d)) {}
private:
	string m_data;
};

mutex m;
condition_variable cv;
atomic<int> totalSize{0};
atomic<bool> ready{false};
atomic<bool> quit{false};
list<Message> globalList;


//情景二：锁过于密集，导致多线程性能不如单线程（伪多线程）
void worker(int i)
{
	//情景一：线程中循环等待，CPU占用率过高
	while(!ready)
	{
		/* 情景一：解决一 释放CPU，无太大作用，此线程释放CPU后，其他线程会立刻抢占
		this_thread::yield();*/
		
		/* 情景一：解决二 线程中等待 有效
		this_thread::sleep_for(chrono::seconds(1));*/
	}
	/*
	while(!quit)
	{
		this_thread::sleep_for(chrono::milliseconds(1));
		lock_guard<mutex> lock(m);
		if(globalList.empty())
			continue;
		auto iter = globalList.begin();
		totalSize += strlen((*iter).data().c_str());
		globalList.erase(iter);
	}*/
	
	Message msg;
	while(!quit)
	{
		{
			unique_lock<mutex> lock(m);
			//释放锁，满足条件加锁，继续执行
			cv.wait(lock, [] { return quit || !globalList.empty(); });
			if(quit)
				return;
			auto iter = globalList.begin();
			msg = move(*iter);
			globalList.erase(iter);
		}
		totalSize += strlen(msg.data().c_str());
	}
}

int main()
{
	const auto threadCount = 4;
	for(int i = 0; i < 500000; ++i)
		globalList.push_back("this is a test" + to_string(i));
	vector<thread> pool;
	for(int i = 0; i < threadCount; ++i)
	{
		pool.emplace_back(worker, i);
	}
	//this_thread::sleep_for(chrono::minutes(1));
	ready = true;
	for(int i = 0; i < 300000; ++i)
	{
		/*情景二：解决一 等待时间不要上锁 无效，效率更低 CPU占用率过高 类似于情景一
		lock_guard<mutex> lock(m);
		this_thread::sleep_for(chrono::milliseconds(1));*/
		
		//this_thread::sleep_for(chrono::milliseconds(1));
		lock_guard<mutex> lock(m);
		globalList.push_back(string("second"));
		cv.notify_one();
	}
	
	while(true)
	{
		lock_guard<mutex> lock(m);
		if(globalList.empty())
		{
			quit = true;
			cv.notify_all();
			break;
		}
	}
	for(auto& v : pool)
		if(v.joinable())
			v.join();
	cout << "total size is " << totalSize << endl;
	
	return 0;
}
