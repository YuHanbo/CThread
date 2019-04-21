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

using namespace std;

class Message
{
public:
	const string& data() const { return m_data; }
	Message(string d) : m_data(move(d)) {}
private:
	string m_data;
};

int totalSize = 0;
list<Message> globalList;

void worker()
{
	while(!globalList.empty())
	{
		auto iter = globalList.begin();
		totalSize += strlen((*iter).data().c_str());
		globalList.erase(iter);
	}
}

int main()
{
	for(int i = 0; i < 500000; ++i)
		globalList.push_back("this is a test" + to_string(i));
	worker();
	for(int i = 0; i < 300000; ++i)
	{
		//this_thread::sleep_for(chrono::milliseconds(1));
		globalList.push_back(string("second"));
	}
	worker();
	cout << "total size is " << totalSize << endl;
	
	return 0;
}
