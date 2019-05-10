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
//GAP��ʾ��������Ч�������һ��ͬ��
using namespace std;

const string POWER_NAME[] = { "AVAILABLE","DELETE","DROP","INSERT","SELECT" };//Ȩ����Ϣ
const int POWER_NAME_NUM = 5;//Ȩ����
extern int valid_order;//��Ч���������
//extern vector<int> MultikeywordSortIndex;//���ڶ�ؼ�������Ĺؼ����±�
//extern void loading(vector<int> v);
//extern bool cmp1(vector<string>& a, vector<string> &b);
//extern bool cmp2(vector<string>& a, vector<string> &b);

struct apply//������Ϣ
{
	string TableName;//����
	string PowerName;//Ȩ����
	string admit;//��Ȩ��(��������)
	string admitted;//����Ȩ��(������)
	string time;//����ʱ��
	string information;//��������
};

class Table//����
{
private:
	int row, column;//rowΪ����,���ļ�����-1��columnΪ�ؼ�������
	string name;//����
	string FileName;//�洢����ļ���
	string OwnerName;//��Ĵ����û�ID
	vector<string> ColumnName;//��Ĺؼ���
	vector<vector<string> > vec;//��άvector����е���Ϣ
	int PowerMatrix[5][MAX_USER_NUM][MAX_USER_NUM];//Ȩ����Ϣ����ά����
public:

	friend class mySQL;
	friend class Legal;
public:
	Table(string n, string fn, string on);//����һ����Ϊn�ļ���Ϊfilename��������Ϊon�ı�
	void LoadingPowerFromFile();
	void loading(vector<string> CN,vector<vector<string> > v, int tmp[POWER_NAME_NUM][MAX_USER_NUM][MAX_USER_NUM]);
	void loading();
	Table(string n, string fn, string on, Table & t);

	bool HasPower( string user, string powername);

	void PowerSynchronize();//Ȩ��ͬ��
	void Synchronize();//table�ļ�����ͬ��
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
	User* CurrentUser;//��½��User
	//UserList.txtΪĬ��User��Ϣ�ļ�,TableList.txtΪTable��Ϣ��
	vector<Table*> TableList;
	vector<User*> UserList;
	//ApplyAdmission.txtΪ�����ļ���Ϣ
	vector<apply*> ApplyPower;
public:
	mySQL();
	/*������Ҫ�������������*/
	void run();
	void LogIn();
	void AfterLogIn();

	/*ͬ������,����Ȩ�ޡ��û��˺���Ϣ��������Ͷ�Ӧ�ļ���Ϣ�� */
	void ApplyPowerSynchronize();
	void UserListSynchronize();
	void TableListSynchronize();

	friend class Legal;
	void DropTable(string tablename);
	void AskApplyPower();

	//�жϺ���
	bool IsTableName(string n);//�ж�n�ǲ���Table��
	bool IsTableFileName(string fn);//�ж�fn�ǲ���table���ļ���
	int FindTableIndex(string s);//ȡ��table��Ϊs���±�
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
	bool IsTxt(string s);//�ǲ���Txt�ļ���
	bool IsCreate2(string s);
	bool LegalFirstBracket(string s);
	int BraketNum(string s);
	bool LegalSecondBracket(string s);
	void ExtractBracketFirstContent(string s, vector<string>& v);
	void ExtractBracketSecondContent(string s, vector<string>& v);
	bool OneBelongOne(vector<string>&s1, vector<string>&s2);
	void Apply(string user, string tablename, string powername);//user����Ȩ��
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

extern bool IsPowerName(string s);//s�ǲ���Ȩ����?
extern int MapPowerNameToInt(string s);//��Ȩ����ӳ�䵽Int
extern int MapUserIdToInt(string s);//��Userӳ�䵽TableList���±�
extern string GetSystemTime();
