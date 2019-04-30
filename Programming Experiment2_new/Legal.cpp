#include"mySQL.h"

unsigned int FindIndexInTableList(string name)
{
	unsigned int index = 0;
	vector<Table*> v = sql.GetTableList();
	unsigned int num = v.size();
	for (unsigned int i = 0; i < num; i++)
	{
		if (v[i]->GetName() == name)
		{
			index = i;
			break;
		}
	}
	return index;
}

bool IsTxt(string s)
{
	bool flag = false;
	int num = s.length();
	if (num <= 4)
		return false;
	else
	{
		if (s[num - 4] == '.'&&s[num - 3] == 't'&&s[num - 2] == 'x'&&s[num - 1] == 't')
			flag = true;
		for (int j = 0; j < num - 4; j++)
			flag = flag && (isdigit(s[j]) || isalpha(s[j]) || s[j] == '_');
		return flag;
	}
}

bool NoNameRepetition(string name)//没重复返回true，有重复返回false
{
	vector<Table*> &v = sql.GetTableList();
	if (empty(v))
		return true;
	unsigned int num = v.size();
	for (unsigned int i = 0; i < num; i++)
	{
		if (v[i]->GetName() == name)
			return false;
	}
	return true;
}

bool NoFileNameRepetition(string filename)
{
	vector<Table*> &v = sql.GetTableList();
	if (empty(v))
		return true;
	unsigned int num = v.size();
	for (unsigned int i = 0; i < num; i++)
	{
		if (v[i]->GetFileName() == filename)
			return false;
	}
	return true;
}

int BracketNum(string s)
{
	int num = 0;
	int len = s.length();
	int i = 0;
	while (i < len)
	{
		bool flag = false;
		for (; i < len; i++)
		{
			if (s[i] == '(')
			{
				flag = true;
				break;
			}
		}
		for (; i < len; i++)
		{
			if (s[i] == ')'&&flag)
			{
				num++;
				break;
			}
		}
	}
	return num;
}

bool LegalFirstBracket(string s)
{
	bool flag = true;
	int len = s.length();
	int i = 0;
	int j = 0;
	for (; i < len; i++)
		if (s[i] == '(')break;
	i++;
	for (; s[i]!=')'; i++)
	{
		if (s[i] != ',')
		{
			j++;
			continue;
		}
		if (j == 0)
			return false;
		if (s[i + 1] != ' '&&s[i + 1] != ')')
			return false;
		j = 0;
	}
	return true;
}

bool LegalSecondBracket(string s)
{
	bool flag1 = false, flag2 = false;
	int len = s.length();
	int i = 0;
	for (; i < len; i++)
	{
		if (s[i] == '(')
			flag1 = true;
		if (s[i] == ')'&&flag1)
			flag2 = true;
		if (flag1&&flag2)
			break;
	}
	i++;
	int j = 0;
	for (; i < len; i++)
	{
		if (s[i] == '(')
			break;
	}
	i++;
	for (; s[i] != ')'; i++)
	{
		if (s[i] != ',')
		{
			j++;
			continue;
		}
		if (j == 0)
			return false;
		if (s[i + 1] != ' '&&s[i + 1] != ')')
			return false;
		j = 0;
	}
	return true;
}

void ExtractBracketFirstContent(string s,vector<string>& v)
{
	int len = s.length();
	int i = 0;
	int num = 0;
	int index;
	for (; i < len; i++)
	{
		if (s[i] == '(')
		{
			index = i + 1;
			break;
		}
	}
	i++;
	for (; s[i] != ')'; i++)
		num++;
	string piece = s.substr(index, num);
	istringstream ss(piece);
	string str;
	while (ss >> str)
	{
		if (str[str.length() - 1] == ',')
		{
			str.pop_back();
			v.push_back(str);
			continue;
		}
		v.push_back(str);
	}
	ss.clear();
}

 void ExtractBracketSecondContent(string s, vector<string>& v)
{
	bool flag1 = false, flag2 = false;
	int len = s.length();
	int i = 0;
	for (; i < len; i++)
	{
		if (s[i] == '(')
			flag1 = true;
		if (s[i] == ')'&&flag1)
			flag2 = true;
		if (flag1&&flag2)
			break;
	}
	i++;
	int index = 0;
	for (; i < len; i++)
	{
		if (s[i] == '(')
		{
			index = i + 1;
			break;
		}
	}
	i++;
	int num = 0;
	for (; s[i] != ')'; i++)
		num++;
	string piece = s.substr(index, num);
	istringstream ss(piece);
	string str;
	while (ss >> str)
	{
		if (str[str.length() - 1] == ',')
		{
			str.pop_back();
			v.push_back(str);
			continue;
		}
		v.push_back(str);
	}
	ss.clear();
}

bool IsCreate1(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	unsigned int num = v.size();
	if (num < 6)
		return false;
	if (v[0] == "CREATE"&&v[1] == "TABLE"&&v[num - 2] == "TO"&&IsTxt(v[num-1])&&NoNameRepetition(v[2]) && NoFileNameRepetition(v[num - 1]))
	{
		if (BracketNum(s) == 1)
		{
			if (LegalFirstBracket(s))
			{
				vector<string> CN;
				ExtractBracketFirstContent(s, CN);
				Table* newt = new Table(v[2], v[num - 1], sql.GetCurrentUser()->ID, CN, vector<vector<string>>(), vector<Admission*>());
				string order1 = "table/" + v[num - 1];
				ofstream out1(order1.c_str());
				if (!out1)
					cout << v[num - 1] << " can't be opened!" << endl;
				else
				{
					int n = CN.size();
					int i = 0;
					for (; i < n - 1; i++)
						out1 << CN[i] << " ";
					out1 << CN[i];
					out1.close();
				}
				string order2 = "tableadmission/" + v[2] + "Admission.txt";
				ofstream out2(order2.c_str());
				if (!out2)
					cout << v[2]<<"Admission.txt"<< " can't be opened!" << endl;
				else
				{
					for (int i = 0; i < 4; i++)
						out2 << AdmissionName[i] << endl;
					out2 << AdmissionName[4];
					out2.close();
				}
				sql.GetTableList().push_back(newt);
				sql.TableListSynchronize();
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool IsCreate2(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	unsigned int num = v.size();
	if (num!=5)
		return false;
	if (v[0] == "CREATE"&&v[1] == "TABLE"&&v[3] == "FROM"&&NoNameRepetition(v[2]) && IsTxt(v[4]) && !NoFileNameRepetition(v[4]))
	{
		vector<Table*>& tablelist = sql.GetTableList();
		unsigned int num =  tablelist.size();
		unsigned int i = 0, j = 0;
		for (; i < num; i++)
			if (tablelist[i]->GetFileName() == v[4])
			{
				j = i;
				break;
			}
		string new_file_name = v[4];
		new_file_name.pop_back(), new_file_name.pop_back();
		new_file_name.pop_back(), new_file_name.pop_back();
		new_file_name += "_copy.txt";
		Table* newt = new Table(v[2], new_file_name, sql.GetCurrentUser()->ID, *(tablelist[i]));
		newt->Synchronize();
		tablelist.push_back(newt);
		sql.TableListSynchronize();
		sql.AdmissionSynchronize(v[2]);
		return true;
	}
	else
		return false;
}

bool IsDrop(string s)
{
	vector<string> vec;
	istringstream ss(s);
	string str;
	while (ss >> str)
		vec.push_back(str);
	if (vec.size() != 3)
		return false;
	if (vec[0] == "DROP"&&vec[1] == "TABLE")
		return true;
	else
		return false;
}


bool HasAdmission(string username, string tablename, string admissionname)
{
	vector<Table*>& v = sql.GetTableList();
	unsigned int i = 0, numi = v.size();
	for (; i < numi; i++)
	{
		if (v[i]->GetName() == tablename)
		{
			if (v[i]->GetOwnerName() == username)
				return true;
			vector<Admission*>& u = v[i]->GetAdmission();
			unsigned int j = 0, numj = u.size();
			for (; j < numj; j++)
			{
				if (u[j]->AdmissionName == admissionname && u[j]->admitted == username)
					return true;
			}
		}
	}
	return false;
}

bool IsInsert1(string s)
{
	vector<string> v;
	istringstream  ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	if (v.size() < 5)
		return false;
	if (v[0] == "INSERT"&&v[1] == "INTO"&&v[3] == "VALUES")
	{
		if (LegalFirstBracket(s))
		{
			int len = s.length();
			int num = 0;
			for (int i = 0; i < len; i++)
			{
				if (s[i] == ')')
				{
					num = i;
					break;
				}
			}
			if (num == len-1)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool IsInsert2(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	if (v.size() < 6)
		return false;
	if (v[0]=="INSERT"&&v[1] == "INTO" && 2 == BracketNum(s))
	{
		if (LegalFirstBracket(s) && LegalSecondBracket(s))
		{
			int len = s.length();
			int num = 0;
			bool flag = false;
			for (int i = 0; i < len; i++)
			{
				if (s[i] == ')'&&flag)
				{
					num = i;
					break;
				}
				if (s[i] == ')')
					flag = true;
			}
			if (num == len-1)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool IsDelete1(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	if (v.size() != 7)
		return false;
	if (v[0] == "DELETE"&&v[1] == "FROM"&&v[3] == "WHERE"&&v[5] == "=")
		return true;
	else
		return false;
}

bool IsDelete2(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	if (v.size() != 4)
		return false;
	if (v[0] != "DELETE"&&v[1] == "*"&&v[2] == "FROM")
		return true;
	else
		return false;
}

bool IsSelect1(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	if (v.size() <= 3)
		return false;
	if (v[0] == "SELECT"&&v[v.size() - 2] == "FROM"&&v[1]!="DISTINCT"&&v[v.size()-3]!="*")
		return true;
	else
		return false;
}

bool IsSelect2(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	if (v.size() !=4)
		return false;
	if (v[0] == "SELECT"&&v[1] == "*"&&v[2] == "FROM")
		return true;
	else
		return false;
}

bool LegalSelectColumn(string s,unsigned int init)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int i = init;
	bool flag = true;
	for (; i < v.size() && v[i + 1] != "FROM"; i++)
		flag = flag && (v[i][v[i].length() - 1] == ',');
	flag = flag && (v[i + 1] == "FROM");
	return flag;
}

bool LegalSelectColumn2(string s, unsigned int init)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int i = init;
	bool flag = true;
	for (; i < v.size() && v[i + 1] != "ASC"&&v[i+1]!="DESC"; i++)
		flag = flag && (v[i][v[i].length() - 1] == ',');
	flag = flag && (v[i + 1] == "ASC"||v[i+1]=="DESC");
	return flag;
}


bool IsSelect3(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	if (v.size() < 5)
		return false;
	if (v[0] == "SELECT"&&v[1] == "DISTINCT"&&v[v.size() - 2] == "FROM")
		return true;
	else
		return false;
}

bool IsSelect4(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	if (v.size() < 8)
		return false;
	if (v[0] == "SELECT"&&v[1] == "*"&&v[2] == "FROM"&&v[4] == "ORDER"&&v[5] == "BY"&&v[v.size() - 1] == "ASC" || v[v.size() - 1] == "DESC")
		return true;
	else
		return false;
}

bool IsSelect5(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	if (v.size() < 8)
		return false;
	if (v[0] == "SELECT"&&v[v.size() - 6] == "FROM"&&v[v.size() - 4] == "WHERE"&&v[v.size() - 2] == "=")
		return true;
	else
		return false;
}

bool IsSelect6(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	if (v.size() < 6)
		return false;
	if (v[0] == "SELECT"&&v[v.size() - 4] == "FROM"&&v[v.size() - 2]=="TO"&&NoFileNameRepetition(v[v.size() - 1])&&IsTxt(v[v.size() - 1]))
		return true;
	else
		return false;
}

bool IsGrant(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(s);
	if (v.size() < 6)
		return false;
	if (v[0] == "GRANT"&&v[v.size() - 2] == "to"&&v[v.size() - 4] == "on")
	{
		if ((v[1] == "DROP" || v[1] == "DELETE" || v[1] == "INSERT" || v[1] == "SELECT"||v[1]=="AVAILABLE")&&!NoNameRepetition(v[v.size()-3]))
		{
			if (v[v.size() - 1] == "public")
				return true;
			bool flag = true;
			for (unsigned int i = 0; i < sql.GetUserList().size(); i++)
			{
				if (v[v.size() - 1] == sql.GetUserList()[i]->ID)
				{
					flag = false;
					return true;
				}
			}
			if (flag)
				return false;
		}
		else
		return false;
	}
	else
		return false;
}

bool IsRevoke(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(s);
	if (v.size() < 6)
		return false;
	if (v[0] == "REVOKE"&&v[v.size() - 4] == "on" && !NoNameRepetition(v[v.size() - 3]) && v[v.size() - 2] == "from")
	{
		if(v[1] == "DROP" || v[1] == "DELETE" || v[1] == "INSERT" || v[1] == "SELECT" || v[1] == "AVAILABLE")
		{
			bool flag = true;
			for (unsigned int i = 0; i < sql.GetUserList().size(); i++)
			{
				if (v[v.size() - 1] == sql.GetUserList()[i]->ID)
				{
					flag = false;
					return true;
				}
			}
			if (flag)
				return false;
		}
		else
			return false;
	}
	else
		return false;
}


bool IsAdmissionWay(string user1, string user2, string tablename,string admissionname)
{
	unsigned int index=FindIndexInTableList(tablename);
	vector<Admission*> v = sql.GetTableList()[index]->GetAdmission();
	vector<Admission*> s;
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i]->AdmissionName == admissionname)
			s.push_back(v[i]);
	return RecursionFindAdmissionWay(s, user1, user2);
}

bool RecursionFindAdmissionWay(vector<Admission*> &s, string user1, string user2)
{
	vector<Admission*> ss;
	for (unsigned int i = 0; i < s.size(); i++)
	{
		if (s[i]->admit == user1)
			ss.push_back(s[i]);
		else if (s[i]->admitted == user2)
			return true;
	}
	if (!empty(ss))
	{
		for (unsigned int j = 0; j < ss.size(); j++)
			if (RecursionFindAdmissionWay(ss, ss[j]->admitted, user2))
				return true;
	}
	else
		return false;
	vector<Admission*>().swap(ss);
	ss.clear();
	return false;
}

void RevokeAdmission(string user1, string user2, string tablename, string admissionname)
{
	unsigned int index = FindIndexInTableList(tablename);
	vector<Admission*> &ss = sql.GetTableList()[index]->GetAdmission();
	vector<Admission*> sss;
	if (HasAdmission(user1, tablename,admissionname))
	{
		for (unsigned int i = 0; i < ss.size(); i++)
		{
			if (ss[i]->AdmissionName == admissionname)
				sss.push_back(ss[i]);
		}
		if(!empty(ss))
			RecursionRevokeAdmission(user1, user2, tablename, ss);
	}
}

void RecursionRevokeAdmission(string user1,string user2,string tablename,vector<Admission*>& ss)
{
	for (int i = 0; i < ss.size(); i++)
	{
		if (ss[i]->admit == user1 && ss[i]->admitted == user2)
		{
			delete ss[i];
			ss.erase(ss.begin() + i);
			i--;
			if (!HasAdmission(user2, tablename, ss[i]->AdmissionName))
			{
				for (unsigned int j = 0; j < ss.size(); j++)
					if (ss[j]->admit == user2)
						RecursionRevokeAdmission(user2, ss[j]->admitted, tablename, ss);
			}
			break;
		}
	}
}