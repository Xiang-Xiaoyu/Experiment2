#include"mySQL.h"

mySQL sql;

int main()
{
	sql.run();
	cout << "Program exits successfully!" << endl;
	system("pause");
	return 0;
}

bool IsAdmissionName(string s)
{
	for (int i = 0; i < 5; i++)
	{
		if (s == AdmissionName[i])
			return true;
	}
	return false;
}

bool cmp1(const Admission* a, const Admission* b)
{
	return a->AdmissionName > b->AdmissionName;
}