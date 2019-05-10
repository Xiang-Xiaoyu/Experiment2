#include"SQL.h"

Table::Table(string n, string fn, string on)//在程序刚刚开始的时候载入一些信息
{
	name = n;
	FileName = fn;
	OwnerName = on;
	string order = "tablepower/" + name + "Power.txt";
	ofstream out(order.c_str());
	out.close();
	LoadingPowerFromFile();
}

void Table::LoadingPowerFromFile()//载入一个表的权限信息
{
	string order = "tablepower/" + name+"Power.txt";
	ifstream in(order.c_str());
	if (!in)
		cout << name << "power.txt Can't be opened!" << endl;
	else
	{
		for (unsigned int i = 0; i < POWER_NAME_NUM; i++)
			for (unsigned int j = 0; j < MAX_USER_NUM; j++)
				for (unsigned int k = 0; k < MAX_USER_NUM; k++)
					PowerMatrix[i][j][k] = 0;
		string s;
		istringstream ss;
		string pn="";
		while (!in.eof())
		{
			getline(in, s);
			if (s == "")
				continue;
			if (IsPowerName(s))
			{
				pn = s;
				continue;
			}
			ss.str(s);
			string s1, s2, s3;
			ss >> s1 >> s2 >> s3;
			ss.clear();
			PowerMatrix[MapPowerNameToInt(pn)][MapUserIdToInt(s1)][MapUserIdToInt(s3)]++;
		}
	}
}

void Table::loading(vector<string> CN, vector<vector<string>> v, int tmp[POWER_NAME_NUM][MAX_USER_NUM][MAX_USER_NUM])
//给一个特定的列名，表内容，权限三维数组加载进表
{
	ColumnName.swap(CN);
	vector<string>().swap(CN);
	vector<vector<string>>().swap(vec);
	unsigned int num = v.size();
	for (unsigned int i = 0; i < num; i++)
		vec.push_back(v[i]);
	for (unsigned int i = 0; i < POWER_NAME_NUM; i++)
		for (unsigned int j = 0; j < MAX_USER_NUM; j++)
			for (unsigned int k = 0; k < MAX_USER_NUM; k++)
				PowerMatrix[i][j][k] = tmp[i][j][k];
	row = vec.size();
	column = ColumnName.size();
}

//void loading(vector<int> v)
//{
//	vector<int>().swap(MultikeywordSortIndex);
//	unsigned int num = v.size();
//	for (unsigned int i = 0; i < num; i++)
//		MultikeywordSortIndex.push_back(v[i]);
//}

void Table::loading()
//使用一个表之前，加载它的相关信息
{
	LoadingPowerFromFile();
	string order1 = "table/" + sql.tostring(FileName);
	ifstream in1(order1.c_str());
	if (!in1)
		cout << sql.tostring(FileName) << " can't be opened!" << endl;
	else
	{
		string s;
		getline(in1, s);
		istringstream ss(s);
		string str;
		vector<string>().swap(ColumnName);
		while (ss >> str)
			ColumnName.push_back(str);
		ss.clear();
		vector<string> row_vec;
		vector<vector<string>>().swap(vec);
		vec.clear();
		while (!in1.eof())
		{
			getline(in1, s);
			if (s == "")
				continue;
			ss.str(s);
			while (ss >> str)
				row_vec.push_back(str);
			ss.clear();
			vec.push_back(row_vec);
			vector<string>().swap(row_vec);
			row_vec.clear();
		}
		in1.close();
	}
	row = vec.size();
	column = ColumnName.size();
}

Table::Table(string n, string fn, string on,Table & t)
//用一个表来初始化一个新表
{
	t.loading();
	name = n;
	FileName = fn;
	OwnerName = on;
	column = t.column;
	row = t.row;
	ColumnName.assign(t.ColumnName.begin(), t.ColumnName.end());
	vec.assign(t.vec.begin(), t.vec.end());
	for (unsigned int i = 0; i < POWER_NAME_NUM; i++)
		for (unsigned int j = 0; j < MAX_USER_NUM; j++)
			for (unsigned int k = 0; k < MAX_USER_NUM; k++)
				PowerMatrix[i][j][k] = t.PowerMatrix[i][j][k];
}

bool Table::HasPower(string user, string powername)
//判断ID为user的用户是否拥有powername的权限
{
	if (user == OwnerName)
		return true;
	int index = MapUserIdToInt(user);
	int index0 = MapPowerNameToInt(powername);
	for (unsigned int i = 0; i < MAX_USER_NUM; i++)
		if (PowerMatrix[index0][i][index] > 0)
			return true;
	return false;
}

unsigned int Table::GetUserListSize()
{
	return sql.UserList.size();
}


//bool cmp1(vector<string>& a, vector<string>& b)
//{
//	for (unsigned int i = 0; i < MultikeywordSortIndex.size(); i++)
//	{
//		if (a[MultikeywordSortIndex[i]] != b[MultikeywordSortIndex[i]])
//			return a[MultikeywordSortIndex[i]] > b[MultikeywordSortIndex[i]];
//		else
//			continue;
//	}
//	return true;
//}

//void Table::ASCSortOutput()
//{
//	vector<vector<string>> new_vec;
//	for (unsigned int i = 0; i < vec.size(); i++)
//		new_vec.push_back(vec[i]);
//	sort(new_vec.begin(), new_vec.end(),cmp1);
//	for (unsigned int i = 0; i < new_vec.size(); i++)
//	{
//		for (unsigned int j = 0; j < ColumnName.size(); j++)
//			cout << setw(20) << new_vec[i][j];
//		cout << endl;
//	}
//}

//bool cmp2(vector<string>& a, vector<string>& b)
//{
//	for (unsigned int i = 0; i < MultikeywordSortIndex.size(); i++)
//	{
//		if (a[MultikeywordSortIndex[i]] != b[MultikeywordSortIndex[i]])
//			return a[MultikeywordSortIndex[i]] < b[MultikeywordSortIndex[i]];
//		else
//			continue;
//	}
//	return true;
//}

//void Table::DESCSortOutput()
//{
//	vector<vector<string>> new_vec;
//	for (unsigned int i = 0; i < vec.size(); i++)
//		new_vec.push_back(vec[i]);
//	sort(new_vec.begin(), new_vec.end(), cmp2);
//	for (unsigned int i = 0; i < new_vec.size(); i++)
//	{
//		for (unsigned int j = 0; j < ColumnName.size(); j++)
//			cout << setw(20) << new_vec[i][j];
//		cout << endl;
//	}
//}


void Table::PowerSynchronize()
//表的权限信息同步
{
	string order = "tablepower/" + name + "Power.txt";
	ofstream out(order.c_str(), ios::trunc);
	if (!out)
		cout << name << "Power.txt can't be opened!" << endl;
	else
	{
		unsigned int size = GetUserListSize();
		vector<User*> &v = sql.GetUserList();
		for (unsigned int i = 0; i < POWER_NAME_NUM; i++)
		{
			out << POWER_NAME[i] << endl;
			for(unsigned int j=0;j<size;j++)
				for (unsigned int k = 0; k < size; k++)
					if (PowerMatrix[i][j][k])
						for (unsigned int l = 0; l < PowerMatrix[i][j][k]; l++)
							out << v[j]->ID << " ==> " << v[k]->ID << endl;
		}
		out.close();
	}
}

void Table::Synchronize()
//表的内容同步
{
	string order = "table/" + sql.tostring(FileName);
	ofstream out(order.c_str(), ios::trunc);
	if (!out)
		cout << sql.tostring(FileName) << " can't be opened!" << endl;
	else
	{
		unsigned int num = ColumnName.size();
		unsigned int i = 0;
		for (; i < num-1; i++)
			out << ColumnName[i]<<" ";
		out << ColumnName[i] << endl;
		if (empty(vec))
		{
			out.close();
			return;
		}
		else
		{
			unsigned int row = vec.size();
			unsigned int i = 0;
			for (; i < row - 1; i++)
			{
				unsigned int j = 0;
				for (; j < num - 1; j++)
					if (vec[i][j] != "")
						out << vec[i][j] << " ";
					else
						out << "null";
				if (vec[i][j] != "")
					out << vec[i][j] <<endl;
				else
					out << "null" << endl;
			}
			unsigned int j = 0;
			for (; j < num - 1; j++)
				if (vec[i][j] != "")
					out << vec[i][j] << " ";
				else
					out << "null";
			if (vec[i][j] != "")
				out << vec[i][j] << endl;
			else
				out << "null" << endl;
		}
		out.close();
	}
}
