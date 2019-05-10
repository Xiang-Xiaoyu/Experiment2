#include"SQL.h"


bool mySQL::IsTableName(string n)
//�ж�n�ǲ���table�����������ж�һ�����Ƿ����
{
	vector<Table*>::iterator it = TableList.begin();
	for (; it != TableList.end(); ++it)
		if (n == (*it)->name)
			return true;
	return false;
}

bool mySQL::IsTableFileName(string fn)
//�ж�fn�ǲ����Ѵ��ڵı��ļ���
{
	vector<Table*>::iterator it = TableList.begin();
	for (; it != TableList.end(); ++it)
	{
		string ss;
		if ((ss=tostring((*it)->FileName)) == fn)
			return true;
	}
	return false;
}

int mySQL::FindTableIndex(string s)
//�ҵ���Ϊs��table��sql.tablelist�е��±�                                                                                                                                          
{
	unsigned int size = TableList.size();
	for (unsigned int i = 0; i < size; i++)
		if (s == TableList[i]->name)
			return i;
	return -1;
}

int mySQL::FindTableIndexByFileName(string s)
//����table�ļ����ҳ�table�±�
{
	unsigned int size = TableList.size();
	string ss;
	for (unsigned int i = 0; i < size; i++)
		if ((ss=tostring(TableList[i]->FileName)) == s)
			return i;
	return -1;
}

string mySQL::tostring(string & s)
{
	string ss = s;
	for (unsigned int i = ss.length() - 1; i >= 0; i--)
		if (ss[i] == '\0')
			ss.pop_back();
		else if (ss[i] != '\0')
			break;
	return ss;
}

void mySQL::Revoke(string user1, string user2, string table, string powername)
{
	int index1 = MapUserIdToInt(user1);
	int index2 = MapUserIdToInt(user2);
	int index3 = MapPowerNameToInt(powername);
	int index = FindTableIndex(table);
	if (index1 == -1 || index2 == -1 || index3 == -1 || index == -1)
	{
		cout << user1 << " <== " << user2 << " in POWER " << powername << " of table " << table << " failed!" << endl;
		return;
	}
	if (TableList[index]->PowerMatrix[index3][index1][index2]>1)
	{
		TableList[index]->PowerMatrix[index3][index1][index2]--;
		return;
	}
	else if (TableList[index]->PowerMatrix[index3][index1][index2] == 1)
	{
		TableList[index]->PowerMatrix[index3][index1][index2]--;
		for (unsigned int i = 0; i < UserList.size(); i++)
			TableList[index]->PowerMatrix[index3][index1][i] = 0;
	}
	else
		return;
}


void mySQL::DropTable(string tablename)
//ɾ��һ�����ڵ���Ϊtablename�ı��һ�в���ͬ����Ϣ
{
	unsigned int index = FindTableIndex(tablename);
	string order1 ="del tablepower/"+tablename+"Power.txt";
	system(order1.c_str());
	string order2 = "del table/" + tostring(TableList[index]->FileName);
	system(order2.c_str());
	unsigned int size = ApplyPower.size();
	for(int i=0;i<size;i++)
		if (ApplyPower[i]->TableName == tablename)
		{
			delete ApplyPower[i];
			ApplyPower.erase(i + ApplyPower.begin());
			i--;
		}
	ApplyPowerSynchronize();
	delete TableList[index];
	TableList.erase(index + TableList.begin());
	TableListSynchronize();
}

void mySQL::AskApplyPower()
//��½��ѯ��Ȩ�������Ϣ
{
	int size = ApplyPower.size(), i = 0;
	for (; i < size; i++)
	{
		if (ApplyPower[i]->admitted == CurrentUser->ID)
		{
			cout << "At" << ApplyPower[i]->time << ", You ask " << ApplyPower[i]->admit << " for";
			cout << " Power " << ApplyPower[i]->PowerName<<" of "<< ApplyPower[i]->TableName;
			cout << ",It hasn't passed yet,Please wait patiently!" << endl;
		}
	}
	for (i = 0; i < ApplyPower.size(); i++)
	{
		if (ApplyPower[i]->admit == CurrentUser->ID)
		{
			cout << "User " << ApplyPower[i]->admitted << " At " << ApplyPower[i]->time;
			cout << ", ask you for power " << ApplyPower[i]->PowerName;
			cout << " of Table " << ApplyPower[i]->TableName<< endl;
			cout << "Additional information:" << ApplyPower[i]->information<<endl;
			cout << "  Whether or not authorized y/n?"<<endl;
			char c = '\0';
			unsigned int index = FindTableIndex(ApplyPower[i]->TableName);
			while (c != 'y' && c != 'Y' && c != 'n' && c != 'N')
			{
				cout << "(mysql)==>(y/n?):";
				cin >> c;
				if (c == 'y' || c == 'Y')
				{
					TableList[index]->PowerMatrix[MapPowerNameToInt(ApplyPower[i]->PowerName)][MapUserIdToInt(ApplyPower[i]->admit)][MapUserIdToInt(ApplyPower[i]->admitted)]++;
					TableList[index]->PowerSynchronize();
					delete ApplyPower[i];
					ApplyPower.erase(i + ApplyPower.begin());
					i--;
					ApplyPowerSynchronize();
				}
				else if (c == 'n' || c == 'N')
				{
					delete ApplyPower[i];
					ApplyPower.erase(i + ApplyPower.begin());
					i--;
					ApplyPowerSynchronize();
				}
				else
					cout << "Input Erro! Please Input y or n or Y or N" << endl;
			}
			getchar();
		}
	}
}

mySQL::mySQL()
{
	ifstream user_in("important/UserList.txt");
	if (!user_in)
		cout << "UserList.txt Cannot be opened!" << endl;
	else
	{
		string s;
		istringstream ss;
		string s1, s2;
		while (!user_in.eof())
		{
			getline(user_in, s);
			if (s == "")
				continue;
			ss.str(s);
			ss >> s1 >> s2;
			ss.clear();
			User* p = new User;
			p->ID = s1;
			p->Password = s2;
			UserList.push_back(p);
		}
		user_in.close();
	}
	//����user��Ϣ

	unsigned int user_num = UserList.size();
	ifstream in1("important/TableList.txt");//��TableListд���ڴ�
	if (!in1)
		cout << "TableList.txt Cannot be opened!" << endl;
	else
	{
		string s;						//TableList.txt���һ������
		istringstream ss;				//TableList.txt���һ������
		string s1, s2, s3;				//TableList.txtһ���е�����string
		while (!in1.eof())
		{
			getline(in1, s);
			if (s == "")
				continue;
			ss.str(s);
			ss >> s1 >> s2 >> s3;
			ss.clear();
			Table* t = new Table(s1, s2, s3);
			TableList.push_back(t);
		}
		in1.close();
	}
	//ֻ����table��������������

	ifstream in2("important/ApplyPower.txt");
	if (!in2)
		cout << "ApplyPower.txt can't be opened!" << endl;
	else
	{
		string s;
		while (!in2.eof())
		{
			getline(in2, s);
			if (s == "")
				continue;
			else if (IsTableName(s))
			{
				apply* p = new apply;
				p->TableName = s;
				string s1, s2,s3;
				getline(in2, s1);
				getline(in2, s2);
				getline(in2, s3);
				p->information = s3;
				p->PowerName = s1;
				istringstream ss(s2);
				string a1, a2, a3, a4, a5;
				ss >> a1 >> a2 >> a3 >> a4 >> a5;
				ss.clear();
				p->admit = a3;
				p->admitted = a1;
				p->time = a5;
				ApplyPower.push_back(p);
			}
		}
	}
	//��������Ȩ�޵���Ϣ
}

void mySQL::run()
{
	string s;
	while (1)
	{
		CurrentUser = nullptr;
		cout << "~$ ";
		getline(cin, s);
		if (s == "")
			continue;
		else if (s == "mySQL")
		{
			LogIn();
		}
		else if (s == "quit")
		{
			break;
		}
	}
}

void mySQL::LogIn()
{
	string s1, s2;
	int times = 0;
	cout << "(mysql)==>login:";
	cin >> s1;
	do
	{
		cout << "(mysql)==>password:";
		char c;
		s2 = "";
		int len = 0;
		while ((c = _getch()) != '\n'&&c != '\r')
		{
			if (c == '\b')
			{
				if (len > 0)
				{
					printf("\b \b");
					len--;
					s2.pop_back();
				}
				continue;
			}
			cout << "*";
			len++;
			s2.push_back(c);
		}
		if (c == '\n' || c == '\r')
			cout << endl;
		unsigned int i = 0;
		unsigned int size = UserList.size();
		for (; i < size; i++)
		{
			if (s1 == UserList[i]->ID && s2 == UserList[i]->Password)
			{
				CurrentUser = UserList[i];
				cout << "Successfully log in!" << endl;
				cout << "Loading..." << endl;
				AfterLogIn();
				return;
			}
		}
		if (i >= UserList.size())
		{
			times++;
			if (times >= 3)
			{
				cout << "You've failed for three times!" << endl;
				return;
			}
			cout << "you have " << 3 - times << " more times" << endl;
			string order;
			cout << "quit or continue?" << endl;
			cout << "(mysql)==>(quit/any other operations):";
			getline(cin, order);
			if (order == "quit")
			{
				return;
			}
			if (order != "")
				;
		}
	} while (1);
}

void mySQL::AfterLogIn()
{
	string order;
	getchar();
	AskApplyPower();
	while (1)
	{
		cout << "(" << CurrentUser->ID << ")==>order:";
		getline(cin, order);
		if (order == "")continue;
		istringstream ss(order);
		vector<string> v;
		string s;
		while (ss >> s)
			v.push_back(s);
		ss.clear();
		if (order == "quit")
			return;
		legal.LegalOrder(order, v);
	}
}

void mySQL::ApplyPowerSynchronize()
{
	ofstream out("important/ApplyPower.txt", ios::trunc);
	if (!out)
		cout << "ApplyPower.txt Cannot be opened!" << endl;
	else
	{
		if (empty(ApplyPower))
		{
			out << endl;
			out.close();
			return;
		}
		else
		{
			unsigned int size = ApplyPower.size();
			for (unsigned int i = 0; i < size; i++)
			{
				out << ApplyPower[i]->TableName << endl;
				out << ApplyPower[i]->PowerName << endl;
				out << ApplyPower[i]->admitted << " " << "==> " << ApplyPower[i]->admit << " time: " << ApplyPower[i]->time << endl;
				out << ApplyPower[i]->information << endl;
			}
		}
		out.close();
	}
}

void mySQL::UserListSynchronize()
{
	ofstream out("important/UserList.txt", ios::trunc);
	if (!out)
		cout << "UserList.txt can't be opened!" << endl;
	else
	{
		if (empty(UserList))
		{
			out << endl;
			out.close();
			return;
		}
		unsigned int size = UserList.size();
		for (unsigned int i = 0; i < size; i++)
			out << UserList[i]->ID << " " << UserList[i]->Password << endl;
		out.close();
	}
}

void mySQL::TableListSynchronize()
{
	unsigned int num = TableList.size();
	ofstream out("important/TableList.txt", ios::trunc);
	if (!out)
		cout << "TableList.txt can't be opened!" << endl;
	else
	{
		if (empty(TableList))
		{
			out << endl;
			out.close();
			return;
		}
		unsigned int i = 0;
		for (; i < num; i++)
			out << TableList[i]->name << " " << tostring(TableList[i]->FileName) << " " << TableList[i]->OwnerName << endl;
		out.close();
	}
}

bool IsPowerName(string s)
//�ж��ַ���s�ǲ��Ǵ�д��Ȩ����
{
	for (unsigned int i = 0; i < POWER_NAME_NUM; i++)
		if (s == POWER_NAME[i])
			return true;
	return false;
}

int MapPowerNameToInt(string s)
//��Ȩ����ӳ�䵽��Ӧ�±�
{
	if (IsPowerName(s))
	{
		for (unsigned int i = 0; i < POWER_NAME_NUM; i++)
			if (s == POWER_NAME[i])
				return i;
	}
	else
		return -1;
}

int MapUserIdToInt(string s)
//��userIDӳ�䵽��Ӧ�±�
{
	unsigned int i = 0, num = sql.UserList.size();
	for (; i < num; i++)
		if (sql.UserList[i]->ID == s)
			return i;
	return -1;
}
