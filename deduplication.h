#pragma once
/**************************************************
* @This function is used to delete same data.
* @Input     : several datas path
* @Output    : one dataset with data deduplication
* @Up-to-date: 2023-07-12 17:14
**************************************************/


#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>

using std::vector;
using std::string;
using std::unordered_set;

/** ����ȥ�� */
unordered_set<string> Deduplication(const char* path) 
{
	string result;  // ���ݸ�ʽ��>�ַ���
	unordered_set<string> data_lib;  // ����ַ���
	unsigned int pos = 0;   // ��ǰ��������
	unsigned int diff = 0;  // ��ͬ��������
	unsigned int same = 0;  // ��ͬ��������

	std::ifstream ifs(path);

	if (!ifs.is_open())
	{
		std::cout << "Can't open this file." << std::endl;
		exit(-1);
	}

	while (!ifs.eof())
	{
		pos++;  // ��������+1
		std::getline(ifs, result, '\n');
		std::replace(result.begin(), result.end(), '\t', '/');  // '\t' -> '/'

		// ��ͬ
		if (data_lib.count(result) != 0)
		{
			++same;
			continue;
		}
		// ��ͬ
		data_lib.emplace(result);
		++diff;
	}
	
	std::cout << "Total��" << pos << "\n";
	std::cout << "Differener��" << diff << "\n";
	std::cout << "Same��" << same << std::endl;

	ifs.close();
	return data_lib;
}

/** ȥ��β���ո� */
void ExportFinalData(unordered_set<string>& lib, const char* path)
{
	int pos = 0;  // ��ǰ��������
	std::ofstream ofs(path);
	for (auto data : lib)
	{
		std::replace(data.begin(), data.end(), '/', '\t');
		ofs << data;
		pos++;
		if (pos < lib.size())
			ofs << "\n";
	}
	ofs.close();
}