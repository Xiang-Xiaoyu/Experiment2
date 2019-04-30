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
using namespace std;

const string AdmissionName[5] = { "AvailableAdmission:","DeleteAdmission:","DropAdmission:","InsertAdmission:","SelectAdmission:" };

struct apply
{
	string TableName;
	string AdmissionName;
	string admit;
	string admitted;
	string time;
};

struct Admission
{
	string AdmissionName;
	string admit;
	string admitted;
};

class Table
{
private:
	string name;
	string FileName;
	string OwnerName;
	int row, column;
	vector<string> ColumnName;
	vector<vector<string>> vec;
	vector<Admission*> admission;
public:
	int GetColumn()const { return column; }
	int GetRow()const { return row; }
	vector<string>& GetColumnName() { return ColumnName; }
	string GetOwnerName()const { return OwnerName; }
	string GetName()const { return name; }
	vector<Admission*>& GetAdmission() { return admission; }
	vector<vector<string>>& GetVec(){ return vec; }
	string GetFileName() { return FileName; }
	Table(string n, string fn,string on,vector<string> CN,vector<vector<string>> v,vector<Admission*> ad);
	Table(string n, string fn, string on, const Table & t);
	void Synchronize();
};

struct User
{
	string ID;
	string Password;
};

class mySQL
{
private:
	User* CurrentUser;
	//UserList.txt为默认User信息文件,TableList.txt为Table信息文
	vector<Table*> TableList;
	vector<User*> UserList;
	//ApplyAdmission.txt为申请文件信息
	vector<apply*> ApplyAdmission;
public:
	mySQL();
	void run();
	void LogIn();
	void AfterLogIn();
	void ApplyAdmissionSynchronize();
	void UserListSynchronize();
	void TableListSynchronize();
	void AdmissionSynchronize(string tablename);
	vector<apply*>& GetApplyAdmission() { return ApplyAdmission; }
	vector<Table*>& GetTableList() { return TableList; }
	vector<User*>& GetUserList() { return UserList; }
	bool IsTableName(string s);//是不是TableName+":"
	void DropTable(string tablename);
	User* GetCurrentUser()const { return CurrentUser; }
	void AskApplyAdmission();
};

extern mySQL sql;
extern bool IsAdmissionName(string s);//是不是AdmissionName+":"
extern bool cmp1(const Admission* a, const Admission* b);

extern unsigned int FindIndexInTableList(string name);
extern bool IsTxt(string s);
extern  bool NoNameRepetition(string name);
extern bool NoFileNameRepetition(string filename);
extern int BracketNum(string s);
extern bool LegalFirstBracket(string s);
extern bool LegalSecondBracket(string s);
extern void ExtractBracketFirstContent(string s, vector<string>& v);
extern void ExtractBracketSecondContent(string s, vector<string>& v);
extern bool IsCreate1(string s);
extern bool IsCreate2(string s);
extern bool IsDrop(string s);
extern bool HasAdmission(string username, string tablename, string admissionname);
extern bool IsInsert1(string s);
extern bool IsInsert2(string s);
extern bool IsDelete1(string s);
extern bool IsDelete2(string s);
extern bool IsSelect1(string s);
extern bool IsSelect2(string s);
extern bool LegalSelectColumn(string s,unsigned int init);
extern bool LegalSelectColumn2(string s, unsigned int init);
extern bool IsSelect3(string s);
extern bool IsSelect4(string s);
extern bool IsSelect5(string s);
extern bool IsSelect6(string s);
extern bool IsGrant(string s);
extern bool IsRevoke(string s);
extern bool IsAdmissionWay(string user1, string user2, string tablename, string admissionname);
extern void RevokeAdmission(string user1, string user2, string tablename, string admissionname);
extern void RecursionRevokeAdmission(string user1, string user2, string tablename, vector<Admission*>& ss);
extern bool RecursionFindAdmissionWay(vector<Admission*> &s, string user1, string user2);
