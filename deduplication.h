#pragma once
/**************************************************
* @This function is used to delete same data.
* @Input     : several datas path
* @Output    : one dataset with data deduplication
* @Up-to-date: 2023-07-17 16:07
**************************************************/


#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>

using std::vector;
using std::string;
using std::unordered_set;

/** ����ȥ�� */
unordered_set<string> Deduplication(const string folder_path) 
{
	string result;  // ���ݸ�ʽ��>�ַ���
	unordered_set<string> data_lib;  // ����ַ���
	unsigned int pos = 0;   // ��ǰ��������
	unsigned int diff = 0;  // ��ͬ��������
	unsigned int same = 0;  // ��ͬ��������
	vector<int> index = {111, 112, 121};  // ָ�����ں����ݼ��ļ�

	for(auto it  = index.begin(); it != index.end(); ++it)
	{
		string path = folder_path + "\\" + std::to_string(*it);  // �����ļ���·��
		path += "\\Dataset_" + std::to_string(*it).append(".txt");  // ���ݼ��ļ�

		std::ifstream ifs(path);

		if (!ifs.is_open())
		{
			std::cout << "Can't open " << *it << " file." << std::endl;
			continue;
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
		ifs.close();
	}
	
	std::cout << "Total��" << pos << "\n";
	std::cout << "Differener��" << diff << "\n";
	std::cout << "Same��" << same << std::endl;

	return data_lib;
}

/** ȥ��β���ո� */
void ExportFinalData(unordered_set<string> &lib, const char* path)
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
