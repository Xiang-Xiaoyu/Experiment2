#include"mySQL.h"

bool mySQL::IsTableName(string s)
{
	if (empty(TableList)||s=="")
		return false;
	else
	{
		s.pop_back();
		unsigned int n = TableList.size();
		for (unsigned int i = 0; i < n; i++)
		{
			if (TableList[i]->GetName() == s)
				return true;
		}
		return false;
	}
}

void mySQL::DropTable(string tablename)
{
	unsigned int i = 0, num = TableList.size();
	for (; i < num; i++)
	{
		if (TableList[i]->GetName() == tablename)
		{
			string order1 = "del tablename/" + TableList[i]->GetFileName();
			system(order1.c_str());
			string order2 = "del tableadmission/" + TableList[i]->GetName() + "Admission.txt";
			system(order2.c_str());
			delete TableList[i];
			TableList.erase(TableList.begin() + i);
			TableListSynchronize();
			break;
		}
	}
	unsigned int j = 0;
	for (; j < ApplyAdmission.size(); j++)
	{
		if (ApplyAdmission[j]->TableName == tablename)
		{
			delete ApplyAdmission[j];
			ApplyAdmission.erase(ApplyAdmission.begin() + j);
			j--;
		}
	}
	ApplyAdmissionSynchronize();
}

void mySQL::AskApplyAdmission()
{
	unsigned int size = ApplyAdmission.size(), i = 0;
	for (; i < size; i++)
	{
		if (ApplyAdmission[i]->admitted == CurrentUser->ID)
		{
			cout << "您于" << ApplyAdmission[i]->time << "向用户" << ApplyAdmission[i]->admit << "发送的";
			cout << "有关表" << ApplyAdmission[i]->TableName << "的" << ApplyAdmission[i]->AdmissionName << "权限";
			cout << "的请求尚未通过，请耐心等待" << endl;
		}
	}
	for (i = 0; i < ApplyAdmission.size();i++ )
	{
		if (ApplyAdmission[i]->admit == CurrentUser->ID)
		{
			cout << "用户" << ApplyAdmission[i]->admitted << "于时间" << ApplyAdmission[i]->time;
			cout<<"向您发送了有关表" << ApplyAdmission[i]->TableName;
			cout << "的" << ApplyAdmission[i]->AdmissionName << "权限申请,是否允许y/n?" << endl;
			char c='\0';
			while (c != 'y' && c != 'Y' && c != 'n' && c != 'N')
			{
				cout << "(mysql)==>(y/n?):";
				cin >> c;
				if (c == 'y' || c == 'Y')
				{
					Admission* p = new Admission;
					p->AdmissionName = ApplyAdmission[i]->AdmissionName;
					p->admit = ApplyAdmission[i]->admit;
					p->admitted = ApplyAdmission[i]->admitted;
					unsigned int index = 0, size = TableList.size();
					for (; index < size; index++)
					{
						if (TableList[index]->GetName() == ApplyAdmission[i]->TableName)
						{
							TableList[index]->GetAdmission().push_back(p);
							AdmissionSynchronize(ApplyAdmission[i]->TableName);
							delete ApplyAdmission[i];
							ApplyAdmission.erase(ApplyAdmission.begin() + i);
							ApplyAdmissionSynchronize();
							cout << "成功授权!" << endl;
							i--;
							break;
						}
					}
				}
				else if (c == 'n' || c == 'N')
				{
					delete ApplyAdmission[i];
					ApplyAdmission.erase(ApplyAdmission.begin() + i);
					ApplyAdmissionSynchronize();
					cout << "成功拒绝授权!" << endl;
					i--;
				}
				else
					cout << "输入错误.请输入y/n?(大小写均可)" << endl;
			}
		}
	}
}

mySQL::mySQL()
{
	ifstream in1("important/TableList.txt");
	if (!in1)
		cout << "TableList.txt Cannot be opened!" << endl;
	else
	{
		string s;
		istringstream ss;
		string s1, s2, s3;
		vector<vector<string>> v;
		vector<string> vrow;
		vector<Admission*> ad;
		vector<string> CN;
		while (!in1.eof())
		{
			getline(in1, s);
			if (s == "")
				continue;
			ss.str(s);
			ss >> s1 >> s2 >> s3;
			ss.clear();
			string s2_order = "table/" + s2;
			ifstream in(s2_order.c_str());
			if (!in)
				cout << s2 << " can't be opened!" << endl;
			else
			{
				string s4;
				istringstream ss1;
				getline(in, s4);
				ss1.str(s4);
				string s5;
				while (ss1 >> s5)
					CN.push_back(s5);
				ss1.clear();
				while(getline(in, s4))
				{
					ss1.str(s4);
					while (ss1 >> s5)
						vrow.push_back(s5);
					v.push_back(vrow);
					vector<string>().swap(vrow);
					vrow.clear();
					ss1.clear();
				}
				in.close();
			}
			string admission_order = "tableadmission/" + s1 + "Admission.txt";
			ifstream fin(admission_order.c_str());
			if (!fin)
				cout << s1<<"Admission.txt can't be opened!" << endl;
			else
			{
				string s0;
				string admission_name;
				while (getline(fin, s0))
				{
					bool f = IsAdmissionName(s0);
					if (f)
					{
						admission_name = s0;
						admission_name.pop_back();
						continue;
					}
					string str1, str2, str3;
					istringstream ss_str(s0);
					ss_str >> str1 >> str2 >> str3;
					ss_str.clear();
					Admission* p = new Admission;
					p->AdmissionName = admission_name;
					p->admit = str1;
					p->admitted = str2;
					ad.push_back(p);
				}
				fin.close();
			}
			Table* newt = new Table(s1, s2, s3,CN,v,ad);
			TableList.push_back(newt);
			vector<string>().swap(CN);
			CN.clear();
			vector<vector<string>>().swap(v);
			v.clear();
			vector<Admission*>().swap(ad);
			ad.clear();
		}
		in1.close();
	}
	ifstream in2("important/UserList.txt");
	if (!in2)
		cout << "UserList.txt Cannot be opened!" << endl;
	else
	{
		string s;
		istringstream ss;
		string s1, s2;
		while (!in2.eof())
		{
			getline(in2, s);
			if (s == "")
				continue;
			ss.str(s);
			ss >> s1 >> s2;
			User* p = new User;
			p->ID = s1;
			p->Password = s2;
			UserList.push_back(p);
			ss.clear();
		}
		in2.close();
	}
	ifstream in3("important/ApplyAdmission.txt");
	if (!in3)
		cout << "ApplyAdmisson.txt can't be opened!" << endl;
	else
	{
		if (!in3)
			cout << "ApplyAdmission.txt can't be opened!" << endl;
		else
		{
			string s;
			while (!in3.eof())
			{
				getline(in3, s);
				if (s == "")
					continue;
				else if (IsTableName(s))
				{
					apply* p=new apply;
					s.pop_back();
					p->TableName = s;
					string s1, s2;
					getline(in3, s1);
					getline(in3, s2);
					s1.pop_back();
					p->AdmissionName = s1;
					istringstream ss(s2);
					string a1, a2, a3, a4, a5;
					ss >> a1 >> a2 >> a3 >> a4 >> a5;
					p->admit = a3;
					p->admitted = a1;
					p->time = a5;
					ApplyAdmission.push_back(p);
				}
			}
		}
	}
	in3.close();
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

void mySQL::ApplyAdmissionSynchronize()
{
	ofstream out("important/ApplyAdmission.txt",ios::trunc);
	if (!out)
		cout << "ApplyAdmission.txt Cannot be opened!" << endl;
	else
	{
		if (empty(ApplyAdmission))
		{
			out << endl;
			out.close();
			return;
		}
		else
		{
			unsigned int size = ApplyAdmission.size();
			for (unsigned int i = 0; i < size; i++)
			{
				out << ApplyAdmission[i]->TableName << ":" << endl;
				out << ApplyAdmission[i]->AdmissionName << ":" << endl;
				out << ApplyAdmission[i]->admitted << " " << "==> " <<ApplyAdmission[i]->admit << " time: " << ApplyAdmission[i]->time << endl << endl;
			}
		}
		out.close();
	}
}

void mySQL::UserListSynchronize()
{
	ofstream out("important/UserList.txt",ios::trunc);
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

void mySQL::AdmissionSynchronize(string tablename)
{
	string order = "tableadmission/" + tablename + "Admission.txt";
	ofstream out(order.c_str(), ios::trunc);
	if (!out)
		cout << tablename<<"Admission.txt can't be opened!" << endl;
	else
	{
		Table* p=nullptr;
		unsigned int size = TableList.size();
		for (unsigned int i = 0; i < size; i++)
		{
			if (TableList[i]->GetName() == tablename)
			{
				p = TableList[i];
				break;
			}
		}
		if (p == nullptr)
		{
			cout << "程序在文件"<<tablename<<"Admission.txt权限同步时出错" << endl;
			return;
		}
		vector<Admission*> &v = p->GetAdmission();
		sort(v.begin(), v.end(), cmp1);
		unsigned int num = v.size();
		for (unsigned int i = 0; i < 5; i++)
		{
			out << AdmissionName[i] << endl;
			bool flag = true;
			for (unsigned int j = 0; j < num; j++)
			{
				if ((v[j]->AdmissionName+":") == AdmissionName[i])
				{
					flag = false;
					out << v[j]->admit << " ==> " << v[j]->admitted << endl;
				}
				if (!flag&&(v[j]->AdmissionName+":") != AdmissionName[i])
					break;
			}
		}
		out.close();
	}
}

void mySQL::TableListSynchronize()
{
	unsigned int num = TableList.size();
	ofstream out("important/TableList.txt",ios::trunc);
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
		unsigned int i=0;
		for (; i < num; i++)
			out << TableList[i]->GetName() << " " << TableList[i]->GetFileName() << " " << TableList[i]->GetOwnerName() << endl;
		out.close();
	}
}

void mySQL::AfterLogIn()
{
	string order;
	getchar();
	AskApplyAdmission();
	while (1)
	{
		cout << "("<<CurrentUser->ID<<")==>order:";
		getline(cin, order);
		if (order == "")continue;
		istringstream ss(order);
		vector<string> v;
		string s;
		while (ss >> s)
			v.push_back(s);
		ss.clear();
		if (v[0] == "CREATE")
		{
			if (v.size() <= 5)
			{
				if (IsCreate2(order))
				{
					cout << "创建成功!" << endl;
				}
				else
					cout << "命令格式错误!" << endl;
			}
			else
			{
				if (IsCreate1(order))
				{
					cout << "创建成功!" << endl;
				}
				else
					cout << "命令格式错误!" << endl;
			}
		}
		else if (v[0] == "DROP")
		{
			if (IsDrop(order))
			{
				vector<string> vec;
				istringstream ss;
				string str;
				while (ss >> str)
					vec.push_back(str);
				if (!IsTableName(v[2] + ":"))
					cout << "没有名为" << v[2] << "的表!" << endl;
				else
				{
					if (HasAdmission(CurrentUser->ID, v[2], "DropAdmission"))
					{
						DropTable(v[2]);
						cout << "成功删除表" <<v[2]<< endl;
					}
				}
			}
			else
				cout << "命令格式错误" << endl;
		}
		else if (v[0] == "TABLE"&&v.size()==2)
		{
			if (v[1] == "LIST")
			{
				unsigned int num = TableList.size();
				cout << "    total:" << num << endl;
				for (unsigned int i = 0; i < num; i++)
				{
					cout << TableList[i]->GetName() << ":";
					if(HasAdmission(CurrentUser->ID,TableList[i]->GetName(),"AvailableAdmission"))
					{
						cout << "(" << TableList[i]->GetColumn() << "," << TableList[i]->GetRow() << ")";
						cout << " [";
						unsigned int j = 0;
						vector<string>& v = TableList[i]->GetColumnName();
						for (; j < v.size() - 1; j++)
							cout << v[j] << ", ";
						cout << v[j] << "] ";
						if (HasAdmission(CurrentUser->ID, TableList[i]->GetName(), "DropAdmission"))
							cout << "Drop, ";
						if(HasAdmission(CurrentUser->ID,TableList[i]->GetName(),"InsertAdmission"))
							cout<<"Insert, ";
						if (HasAdmission(CurrentUser->ID, TableList[i]->GetName(), "DeleteAdmission"))
							cout << "Delete, ";
						if (HasAdmission(CurrentUser->ID, TableList[i]->GetName(), "SelectAdmission"))
							cout << "Select, ";
					}
					if (TableList[i]->GetOwnerName() == CurrentUser->ID)
						cout << "[OWNER]" << endl;
					else
						cout << "[SERVICE]" << endl;
				}
			}
			else
				cout << "命令格式错误!" << endl;
		}
		else if (v[0] == "INSERT")
		{
			if (1 == BracketNum(order))
			{
				if (IsInsert1(order))
				{
					if (!NoNameRepetition(v[2]))
					{
						if (HasAdmission(CurrentUser->ID, v[2], "InsertAdmission"))
						{
							vector<string> v1;
							ExtractBracketFirstContent(order, v1);
							unsigned int num = TableList.size();
							unsigned int j = 0;
							for (unsigned int i = 0; i < num; i++)
							{
								if (v[2] == TableList[i]->GetName())
								{
									j = i;
									break;
								}
							}
							if (v1.size() != TableList[j]->GetColumn())
								cout << "括号中输入的值的数目与文件列数不一致!" << endl;
							else
							{
								TableList[j]->GetVec().push_back(v1);
								TableList[j]->Synchronize();
								cout << "插入成功!" << endl;
							}
							vector<string>().swap(v1);
							v1.clear();
						}
						else
							cout << "你没有表" << v[2] << "的Insert权限" << endl;
					}
					else
						cout << "名为" << v[2] << "的Table不存在!" << endl;
				}
				else
					cout << "命令格式错误!" << endl;
			}
			else if (2 == BracketNum(order))
			{
				if (IsInsert2(order))
				{
					if (!NoNameRepetition(v[2]))
					{
						if (HasAdmission(CurrentUser->ID, v[2], "InsertAdmission"))
						{
							vector<string> v1;
							vector<string> v2;
							ExtractBracketFirstContent(order, v1);
							ExtractBracketSecondContent(order, v2);
							if (v1.size() != v2.size())
								cout << "前后括号内内容数目不一致!" << endl;
							else
							{
								unsigned int num = TableList.size();
								unsigned int j = 0;
								for (unsigned int i = 0; i < num; i++)
								{
									if (v[2] == TableList[i]->GetName())
									{
										j = i;
										break;
									}
								}
								vector<string>& CN = TableList[j]->GetColumnName();
								unsigned int l = CN.size();
								unsigned int k = v1.size();
								bool flag0 = false;
								for (unsigned int i = 0; i < k; i++)
								{
									bool flag = false;
									for (unsigned int j = 0; j < l; j++)
									{
										if (v1[i] == CN[j])
										{
											flag = true;
											break;
										}
									}
									if (!flag)
									{
										cout << "第一个括号中的" << "\"" << v1[i] << "\"" << "不是表" << v[2] << "中列的名" << endl;
										flag0 = true;
									}
								}
								if (!flag0)
								{
									vector<string> v3;
									vector<string>& CN = TableList[j]->GetColumnName();
									unsigned int l = CN.size();
									unsigned int k = v1.size();
									for (unsigned int i = 0; i < l; i++)
									{
										bool flag = false;
										string s0 = "null";
										for (unsigned int j = 0; j < k; j++)
										{
											if (CN[i] == v1[j])
											{
												s0 = v2[j];
												flag = true;
											}
										}
										if (!flag)
											s0 = "null";
										v3.push_back(s0);
									}
									TableList[j]->GetVec().push_back(v3);
									TableList[j]->Synchronize();
									cout << "插入成功!" << endl;
								}
							}
						}
						else
							cout << "你没有表" << v[2] << "的Insert权限" << endl;
					}
					else
						cout << "名为" << v[2] << "的Table不存在!" << endl;
				}
				else
					cout << "命令格式错误！" << endl;
			}
		}
		else if (v[0] == "DELETE")
		{
		if (IsDelete1(order))
		{
			if (!NoNameRepetition(v[2]))
			{
				if (HasAdmission(CurrentUser->ID, v[2], "DeleteAdmission"))
				{
					unsigned int num = TableList.size();
					unsigned int j = 0;
					for (unsigned int i = 0; i < num; i++)
					{
						if (v[2] == TableList[i]->GetName())
						{
							j = i;
							break;
						}
					}
					vector<string> &CN = TableList[j]->GetColumnName();
					unsigned int k = CN.size();
					bool flag = false;
					unsigned int num0 = 0;
					for (unsigned int i = 0; i < k; i++)
					{
						if (v[4] == CN[i])
						{
							flag = true;
							num0 = i;
							break;
						}
					}
					if (!flag)
						cout << v[4] << " 这个列名不存在!" << endl;
					else
					{
						int sum = 0;
						vector<vector<string>>& vec = TableList[j]->GetVec();
						for (unsigned int i = 0; i < vec.size(); i++)
						{
							if (vec[i][num0] == v[6])
							{
								sum++;
								vec.erase(vec.begin() + i);
								i--;
							}
						}
						if (sum == 0)
							cout << "表中没有信息与之匹配!" << endl;
						else
						{
							TableList[j]->Synchronize();
							cout << "表中成功删除n行信息";
						}
					}
				}
				else
					cout << "你没有表" << v[2] << "的Delete权限" << endl;
			}
			else
				cout << "名为" << v[2] << "的Table不存在!" << endl;
		}
		else if (IsDelete2(order))
		{
			if (!NoNameRepetition(v[2]))
			{
				if (HasAdmission(CurrentUser->ID, v[2], "DeleteAdmission"))
				{
					unsigned int num = TableList.size();
					unsigned int j = 0;
					for (unsigned int i = 0; i < num; i++)
					{
						if (v[2] == TableList[i]->GetName())
						{
							j = i;
							break;
						}
					}
					vector<vector<string>>().swap(TableList[j]->GetVec());
					TableList[j]->Synchronize();
				}
				else
					cout << "你没有表" << v[2] << "的Delete权限" << endl;
			}
			else
				cout << "名为" << v[2] << "的Table不存在!" << endl;
		}
		else
			cout << "命令格式错误!" << endl;
		}
		else if (v[0] == "SELECT")
		{
			if (IsSelect1(order))
			{
				if (!NoNameRepetition(v[v.size() - 1]))
				{
					if (HasAdmission(CurrentUser->ID, v[v.size() - 1], "SelectAdmission"))
					{
						if (LegalSelectColumn(order,1))
						{
							vector<string> tmp;
							unsigned int i = 1;
							for (; v[i + 1] != "FROM"; i++)
							{
								string s = v[i];
								s.pop_back();
								tmp.push_back(s);
							}
							tmp.push_back(v[i]);
							unsigned int index=0;//记录下名为v[v.size()-1]的表在TableList中的位置
							unsigned int size = TableList.size();
							for (unsigned int i = 0; i < size; i++)
							{
								if (TableList[i]->GetName() == v[v.size() - 1])
								{
									index = i;
									break;
								}
							}
							//下面判断列名是否都存在
							unsigned int size0 = tmp.size();
							vector<string>& CN = TableList[index]->GetColumnName();
							bool flag1 = true;
							vector<int> vec_index;
							for (unsigned int i = 0; i < size0; i++)
							{
								bool flag = false;//找到了置为true
								unsigned int j = 0;
								for (; j < CN.size(); j++)
								{
									if (CN[j] == tmp[i])
									{
										flag = true;
										break;
									}
								}
								if (!flag)
								{
									cout << "列名" << tmp[i] << "不存在!" << endl;
									flag1 = false;
								}
								else
									vec_index.push_back(j);
							}
							if (flag1)//每列都存在
							{
								vector<vector<string>>& sss = TableList[index]->GetVec();
								unsigned int num = sss.size();
								unsigned int num0 = vec_index.size();
								cout << setw(15) << "ID";
								for (unsigned int i = 0; i < size0; i++)
									cout << setw(15) << tmp[i];
								cout << endl;
								for (unsigned int i = 0; i < num; i++)
								{
									cout << setw(15) << i;
									for (unsigned int j = 0; j < num0; j++)
										cout << setw(15) << sss[i][j];
									cout << endl;
								}
							}
						}
						else
							cout << "命令格式错误!" << endl;
					}
					else
						cout << "你没有表" << v[v.size()-1] << "的Select权限" << endl;
				}
				else
					cout << "名为" << v[v.size()-1] << "的Table不存在!" << endl;
			}
			else if (IsSelect2(order))
			{
				if (!NoNameRepetition(v[v.size() - 1]))
				{
					if (HasAdmission(CurrentUser->ID, v[v.size() - 1], "SelectAdmission"))
					{
						unsigned int index = 0;//记录下名为v[v.size()-1]的表在TableList中的位置
						unsigned int size = TableList.size();
						for (unsigned int i = 0; i < size; i++)
						{
							if (TableList[i]->GetName() == v[v.size() - 1])
							{
								index = i;
							    break;
							}
						}
						vector<vector<string>> &sss = TableList[index]->GetVec();
						unsigned int num = sss.size();
						cout << setw(15) << "ID" ;
						for (unsigned int i = 0; i < TableList[index]->GetColumnName().size(); i++)
							cout << setw(15) << TableList[index]->GetColumnName()[i];
						cout << endl;
						unsigned int num0 = TableList[index]->GetColumnName().size();
						for (unsigned int i = 0; i < num; i++)
						{
							cout << setw(15) << i;
							for (unsigned int j = 0; j < num0; j++)
								cout << setw(15) << sss[i][j];
							cout << endl;
						}
					}
					else
						cout << "你没有表" << v[v.size() - 1] << "的Select权限" << endl;
				}
				else
					cout << "名为" << v[v.size()-1] << "的Table不存在!" << endl;
			}
			else if (IsSelect3(order))
			{
				if (!NoNameRepetition(v[v.size() - 1]))
				{
					if (HasAdmission(CurrentUser->ID, v[v.size() - 1], "SelectAdmission"))
					{
						if (LegalSelectColumn(order, 2))
						{
							vector<string> tmp;
							unsigned int i = 1;
							for (; v[i + 1] != "FROM"; i++)
							{
								string s = v[i];
								s.pop_back();
								tmp.push_back(s);
							}
							tmp.push_back(v[i]);
							unsigned int index = FindIndexInTableList(v[v.size() - 1]);
							unsigned int size0 = tmp.size();
							vector<string>& CN = TableList[index]->GetColumnName();
							vector<vector<string>>& sss = TableList[index]->GetVec();
							vector<int> vec_index;
							bool flag1 = true;
							for (unsigned int i = 0; i < size0; i++)
							{
								bool flag = false;//找到了置为true
								unsigned int j = 0;
								for (; j < CN.size(); j++)
								{
									if (CN[j] == tmp[i])
									{
										flag = true;
										break;
									}
								}
								if (!flag)
								{
									cout << "列名" << tmp[i] << "不存在!" << endl;
									flag1 = false;
								}
								else
									vec_index.push_back(j);
							}
							if (flag1)
							{
								for (unsigned int i = 0; i < tmp.size(); i++)
									cout << setw(15) << tmp[i];
								cout << endl;
								unsigned int max = 0;
								vector<vector<string>> output;
								for (unsigned int i = 0; i < vec_index.size(); i++)
								{
									vector<string> vs;
									for (unsigned int j = 0; j < sss.size(); j++)
									{
										bool flag = true;
										for (unsigned int k = 0; k < vs.size(); k++)
										{
											if (sss[vec_index[i]][j] == vs[k])
											{
												flag = false;
												break;
											}
										}
										if (flag)
											vs.push_back(sss[vec_index[i]][j]);
									}
									if (vs.size() > max)
										max = vs.size();
									output.push_back(vs);
								}
								for (unsigned int i = 0; i < max; i++)
								{
									for (unsigned int j = 0; j < vec_index.size(); j++)
									{
										if (output[j].size() <= i + 1)
											cout << setw(15) << output[j][i];
										else
											cout << setw(15) << "";
									}
									cout << endl;
								}
							}
						}
						else
							cout << "命令格式错误!" << endl;
					}
					else
						 cout << "你没有表" << v[v.size() - 1] << "的Select权限" << endl;
				}
				else
					cout << "名为" << v[v.size() - 1] << "的Table不存在!" << endl;
			}
			else if (IsSelect4(order))
			{
				if (!NoNameRepetition(v[3]))
				{
					if (HasAdmission(CurrentUser->ID, v[3], "SelectAdmission"))
					{
						if (LegalSelectColumn2(order, 6))
						{
							;
						}
						else
							cout << "命令格式错误!" << endl;
					}
					else
						cout << "你没有表" << v[v.size() - 1] << "的Select权限" << endl;
				}
				else
					cout << "名为" << v[v.size() - 1] << "的Table不存在!" << endl;
			}
			else if (IsSelect5(order))
			{
			if (!NoNameRepetition(v[v.size()-5]))
			{
				if (HasAdmission(CurrentUser->ID, v[v.size()-5], "SelectAdmission"))
				{
					if (LegalSelectColumn(order, 1))
					{
						vector<string> tmp;
						unsigned int i = 1;
						for (; v[i + 1] != "FROM"; i++)
						{
							string s = v[i];
							s.pop_back();
							tmp.push_back(s);
						}
						tmp.push_back(v[i]);
						unsigned int index = FindIndexInTableList(v[v.size() - 5]);
						unsigned int special_index = 0;
						unsigned int size0 = tmp.size();
						vector<string>& CN = TableList[index]->GetColumnName();
						vector<vector<string>>& sss = TableList[index]->GetVec();
						vector<int> vec_index;
						for (unsigned int i = 0; i < CN.size(); i++)
						{
							if (CN[i] == v[v.size() - 3])
							{
								special_index = i;
								break;
							}
						}
						bool flag1 = true;
						for (unsigned int i = 0; i < size0; i++)
						{
							bool flag = false;//找到了置为true
							unsigned int j = 0;
							for (; j < CN.size(); j++)
							{
								if (CN[j] == tmp[i])
								{
									flag = true;
									break;
								}
							}
							if (!flag)
							{
								cout << "列名" << tmp[i] << "不存在!" << endl;
								flag1 = false;
							}
							else
								vec_index.push_back(j);
						}
						if (flag1)
						{
							cout << setw(15) << "ID";
							for (unsigned int i = 0; i < tmp.size(); i++)
								cout << setw(15) << tmp[i];
							cout << endl;
							for (unsigned int i = 0; i < sss.size(); i++)
							{
								if (sss[i][special_index] == v[v.size() - 1])
								{
									cout << setw(15) << i + 1;
									for (unsigned int j = 0; j < vec_index.size(); j++)
										cout << setw(15) << sss[i][vec_index[j]];
									cout << endl;
								}
							}
						}
					}
					else
						cout << "命令格式错误!" << endl;
				}
				else
					cout << "你没有表" << v[v.size() - 1] << "的Select权限" << endl;
			}
			else
				cout << "名为" << v[v.size() - 1] << "的Table不存在!" << endl;
			}
			else if (IsSelect6(order))
			{
			if (!NoNameRepetition(v[v.size()-3]))
			{
				if (HasAdmission(CurrentUser->ID, v[v.size()-3], "SelectAdmission"))
				{
					if (LegalSelectColumn(order, 1))
					{
						vector<string> tmp;
						unsigned int i = 1;
						for (; v[i + 1] != "FROM"; i++)
						{
							string s = v[i];
							s.pop_back();
							tmp.push_back(s);
						}
						tmp.push_back(v[i]);
						unsigned int index = FindIndexInTableList(v[v.size() - 3]);
						unsigned int size0 = tmp.size();
						vector<string>& CN = TableList[index]->GetColumnName();
						vector<vector<string>>& sss = TableList[index]->GetVec();
						vector<int> vec_index;
						bool flag1 = true;
						for (unsigned int i = 0; i < size0; i++)
						{
							bool flag = false;//找到了置为true
							unsigned int j = 0;
							for (; j < CN.size(); j++)
							{
								if (CN[j] == tmp[i])
								{
									flag = true;
									break;
								}
							}
							if (!flag)
							{
								cout << "列名" << tmp[i] << "不存在!" << endl;
								flag1 = false;
							}
							else
								vec_index.push_back(j);
						}
						if (flag1)
						{
							string order1 = "table/" + v[v.size() - 3] + "_output.txt";
							ofstream fout(order1.c_str());
							cout << setw(15) << "ID";
							for (unsigned int i = 0; i < tmp.size(); i++)
							{
								cout << setw(15) << tmp[i];
								fout << tmp[i] << " ";
							}
							cout << endl;
							fout << endl;
							for (unsigned int i = 0; i < sss.size(); i++)
							{
									cout << setw(15) << i + 1;
									for (unsigned int j = 0; j < vec_index.size(); j++)
									{
										cout << setw(15) << sss[i][vec_index[j]];
										fout << sss[i][vec_index[j]] << " ";
									}
									cout << endl;
									fout << endl;
							}
							fout.close();
						}
					}
					else
						cout << "命令格式错误!" << endl;
				}
				else
					cout << "你没有表" << v[v.size() - 1] << "的Select权限" << endl;
			}
			else
				cout << "名为" << v[v.size() - 1] << "的Table不存在!" << endl;
			}
			else
				cout << "命令格式错误!" << endl;
		}
		else if (v[0] == "GRANT")
		{
		if (IsGrant(order))
		{
			string admissionname;
			if (v[1] == "DROP")admissionname = "DropAdmission";
			else if (v[1] == "DELETE")admissionname = "DeleteAdmission";
			else if (v[1] == "INSERT")admissionname = "InsertAdmission";
			else if (v[1] == "SELECT")admissionname = "SelectAdmission";
			else if (v[1] == "AVAILABLE")admissionname = "AvailableAdmission";
			if (HasAdmission(CurrentUser->ID, v[v.size() - 3], admissionname))
			{
				if (v[v.size() - 1] != "public")
				{
					if (IsAdmissionWay(v[v.size() - 1], CurrentUser->ID, v[v.size() - 3], admissionname))
					{
						unsigned int index = FindIndexInTableList(v[v.size() - 3]);
						vector<Admission*> &Ad = TableList[index]->GetAdmission();
						Admission* p = new Admission;
						p->AdmissionName = admissionname;
						p->admit = CurrentUser->ID;
						p->admitted = v[v.size() - 1];
						Ad.push_back(p);
						AdmissionSynchronize(v[v.size() - 3]);
					}
					else
						cout << "给" << v[v.size() - 1] << " 授予" << admissionname << "权限失败，失败原因:权限表成环!" << endl;
				}
				else
				{
					for (unsigned int i = 0; i < GetUserList().size(); i++)
					{
						if (IsAdmissionWay(GetUserList()[i]->ID, CurrentUser->ID, v[v.size() - 3], admissionname))
						{
							unsigned int index = FindIndexInTableList(v[v.size() - 3]);
							vector<Admission*> &Ad = TableList[index]->GetAdmission();
							Admission* p = new Admission;
							p->AdmissionName = admissionname;
							p->admit = CurrentUser->ID;
							p->admitted = GetUserList()[i]->ID;
							Ad.push_back(p);
							AdmissionSynchronize(v[v.size() - 3]);
						}
						else
							cout << "给" << v[v.size() - 1] << " 授予" << admissionname << "权限失败，失败原因:权限表成环!" << endl;
					}
				}
			}
			else
				cout << "你没有表" << v[v.size() - 3] << "的Select权限" << endl;
		}
		else
			cout << "命令格式错误!" << endl;
		}
		else if (v[0] == "REVOKE")
		{
			if (IsRevoke(order))
			{
				string admissionname;
				if (v[1] == "DROP")admissionname = "DropAdmission";
				else if (v[1] == "DELETE")admissionname = "DeleteAdmission";
				else if (v[1] == "INSERT")admissionname = "InsertAdmission";
				else if (v[1] == "SELECT")admissionname = "SelectAdmission";
				else if (v[1] == "AVAILABLE")admissionname = "AvailableAdmission";
				RevokeAdmission(CurrentUser->ID, v[v.size() - 1], v[v.size() - 3], admissionname);
				AdmissionSynchronize(v[v.size() - 3]);
			}
			else
				cout << "命令格式错误!"<<endl;
		}
		else if(v.size()==1&&v[0]=="quit")
			return;
	}
}

