#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <ctime>

using namespace std;

double caculate(double v)
{
	if(v <= 0)
		return v;
	this_thread::sleep_for(chrono::milliseconds(10));
	return sqrt((v * v + sqrt((v - 5) * (v + 2.5)) / 2.0) / v);
}

template<typename Iter, typename Fun>
double visitRange(Iter iterBegin, Iter iterEnd, Fun func)
{
	double v = 0;
	for(auto iter = iterBegin; iter != iterEnd; ++iter)
	{
		v += func(*iter);
	}
	return v;
}

int main()
{
	vector<double> v;
	for(int i = 0; i < 1000; ++i)
	{
		v.push_back(rand());
	}
	auto nowc = clock();
	double value = 0.0;
	for(auto iter : v)
	{
		value += caculate(iter);
	}
	auto finishc = clock();
	cout << value << (finishc - nowc) << endl;;
	
	nowc = clock();
	auto iter = v.begin() + (v.size() / 2);
	auto iterEnd = v.end();
	double anotherv = 0.0;
	thread s([&anotherv, iter, iterEnd]()
		{
			anotherv = visitRange(iter, iterEnd, caculate);
		});
	
	auto halfv = visitRange(v.begin(), iter, caculate);
	s.join();
	finishc = clock();
	cout << (halfv + anotherv) << (finishc - nowc) << endl;
	
	return 0;
}
