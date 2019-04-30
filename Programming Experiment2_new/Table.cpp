#include"mySQL.h"

Table::Table(string n, string fn, string on, vector<string> CN, vector<vector<string>> v, vector<Admission*> ad)
{
	name = n;
	FileName = fn;
	OwnerName = on;
	CN.swap(ColumnName);
	v.swap(vec);
	ad.swap(admission);
	column = CN.size();
	row = v.size();
}

Table::Table(string n,string fn,string on,const Table & t)
{
	name = n;
	FileName = fn;
	OwnerName = on;
	ColumnName.assign(t.ColumnName.begin(), t.ColumnName.end());
	vec.assign(t.vec.begin(), t.vec.end());
	unsigned int num = t.admission.size();
	for (unsigned int i = 0; i < num; i++)
	{
		Admission* newa = new Admission;
		*newa = *(t.admission[i]);
		admission.push_back(newa);
	}
	row = t.row;
	column = t.column;
}


void Table::Synchronize()
{
	string order = "table/" + FileName;
	ofstream out(order.c_str(), ios::trunc);
	if (!out)
		cout << FileName << " can't be opened!" << endl;
	else
	{
		unsigned int num = ColumnName.size();
		unsigned int i = 0;
		for (; i < num - 1; i++)
			out << ColumnName[i] << " ";
		out << ColumnName[i] <<endl;
		if (empty(vec))
			return;
		else
		{
			unsigned int row = vec.size();
			unsigned int i = 0;
			for (; i < row-1; i++)
			{
				unsigned int j = 0;
				for (; j < num - 1; j++)
					out << vec[i][j] << " ";
				out << vec[i][j] << endl;
			}
			unsigned int j = 0;
			for (; j < num - 1; j++)
				out << vec[i][j] << " ";
			out << vec[i][j];
		}
		out.close();
	}
}