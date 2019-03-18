#include "iostream"
#include "string"
#include "chrono"
#include "iomanip"
using namespace std;

class Log 
{
public:
	Log() {}

	void out(string content)
	{
		using namespace chrono;
		auto t = system_clock::to_time_t(system_clock::now());
		struct tm* nowTime = localtime(&t);
		cout << "[" << nowTime->tm_year + 1900 << setfill('0') << setw(2) << nowTime->tm_mon + 1
			<< setfill('0') << setw(2) <<  (int)nowTime->tm_mday << " "
			<< setfill('0') << setw(2) << (int)nowTime->tm_hour  << ":"
			<< setfill('0') << setw(2) << (int)nowTime->tm_min << ":"
			<< setfill('0') << setw(2) << (int)nowTime->tm_sec << "] "
			<< content << endl;
	}
};

