#pragma once
#include<vector>
#include<string>
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<sstream>
#include<conio.h>
#include<cstdio>
#include<cctype>
#include<algorithm>
#include<iomanip>
#include<time.h>
#define MAX_USER_NUM 20
//GAP表示多少条有效命令进行一次同步
using namespace std;

const string POWER_NAME[] = { "AVAILABLE","DELETE","DROP","INSERT","SELECT" };//权限信息
const int POWER_NAME_NUM = 5;//权限数
extern int valid_order;//有效命令的条数
//extern vector<int> MultikeywordSortIndex;//用于多关键词排序的关键词下标
//extern void loading(vector<int> v);
//extern bool cmp1(vector<string>& a, vector<string> &b);
//extern bool cmp2(vector<string>& a, vector<string> &b);

struct apply//申请信息
{
	string TableName;//表名
	string PowerName;//权限名
	string admit;//授权人(被申请者)
	string admitted;//被授权人(申请者)
	string time;//申请时间
	string information;//申请留言
};

class Table//表类
{
private:
	int row, column;//row为行数,即文件行数-1，column为关键词数量
	string name;//表名
	string FileName;//存储表的文件名
	string OwnerName;//表的创建用户ID
	vector<string> ColumnName;//表的关键词
	vector<vector<string> > vec;//二维vector存表中的信息
	int PowerMatrix[5][MAX_USER_NUM][MAX_USER_NUM];//权限信息，三维数组
public:

	friend class mySQL;
	friend class Legal;
public:
	Table(string n, string fn, string on);//创建一个名为n文件名为filename创建者名为on的表
	void LoadingPowerFromFile();
	void loading(vector<string> CN,vector<vector<string> > v, int tmp[POWER_NAME_NUM][MAX_USER_NUM][MAX_USER_NUM]);
	void loading();
	Table(string n, string fn, string on, Table & t);

	bool HasPower( string user, string powername);

	void PowerSynchronize();//权限同步
	void Synchronize();//table文件内容同步
	unsigned int GetUserListSize();
	void ASCSortOutput();
	void DESCSortOutput();
};


struct User
{
	string ID;
	string Password;
};

class mySQL
{
private:
	User* CurrentUser;//登陆的User
	//UserList.txt为默认User信息文件,TableList.txt为Table信息文
	vector<Table*> TableList;
	vector<User*> UserList;
	//ApplyAdmission.txt为申请文件信息
	vector<apply*> ApplyPower;
public:
	mySQL();
	/*运行主要有三个函数完成*/
	void run();
	void LogIn();
	void AfterLogIn();

	/*同步函数,申请权限、用户账号信息、表的名和对应文件信息、 */
	void ApplyPowerSynchronize();
	void UserListSynchronize();
	void TableListSynchronize();

	friend class Legal;
	void DropTable(string tablename);
	void AskApplyPower();

	//判断函数
	bool IsTableName(string n);//判断n是不是Table名
	bool IsTableFileName(string fn);//判断fn是不是table的文件名
	int FindTableIndex(string s);//取出table名为s的下标
	int FindTableIndexByFileName(string s);

	string tostring(string &s);

	friend unsigned int Table::GetUserListSize();
	vector<User*>& GetUserList() { return UserList; }
	friend int MapUserIdToInt(string s);
	void Revoke(string user1, string user2, string table, string powername);
};

class Legal
{
public:
	Legal(){}
	void LegalOrder(string order,vector<string> &v);
	bool IsCreate1(string s);
	bool IsTxt(string s);//是不是Txt文件名
	bool IsCreate2(string s);
	bool LegalFirstBracket(string s);
	int BraketNum(string s);
	bool LegalSecondBracket(string s);
	void ExtractBracketFirstContent(string s, vector<string>& v);
	void ExtractBracketSecondContent(string s, vector<string>& v);
	bool OneBelongOne(vector<string>&s1, vector<string>&s2);
	void Apply(string user, string tablename, string powername);//user申请权限
	bool HasPowerWayFromTo(string user1, string user2, string tablename, string powername);
	bool RecursionFindWay(int user_index1, int user_index2, int table_index, int power_index);
	bool IsInsert1(string s);
	bool IsInsert2(string s);
	void ExtractVecInt(vector<int>& vec_int, vector<string>& son, vector<string> &dad);
	bool IsDelete1(string s);
	bool IsDelete2(string s);
	bool IsSelect1(string s);
	bool IsSelect2(string s);
	bool IsSelect3(string s);
	bool IsSelect4(string s);
	bool IsSelect5(string s);
	bool IsSelect6(string s);
	void ExtractVecString(string order,string begin, string end,vector<string> &vec_string);
	bool LegalMidString(string order, string begin, string end);
	bool LegalLastString(string order, string begin);
	void ExtractVecString(string order, string begin, vector<string> &vec_string);
	bool IsGrant(string s);
	bool IsRevoke(string s);
	bool IsApply(string s);
};

extern Legal legal;
extern mySQL sql;

extern bool IsPowerName(string s);//s是不是权限名?
extern int MapPowerNameToInt(string s);//将权限名映射到Int
extern int MapUserIdToInt(string s);//将User映射到TableList的下标
extern string GetSystemTime();
