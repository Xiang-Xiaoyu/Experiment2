#include"SQL.h"


void Legal::LegalOrder(string order,vector<string> &v)
{
	if (v[0] == "CREATE")
	{
		if (v.size() == 5)
		{
			if (IsCreate1(order))
			{
				if (!sql.IsTableName(v[2]))
				{
					if (sql.IsTableFileName(v[4]))
					{
						unsigned int index = sql.FindTableIndexByFileName(v[4]);
						if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, "AVAILABLE"))
						{
							string new_file_name = v[4].substr(0, v[4].length() - 4);
							new_file_name += "_copy.txt";
							sql.TableList[index]->loading();
							Table* newt = new Table(v[2], new_file_name, sql.CurrentUser->ID, *sql.TableList[index]);
							newt->Synchronize();
							newt->PowerSynchronize();
							sql.TableList.push_back(newt);
							sql.TableListSynchronize();
							return;
						}
						else
						{
							char f;
							cout << "You have no \" AVAILABLE \", Do you apply now?(y/n)" << endl;
							do {
								cin >> f;
								if (f == 'y' || f == 'Y')
								{
									Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "AVAILABLE");
									break;
								}
								else if (f == 'n' || f == 'N')
									break;
							} while (1);
						}
					}
					else
						cout << "table file " << v[4] << " does't exist!" << endl;
				}
				else
					cout << "table " << v[2] << " has existed!" << endl;
				return;
			}
			cout << "Command format erro!" << endl;
		}
		else if (v.size() >= 6)
		{
			if (IsCreate2(order))
			{
				if (!sql.IsTableName(v[2]))
				{
					vector<string> bracket;
					ExtractBracketFirstContent(order, bracket);
					Table* newt = new Table(v[2], v[v.size() - 1], sql.CurrentUser->ID);
					vector<vector<string> > v;
					int tmp[POWER_NAME_NUM][MAX_USER_NUM][MAX_USER_NUM] = { 0 };
					newt->loading(bracket, v, tmp);
					newt->Synchronize();
					newt->PowerSynchronize();
					sql.TableList.push_back(newt);
					sql.TableListSynchronize();
					return;
				}
				else
					cout << "table " << v[2] << " has existed!" << endl;
			}
			else
				cout << "Command format erro!" << endl;
		}
	}
	else if (v[0] == "DROP")
	{
		if (v.size() == 3)
		{
			if (v[1] == "TABLE")
			{
				if (sql.IsTableName(v[2]))
				{
					int index = sql.FindTableIndex(v[2]);
					sql.TableList[index]->loading();
					if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, "DROP"))
					{
						sql.DropTable(v[2]);
						cout << "Successfully DROP table " << v[2] << endl;
					}
					else
					{
						char f;
						cout << "You have no \" DROP \", Do you apply now?(y/n)" << endl;
						do {
							cin >> f;
							if (f == 'y' || f == 'Y')
							{
								Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "DROP");
								break;
							}
							else if (f == 'n' || f == 'N')
								break;
						} while (1);
					}
				}
				else
					cout << "There is no " << v[2] << " table!" << endl;
			}
			else
				cout << "Command format erro!" << endl;
		}
		else
			cout << "Command format erro!" << endl;
	}
	else if (v[0] == "TABLE")
	{
		if (v.size() == 2)
		{
			if (v[1] == "LIST")
			{
				unsigned int num = sql.TableList.size();
				cout << "    total:" << num << endl;
				for (unsigned int i = 0; i < num; i++)
				{
					sql.TableList[i]->loading();
					cout << sql.TableList[i]->name << ":";
					if (sql.TableList[i]->HasPower(sql.CurrentUser->ID, "AVAILABLE"))
					{
						cout << "(" << sql.TableList[i]->column << "," << sql.TableList[i]->row << ")";
						cout << " [";
						unsigned int j = 0;
						vector<string>& v = sql.TableList[i]->ColumnName;
						for (; j < v.size() - 1; j++)
							cout << v[j] << ", ";
						cout << v[j] << "] ";
						if (sql.TableList[i]->HasPower(sql.CurrentUser->ID, "DROP"))
							cout << "Drop, ";
						if (sql.TableList[i]->HasPower(sql.CurrentUser->ID, "INSERT"))
							cout << "Insert, ";
						if (sql.TableList[i]->HasPower(sql.CurrentUser->ID, "DELETE"))
							cout << "Delete, ";
						if (sql.TableList[i]->HasPower(sql.CurrentUser->ID, "SELECT"))
							cout << "Select, ";
					}
					if (sql.TableList[i]->OwnerName == sql.CurrentUser->ID)
						cout << "[OWNER]" << endl;
					else
						cout << "[SERVICE]" << endl;
				}
			}
			else
				cout << "Command format erro!" << endl;
		}
		else
			cout << "Command format erro!" << endl;
	}
	else if (v[0] == "INSERT")
	{
		if (v.size() >= 5)
		{
			if (IsInsert1(order))
			{
				if(sql.IsTableName(v[2]))
				{
					vector<string> bracket1;
					ExtractBracketFirstContent(order, bracket1);
					int index = sql.FindTableIndex(v[2]);
					sql.TableList[index]->loading();
					if (sql.TableList[index]->HasPower(sql.CurrentUser->ID,"INSERT"))
					{
						vector<string> newv;
						sql.TableList[index]->row++;
						newv.assign(bracket1.begin(), bracket1.end());
						sql.TableList[index]->vec.push_back(newv);
						sql.TableList[index]->Synchronize();
						cout << "Successfully Insert!" << endl;
						return;
					}
					else
					{
						char f;
						cout << "You have no \" INSERT \", Do you apply now?(y/n)" << endl;
						do {
							cin >> f;
							if (f == 'y' || f == 'Y')
							{
								Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "INSERT");
								break;
							}
							else if (f == 'n' || f == 'N')
							break;
						} while (1);
					}
					}
				else
					cout << "table " << v[2] << " doesn't exist!" << endl;
			}
			else if (IsInsert2(order))
			{
				if (sql.IsTableName(v[2]))
				{
					int index = sql.FindTableIndex(v[2]);
					sql.TableList[index]->loading();
					vector<string> bracket1, bracket2;
					ExtractBracketFirstContent(order, bracket1);
					ExtractBracketSecondContent(order, bracket2);
					if (bracket1.size() < sql.TableList[index]->ColumnName.size())
					{
						if (OneBelongOne(bracket1, sql.TableList[index]->ColumnName))
						{
							if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, "INSERT"))
							{
								vector<int> vec_int;
								vector<string> newv;
								sql.TableList[index]->row++;
								ExtractVecInt(vec_int, bracket1, sql.TableList[index]->ColumnName);
								unsigned int vec_int_size = vec_int.size();
								unsigned int Column_size = sql.TableList[index]->ColumnName.size();
								for (unsigned int i = 0, j = 0; i < Column_size; ++i)
								{
									vector<int>::iterator it = vec_int.begin();
									it = find(vec_int.begin(), vec_int.end(), i);
									if (it != vec_int.end())
										newv.push_back(bracket2[j++]);
									else
										newv.push_back("null");
								}
								sql.TableList[index]->vec.push_back(newv);
								sql.TableList[index]->Synchronize();
								cout << "Successfully Insert!" << endl;
							}
							else
							{
								char f;
								cout << "You have no \" INSERT \", Do you apply now?(y/n)" << endl;
								do {
									cin >> f;
									if (f == 'y' || f == 'Y')
									{
										Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "INSERT");
										break;
									}
									else if (f == 'n' || f == 'N')
										break;
								} while (1);
							}
						}
						else
							cout << "Command format erro!" << endl;
					}
					else
						cout << "Command format erro!" << endl;
				}
				else
					cout << "table " << v[2] << " doesn't exist!" << endl;
			}
			else
				cout << "Command format erro!" << endl;
		}
		else
			cout << "Command format erro!" << endl;
	}
	else if (v[0] == "DELETE")
	{
		if (IsDelete1(order))
		{
			if (sql.IsTableName(v[2]))
			{
				unsigned int index = sql.FindTableIndex(v[2]),tmp_index=0;
				sql.TableList[index]->loading();
				unsigned int size = sql.TableList[index]->ColumnName.size();
				vector<string> tmp;
				tmp.push_back(v[4]);
				if (OneBelongOne(tmp, sql.TableList[index]->ColumnName))
				{
					if (sql.TableList[index]->HasPower(sql.CurrentUser->ID,"DELETE"))
					{
						for (unsigned int i = 0; i < size; ++i)
							if (v[4] == sql.TableList[index]->ColumnName[i])
							{
								tmp_index = i;
								break;
							}
						for (int i = 0; i < sql.TableList[index]->vec.size(); ++i)
							if (sql.TableList[index]->vec[i][tmp_index] == v[6])
							{
								sql.TableList[index]->row--;
								sql.TableList[index]->vec.erase(i + sql.TableList[index]->vec.begin());
								i--;
							}
						sql.TableList[index]->Synchronize();
						cout << "Successfully Delete!" << endl;
					}
					else
					{
						char f;
						cout << "You have no \" DELETE \", Do you apply now?(y/n)" << endl;
						do {
							cin >> f;
							if (f == 'y' || f == 'Y')
							{
								Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "DELETE");
								break;
							}
							else if (f == 'n' || f == 'N')
								break;
						} while (1);
					}
				}
				else
					return;
			}
			else
				cout << "table " << v[2] << " doesn't exist!" << endl;
		}
		else if (IsDelete2(order))
		{
			if (sql.IsTableName(v[3]))
			{
				unsigned int index = sql.FindTableIndex(v[3]);
				sql.TableList[index]->loading();
				if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, "DELETE"))
				{
					vector<vector<string> >().swap(sql.TableList[index]->vec);
					sql.TableList[index]->vec.clear();
					sql.TableList[index]->Synchronize();
					sql.TableList[index]->row=0;
					cout << "Successfully Delete!" << endl;
				}
				else
				{
					char f;
					cout << "You have no \" DELETE \", Do you apply now?(y/n)" << endl;
					do {
						cin >> f;
						if (f == 'y' || f == 'Y')
						{
							Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "DELETE");
							break;
						}
						else if (f == 'n' || f == 'N')
							break;
					} while (1);
				}
			}
			else
				cout << "table " << v[2] << " doesn't exist!" << endl;
		}
		else
			cout << "Command format erro!" << endl;
	}
	else if (v[0] == "SELECT")
	{
		if (IsSelect1(order))
		{
			if (sql.IsTableName(v[v.size() - 1]))
			{
				int index = sql.FindTableIndex(v[v.size() - 1]);
				sql.TableList[index]->loading();
				vector<string> bracket1;
				ExtractBracketFirstContent(order, bracket1);
				if (OneBelongOne(bracket1, sql.TableList[index]->ColumnName))
				{
					if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, "SELECT"))
					{
						vector<int> vec_int;
						ExtractVecInt(vec_int, bracket1, sql.TableList[index]->ColumnName);
						unsigned int size = sql.TableList[index]->vec.size();
						cout << setw(20) << "ID";
						for (unsigned int i = 0; i < bracket1.size(); i++)
							cout << setw(20) << bracket1[i];
						cout << endl;
						for (unsigned int i = 0; i < size; i++)
						{
							cout << setw(20) << i;
							for (unsigned int j = 0; j < vec_int.size(); j++)
								cout << setw(20) << sql.TableList[index]->vec[i][vec_int[j]];
							cout << endl;
						}
						cout << "Successfully Select!" << endl;
					}
					else
					{
						char f;
						cout << "You have no \" SELECT \", Do you apply now?(y/n)" << endl;
						do {
							cin >> f;
							if (f == 'y' || f == 'Y')
							{
								Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "SELECT");
								break;
							}
							else if (f == 'n' || f == 'N')
								break;
						} while (1);
					}
				}
				else
					cout << "Command format erro!" << endl;
			}
			else
				cout << "table " << v[v.size()-1] << " doesn't exist!" << endl;
		}
		else if (IsSelect2(order))
		{
			if (sql.IsTableName(v[3]))
			{
				int index = sql.FindTableIndex(v[v.size() - 1]);
				sql.TableList[index]->loading();
				if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, "SELECT"))
				{
					unsigned int size = sql.TableList[index]->vec.size();
					cout << setw(20) << "ID";
					for (unsigned int i = 0; i < sql.TableList[index]->ColumnName.size(); i++)
						cout << setw(20) << sql.TableList[index]->ColumnName[i];
					cout << endl;
					for (unsigned int i = 0; i < sql.TableList[index]->vec.size(); i++)
					{
						cout << setw(20) << i;
						for (unsigned int j = 0; j < sql.TableList[index]->ColumnName.size(); j++)
							cout << setw(20) << sql.TableList[index]->vec[i][j];
							cout << endl;
					}
					cout << "Successfully Select!" << endl;
				}
				else
				{
					char f;
					cout << "You have no \" SELECT \", Do you apply now?(y/n)" << endl;
					do {
						cin >> f;
						if (f == 'y' || f == 'Y')
						{
							Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "SELECT");
							break;
						}
						else if (f == 'n' || f == 'N')
							break;
					} while (1);
				}
			}
			else
				cout << "table " << v[v.size() - 1] << " doesn't exist!" << endl;
		}
		else if (IsSelect3(order))
		{
			if (sql.IsTableName(v[v.size() - 1]))
			{
				int index = sql.FindTableIndex(v[v.size() - 1]);
				sql.TableList[index]->loading();
				vector<string> vec_string;
				ExtractVecString(order, "DISTINCT", "FROM", vec_string);
				if (OneBelongOne(vec_string, sql.TableList[index]->ColumnName))
				{
					vector<int> vec_int;
					ExtractVecInt(vec_int, vec_string, sql.TableList[index]->ColumnName);
					if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, "SELECT"))
					{
						for (unsigned int i = 0; i < vec_string.size(); i++)
							cout << setw(20) << vec_string[i];
						cout << endl;
						vector<vector<string> > new_vec;
						for (unsigned int i = 0; i < vec_int.size(); i++)
						{
							vector<string> column_vec;
							for (unsigned int j = 0; j < sql.TableList[index]->vec.size(); j++)
							{
								bool flag = false;
								for (unsigned int k = 0; k < column_vec.size(); k++)
								{
									if (column_vec[k] == sql.TableList[index]->vec[j][vec_int[i]])
									{
										flag = true;
										break;
									}
								}
								if (!flag)
									column_vec.push_back(sql.TableList[index]->vec[j][vec_int[i]]);
								else
									column_vec.push_back("null");
							}
							new_vec.push_back(column_vec);
							vector<string>().swap(column_vec);
						}
						for (unsigned int i = 0; i < sql.TableList[index]->vec.size(); i++)
						{
							for (unsigned int j = 0; j < new_vec.size(); j++)
							{
								if (new_vec[j][i] == "null")
									cout << setw(20) << "";
								else
									cout << setw(20) << new_vec[j][i];
							}
							cout << endl;
						}
						cout << "Successfully Select!" << endl;
					}
					else
					{
						char f;
						cout << "You have no \" SELECT \", Do you apply now?(y/n)" << endl;
						do {
							cin >> f;
							if (f == 'y' || f == 'Y')
							{
								Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "SELECT");
								break;
							}
							else if (f == 'n' || f == 'N')
								break;
						} while (1);
					}
				}
				else
					cout << "Command format erro!" << endl;
			}
			else
				cout << "table " << v[v.size() - 1] << " doesn't exist!" << endl;
		}
		else if (IsSelect4(order))
		{
			if (sql.IsTableName(v[3]))
			{
				int index = sql.FindTableIndex(v[3]);
				sql.TableList[index]->loading();
				vector<string> vec_string;
				vector<int> vec_int;
				if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, "SELECT"))
				{
					if (v[v.size() - 1] == "ASC")
					{
						ExtractVecString(order, "BY", "ASC", vec_string);
						if (OneBelongOne(vec_string, sql.TableList[index]->ColumnName))
						{
							ExtractVecInt(vec_int, vec_string, sql.TableList[index]->ColumnName);
							/*loading(vec_int);*/
							cout << setw(20) << "ID";
							for (unsigned int i = 0; i < vec_string.size(); i++)
								cout << setw(20) << vec_string[i];
							cout << endl;
							/*sql.TableList[index]->ASCSortOutput();*/
							cout << "Successfully Select!" << endl;
						}
						else
							cout << "Command format erro!" << endl;
					}
					else if (v[v.size() - 1] == "DESC")
					{
						ExtractVecString(order, "BY", "DESC", vec_string);
						if (OneBelongOne(vec_string, sql.TableList[index]->ColumnName))
						{
							ExtractVecInt(vec_int, vec_string, sql.TableList[index]->ColumnName);
							/*loading(vec_int);*/
							cout << setw(20) << "ID";
							for (unsigned int i = 0; i < vec_string.size(); i++)
								cout << setw(20) << vec_string[i];
							cout << endl;
							/*sql.TableList[index]->DESCSortOutput();*/
							cout << "Successfully Select!" << endl;
						}
						else
							cout << "Command format erro!" << endl;
					}
					else
						cout << "Command format erro!" << endl;
				}
				else
				{
					char f;
					cout << "You have no \" SELECT \", Do you apply now?(y/n)" << endl;
					do {
						cin >> f;
						if (f == 'y' || f == 'Y')
						{
							Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "SELECT");
							break;
						}
						else if (f == 'n' || f == 'N')
							break;
					} while (1);
				}
			}
			else
				cout << "table " << v[3] << " doesn't exist!" << endl;
		}
		else if (IsSelect5(order))
		{
			if (sql.IsTableName(v[v.size() - 5]))
			{
				int index = sql.FindTableIndex(v[v.size() - 5]);
				sql.TableList[index]->loading();
				if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, "SELECT"))
				{
					vector<vector<string> > new_vec;
					int index1 = -1;//找出关键字的下标
					for (unsigned int i = 0; i < sql.TableList[index]->ColumnName.size(); i++)
					{
						if (v[v.size() - 3] == sql.TableList[index]->ColumnName[i])
						{
							index1 = i;
							break;
						}
					}
					if (index1 != -1)
					{
						vector<string> vec_string;
						vector<int> vec_int;
						ExtractVecString(order, "SELECT", "FROM", vec_string);
						if (OneBelongOne(vec_string, sql.TableList[index]->ColumnName))
						{
							ExtractVecInt(vec_int, vec_string, sql.TableList[index]->ColumnName);
							for (unsigned int i = 0; i < vec_string.size(); ++i)
								cout << setw(20) << vec_string[i];
							cout << endl;
							for (unsigned int i = 0; i < sql.TableList[index]->vec.size(); i++)
							{
								if (sql.TableList[index]->vec[i][index1] == v[v.size() - 1])
								{
									for (unsigned int j = 0; j < vec_int.size(); j++)
										cout << setw(20) << sql.TableList[index]->vec[i][vec_int[j]];
									cout << endl;
								}
							}
							cout << "Successfully Select!" << endl;
						}
						else
							cout << "Command format erro!" << endl;
					}
					else
						cout << "Command format erro!" << endl;
				}
				else
				{
					char f;
					cout << "You have no \" SELECT \", Do you apply now?(y/n)" << endl;
					do {
						cin >> f;
						if (f == 'y' || f == 'Y')
						{
							Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "SELECT");
							break;
						}
						else if (f == 'n' || f == 'N')
							break;
					} while (1);
				}
			}
			else
				cout << "table " << v[3] << " doesn't exist!" << endl;
		}
		else if (IsSelect6(order))
		{
			if (sql.IsTableName(v[v.size()-3]))
			{
				if (!sql.IsTableFileName(v[v.size()-1]))
				{
					int index = sql.FindTableIndex(v[v.size() - 3]);
					sql.TableList[index]->loading();
					vector<string> vec_string;
					vector<int> vec_int;
					ExtractVecString(order, "SELECT", "FROM", vec_string);
					if (OneBelongOne(vec_string,sql.TableList[index]->ColumnName))
					{
						ExtractVecInt(vec_int, vec_string, sql.TableList[index]->ColumnName);
						if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, "SELECT"))
						{
							vector<vector<string> > new_vec;
							for (unsigned int i = 0; i < sql.TableList[index]->vec.size(); i++)
							{
								vector<string> new_row;
								for (unsigned int j = 0; j < vec_int.size(); j++)
									new_row.push_back(sql.TableList[index]->vec[i][vec_int[j]]);
								new_vec.push_back(new_row);
								vector<string>().swap(new_row);
								new_row.clear();
							}
							Table *newt = new Table(sql.TableList[index]->name + "_copy1", v[v.size() - 1], sql.CurrentUser->ID);
							int a[POWER_NAME_NUM][MAX_USER_NUM][MAX_USER_NUM] = { 0 };
							newt->loading(vec_string, new_vec, a);
							newt->Synchronize();
							newt->PowerSynchronize();
							sql.TableList.push_back(newt);
							sql.TableListSynchronize();
							cout << "Successfully Select!" << endl;
						}
						else
						{
							char f;
							cout << "You have no \" SELECT \", Do you apply now?(y/n)" << endl;
							do {
								cin >> f;
								if (f == 'y' || f == 'Y')
								{
									Apply(sql.CurrentUser->ID, sql.TableList[index]->name, "SELECT");
									break;
								}
								else if (f == 'n' || f == 'N')
									break;
							} while (1);
						}
					}
					else
						cout << "Command format erro!" << endl;
				}
				else
					cout << "table file name " << v[v.size()-1] << " repeats!" << endl;
			}
			else
				cout << "table " << v[v.size()-3] << " doesn't exist!" << endl;
		}
		else 
			cout << "Command format erro!" << endl;
	}
	else if (v[0] == "GRANT")
	{
		if (IsGrant(order))
		{
			vector<string> power_vec;
			vector<string> table_vec;
			vector<string> user_vec;
			ExtractVecString(order, "GRANT", "ON", power_vec);
			ExtractVecString(order, "ON", "TO", table_vec);
			ExtractVecString(order, "TO", user_vec);
			for (unsigned int tablenum = 0; tablenum < table_vec.size(); tablenum++)
		{
			if (sql.IsTableName(table_vec[tablenum]))
			{
				int index = sql.FindTableIndex(table_vec[tablenum]);
				sql.TableList[index]->LoadingPowerFromFile();
				for (unsigned int powernum = 0; powernum < power_vec.size(); powernum++)
				{
					if (MapPowerNameToInt(power_vec[powernum]) != -1)
					{
						if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, power_vec[powernum]))
						{
							for (unsigned int usernum = 0; usernum < user_vec.size(); usernum++)
							{
								if (MapUserIdToInt(user_vec[usernum]) != -1)
								{
									if (!HasPowerWayFromTo(user_vec[usernum], sql.CurrentUser->ID, sql.TableList[index]->name, power_vec[powernum]))
									{
										sql.TableList[index]->PowerMatrix[MapPowerNameToInt(power_vec[powernum])][MapUserIdToInt(sql.CurrentUser->ID)][MapUserIdToInt(user_vec[usernum])]++;
										sql.TableList[index]->PowerSynchronize();
									}
									else
										cout << "GRANT FOR A LOOP!" << endl;
								}
								else
									cout << user_vec[usernum] << " is not a user ID!" << endl;
							}
						}
						else
							cout << "You don't have POWER " << power_vec[powernum] << " !" << endl;
					}
					else
						cout << power_vec[powernum] << " is not a power name!" << endl;
				}
			}
			else
				cout << table_vec[tablenum] << " is not a table name!" << endl;
		}
		}
		else
			cout << "Command format erro!" << endl;
	}
	else if (v[0] == "REVOKE")
	{
		if (IsRevoke(order))
		{
			vector<string> power_vec;
			vector<string> table_vec;
			vector<string> user_vec;
			ExtractVecString(order, "REVOKE", "ON", power_vec);
			ExtractVecString(order, "ON", "FROM", table_vec);
			ExtractVecString(order, "FROM", user_vec);
			for (unsigned int tablenum = 0; tablenum < table_vec.size(); tablenum++)
			{
				if (sql.IsTableName(table_vec[tablenum]))
				{
					int index = sql.FindTableIndex(table_vec[tablenum]);
					sql.TableList[index]->LoadingPowerFromFile();
					for (unsigned int powernum = 0; powernum < power_vec.size(); powernum++)
					{
						if (MapPowerNameToInt(power_vec[powernum]) != -1)
						{
							if (sql.TableList[index]->HasPower(sql.CurrentUser->ID, power_vec[powernum]))
							{
								for (unsigned int usernum = 0; usernum < user_vec.size(); usernum++)
								{
									if (MapUserIdToInt(user_vec[usernum]) != -1)
									{
										sql.Revoke(sql.CurrentUser->ID, user_vec[usernum], table_vec[tablenum], power_vec[powernum]);
										sql.TableList[index]->PowerSynchronize();
									}
									else
										cout << user_vec[usernum] << " is not a user ID!" << endl;
								}
							}
							else
								cout << "You don't have POWER " << power_vec[powernum] << " !" << endl;
						}
						else
							cout << power_vec[powernum] << " is not a power name!" << endl;
					}
				}
				else
					cout << table_vec[tablenum] << " is not a table name!" << endl;
			}
		}
		else
			cout << "Command format erro!" << endl;
	}
	else if (v[0] == "APPLY")
	{
		if (IsApply(order))
		{
			if (sql.IsTableName(v[3]))
			{
				int power_index = MapPowerNameToInt(v[1]);
				int user_index = MapUserIdToInt(v[5]);
				if (power_index == -1 || user_index == -1)
				{
					cout << "Command format erro!" << endl;
					return;
				}
				int index = sql.FindTableIndex(v[3]);
				sql.TableList[index]->LoadingPowerFromFile();
				if (HasPowerWayFromTo(sql.CurrentUser->ID, v[5], v[3], v[1]))
				{
					cout << "GRANT FOR A LOOP!" << endl;
					return;
				}
				char f;
				cout <<"You("<<sql.CurrentUser->ID<<") ask"<<v[5]<<" for POWER "<<v[1]<<" Do you apply now?(y/n)" << endl;
				do {
					cin >> f;
					if (f == 'y' || f == 'Y')
					{
						Apply(sql.CurrentUser->ID, sql.TableList[index]->name, v[1]);
						break;
					}
					else if (f == 'n' || f == 'N')
						break;
				} while (1);
			}
			else
				cout << v[3] << " is not a table name!" << endl;
		}
	}
	else
		cout << "Command format erro!" << endl;
	return;
		
}

bool Legal::IsCreate1(string s)
//是不是从文件中创建一个table的格式
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num != 5)
		return false;
	else if (v[0] == "CREATE"&&v[1] == "TABLE"&&v[3] == "FROM"&&IsTxt(v[4]))
		return true;
	else
		return false;
}

bool Legal::IsTxt(string s)
//s是否是一个文件名，不判断其是否存在
{
	unsigned int len = s.length();
	if (len < 5)
		return false;
	if (s[len - 4] == '.'&&s[len - 3] == 't'&&s[len - 2] == 'x'&&s[len - 1] == 't')
		return true;
	else
		return false;
}

bool Legal::IsCreate2(string s)
//是否是创建表的第二个命令，即直接创建
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num < 6)
		return false;
	if (v[0] == "CREATE"&&v[1] == "TABLE"&&v[v.size() - 2] == "TO"&&v[3][0] == '('&&v[v.size() - 3][v[v.size() - 3].length() - 1] == ')'&&BraketNum(s)==1&&LegalFirstBracket(s))
		return true;
	else
		return false;
}

bool Legal::LegalFirstBracket(string s)
//判断第一个括号中的命令格式是否正确
{
	bool flag = true;
	int len = s.length();
	int i = 0;
	int j = 0;
	for (; i < len; i++)
		if (s[i] == '(')break;
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

int Legal::BraketNum(string s)
//返回这条命令中括号的对数
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

bool Legal::LegalSecondBracket(string s)
//判断第二个括号格式是否正确
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

void Legal::ExtractBracketFirstContent(string s, vector<string>& v)
//提取第1个括号中的内容
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

void Legal::ExtractBracketSecondContent(string s, vector<string>& v)
//提取第二个括号的内容
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

bool Legal::OneBelongOne(vector<string>& s1, vector<string>& s2)
//判断s1中的每一项是否属于s2
{
	unsigned int size1 = s1.size();
	unsigned int size2 = s2.size();
	bool f = true;
	for (unsigned int i = 0; i < size1; i++)
	{
		bool flag = false;
		for (unsigned int j = 0; j < size2; j++)
		{
			if (s1[i] == s2[j])
				flag = true;
		}
		if (!flag)
			cout << s1[i] << " is not a column name!" << endl;
		f = f && flag;
	}
	return f;
}

void Legal::Apply(string user, string tablename, string powername)
//user向tablename的表申请powername的权限
{
	int user_index = MapUserIdToInt(user);
	int table_index = sql.FindTableIndex(tablename);
	int power_index = MapPowerNameToInt(powername);
	if (HasPowerWayFromTo(user, sql.TableList[table_index]->OwnerName, tablename, powername))
	{
		cout << "can't empower for a loop!" << endl;
		return;
	}
	string infor;
	cout << "Leave a message:";
	getline(cin, infor);
	while (infor == "")
		getline(cin, infor);
	apply* newa = new apply;
	newa->admit = sql.TableList[table_index]->OwnerName;
	newa->admitted = user;
	newa->TableName = tablename;
	newa->PowerName = powername;
	newa->time = GetSystemTime();
	newa->information = infor;
	sql.ApplyPower.push_back(newa);
	sql.ApplyPowerSynchronize();
	cout << "Successfully apply!" << endl;
}

bool Legal::HasPowerWayFromTo(string user1, string user2, string tablename, string powername)
//从user1到user2是否有一条权限通路，防止循环授权
{
	int user_index1 = MapUserIdToInt(user1);
	int user_index2 = MapUserIdToInt(user2);
	int table_index = sql.FindTableIndex(tablename);
	int power_index = MapPowerNameToInt(powername);
	if (sql.TableList[table_index]->PowerMatrix[power_index][user_index1][user_index2])
		return true;
	for (int i = 0; i < MAX_USER_NUM; i++)
	{
		if (sql.TableList[table_index]->PowerMatrix[power_index][user_index1][i])
			if (RecursionFindWay(user_index1, user_index2, table_index, power_index))
				return true;
	}
	return false;
}

bool Legal::RecursionFindWay(int user_index1, int user_index2, int table_index, int power_index)
//递归完成HasPowerWayFromTo()函数的功能
{
	if (sql.TableList[table_index]->PowerMatrix[power_index][user_index1][user_index2])
		return true;
	for (int i = 0; i < MAX_USER_NUM; i++)
	{
		if (sql.TableList[table_index]->PowerMatrix[power_index][user_index1][i])
			if (RecursionFindWay(user_index1, user_index2, table_index, power_index))
				return true;
	}
	return false;
}

bool Legal::IsInsert1(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num < 5)
		return false;
	if (v[0] == "INSERT"&&v[1] == "INTO"&&v[3] == "VALUES"&&BraketNum(s) == 1 && LegalFirstBracket(s))
		return true;
	else
		return false;
}

bool Legal::IsInsert2(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num < 6)
		return false;
	if (v[0] == "INSERT"&&v[1] == "INTO"&&BraketNum(s) == 2 && LegalFirstBracket(s)&& LegalSecondBracket(s))
		return true;
	else
		return false;
}

void Legal::ExtractVecInt(vector<int>& vec_int, vector<string>& son, vector<string>& dad)
{
	unsigned int son_size = son.size();
	unsigned int dad_size = dad.size();
	for (unsigned int i = 0; i < son_size; ++i)
	{
		for (unsigned int j = 0; j < dad_size; ++j)
		{
			if (son[i] == dad[j])
			{
				vec_int.push_back(j);
				break;
			}
		}
	}
}

bool Legal::IsDelete1(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num !=7)
		return false;
	if (v[0] == "DELETE"&&v[1] == "FROM"&&v[3]=="WHERE" &&v[5]=="=")
		return true;
	else
		return false;
}

bool Legal::IsDelete2(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num != 4)
		return false;
	if (v[0] == "DELETE"&&v[1] == "*"&&v[2] == "FROM")
		return true;
	else
		return false;
}

bool Legal::IsSelect1(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num <4)
		return false;
	if (v[0] == "SELECT"&&v[1][0] == '('&&v[v.size()-3][v[v.size()-3].length()-1]==')'&&v[v.size()-2] == "FROM"&&BraketNum(s)==1&&LegalFirstBracket(s))
		return true;
	else
		return false;
}

bool Legal::IsSelect2(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num != 4)
		return false;
	if (v[0] == "SELECT"&&v[1] == "*"&&v[2] == "FROM")
		return true;
	else
		return false;
}

bool Legal::IsSelect3(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num <5)
		return false;
	if (v[0] == "SELECT"&&v[1] == "DISTINCT"&&v[v.size()-2] == "FROM"&&LegalMidString(s,"DISTINCT","FROM"))
		return true;
	else
		return false;
}

bool Legal::IsSelect4(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num < 8)
		return false;
	if (v[0] == "SELECT"&&v[1] == "*"&&v[2] == "FROM"&&v[4] == "ORDER"&&v[5] == "BY"&&((v[v.size() - 1] == "ASC"&&LegalMidString(s,"BY","ASC"))||(v[v.size()-1]=="DESC"&&LegalMidString(s,"BY","DESC"))))
		return true;
	else
		return false;
}

bool Legal::IsSelect5(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num <8)
		return false;
	if (v[0] == "SELECT"&&LegalMidString(s,"SELECT","FROM")&&v[v.size()-6]=="FROM"&&v[v.size()-4]=="WHERE"&&v[v.size()-2]=="=")
		return true;
	else
		return false;
}

bool Legal::IsSelect6(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num <6)
		return false;
	if (v[0] == "SELECT" &&v[v.size()-4] == "FROM"&&v[v.size()-2]=="TO")
		return true;
	else
		return false;
}

void Legal::ExtractVecString(string order,string begin, string end,vector<string>& vec_string)
{
	string s;
	istringstream ss(order);
	string str;
	vector<string> v;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	vector<string>::iterator it1 = v.begin(),it2=v.begin(),it;
	it1 = find(v.begin(), v.end(), begin);
	it2 = find(v.begin(), v.end(), end);
	for (it = it1 + 1; it != it2; it++)
	{
		if (it != it2 - 1)
		{
			(*it).pop_back();
			vec_string.push_back(*it);
		}
		else
			vec_string.push_back(*it);
	}
}

bool Legal::LegalMidString(string order, string begin, string end)
{
	string s;
	istringstream ss(order);
	string str;
	vector<string> v;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	vector<string>::iterator it1 = v.begin(), it2 = v.begin(), it;
	it1 = find(v.begin(), v.end(), begin);
	if (it1 == v.end())
		return false;
	it2 = find(v.begin(), v.end(), end);
	if (it2 == v.end())
		return false;
	bool flag = true;
	for (it = it1 + 1; it != it2; it++)
	{
		if (it != it2 - 1)
		{
			flag = flag && ((*it)[(*it).length() - 1] == ',');
		}
		else
			flag = flag &&((*it)[(*it).length() - 1] != ',');
	}
	return flag;
}

bool Legal::LegalLastString(string order, string begin)
{
	string s;
	istringstream ss(order);
	string str;
	vector<string> v;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	vector<string>::iterator it0 = v.begin(),it=v.begin();
	it0 = find(v.begin(), v.end(), begin);
	if (it0 == v.end())
		return false;
	bool flag = true;
	for (it = it0 + 1; it != v.end(); it++)
		if (it != v.end() - 1)
			flag = flag && ((*it)[(*it).length() - 1] == ',');
		else
			flag = flag && ((*it)[(*it).length() - 1] != ',');
	return flag;
}

void Legal::ExtractVecString(string order, string begin, vector<string>& vec_string)
{
	string s;
	istringstream ss(order);
	string str;
	vector<string> v;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	vector<string>::iterator it0 = v.begin(), it = v.begin();
	it0 = find(v.begin(), v.end(), begin);
	if (it0 == v.end())
		return;
	for (it = it0 + 1; it != v.end(); it++)
		if (it != v.end() - 1)
		{
			(*it).pop_back();
			vec_string.push_back(*it);
		}
		else
			vec_string.push_back(*it);
}

bool Legal::IsGrant(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num < 6)
		return false;
	if (v[0] == "GRANT"&&LegalMidString(s, "GRANT", "ON") && LegalMidString(s, "ON", "TO") && LegalLastString(s, "TO"))
		return true;
	else
		return false;
}

bool Legal::IsRevoke(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num <6)
		return false;
	if (v[0] == "REVOKE"&&LegalMidString(s, "REVOKE", "ON") && LegalMidString(s, "ON", "FROM") && LegalLastString(s, "FROM"))
		return true;
	else
		return false;
}

bool Legal::IsApply(string s)
{
	vector<string> v;
	istringstream ss(s);
	string str;
	while (ss >> str)
		v.push_back(str);
	ss.clear();
	unsigned int num = v.size();
	if (num != 6)
		return false;
	if (v[0] == "APPLY"&&v[2] == "ON"&&v[4] == "FROM")
		return true;
	else
		return false;
}

