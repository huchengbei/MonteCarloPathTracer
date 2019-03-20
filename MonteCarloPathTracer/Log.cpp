#include "iostream"
#include "string"
#include "chrono"
#include "fstream"
using namespace std;

class Log 
{
public:
	string path;
	bool is2file = false;
	ofstream outFile;
	
	Log() {}

	void setPath(string p)
	{
		is2file = true;
		path = p;
		outFile.open(path);
		if (!outFile.is_open())
		{
			outFile.close();
			is2file = false;
			out("open log file error!");
		}
		else
		{
			outFile.close();
			out("open log file success!");
		}
	}

	void out(string content)
	{
		using namespace chrono;
		auto t = system_clock::to_time_t(system_clock::now());
		struct tm* nowTime = localtime(&t);
		char timeFormat[25];
		sprintf_s(timeFormat, "[%d%02d%02d %02d:%02d:%02d] ", 
			nowTime->tm_year + 1900, (int)nowTime->tm_mon + 1, 
			(int)nowTime->tm_mday, (int)nowTime->tm_hour, 
			(int)nowTime->tm_min, (int)nowTime->tm_sec);
		string outContent = string(timeFormat) + content + "\n";
		cout << outContent;
		if (is2file)
		{
			outFile.open(path, fstream::out | ofstream::app);
			outFile << outContent;
			outFile.close();
		}
	}
};

