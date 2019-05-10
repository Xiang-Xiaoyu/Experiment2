#include"SQL.h"

Legal legal;
mySQL sql;
int valid_order = 0;

int main()
{
	sql.run();
	system("pause");
	return 0;
}

string GetSystemTime()
{
	time_t timer;
	time(&timer);
	tm* t_tm = localtime(&timer);
	string year = to_string(t_tm->tm_year + 1900);
	string month = to_string(t_tm->tm_mon + 1);
	string day = to_string(t_tm->tm_mday);
	string hour = to_string(t_tm->tm_hour);
	string minute = to_string(t_tm->tm_min);
	string second = to_string(t_tm->tm_sec);
	string return_time = year + "|" + month + "|" + day + "|" + hour + ":" + minute + ":" + second;
	return return_time;
}
