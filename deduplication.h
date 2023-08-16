#pragma once
/**************************************************
* @This function is used to delete same data.
* @Input     : several datas path
* @Output    : one dataset with data deduplication
* @Up-to-date: 2023-08-16 08:27
**************************************************/

#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>

using std::vector;
using std::string;
using std::unordered_map;

static const vector<int> dataset = { 111, 112, 113, 121, 122, 123, 131, 132, 133, 211, 212, 213, 221, 222, 223, 231, 232, 233, 311, 312, 313, 321,
322, 323, 331, 332, 333, 334, 343, 344, 433, 434, 443, 444};  // ָ�����ں����ݼ��ļ�

/** ����ȥ�� */
unordered_map<string, string> Deduplication(const string folder_path) 
{
	unordered_map<string, string> data;  // �������
	unsigned int diff = 0;  // ��ͬ��������
	unsigned int same = 0;  // ��ͬ��������
	unsigned int index = 0; // ��ǰ��������

	for(auto it = dataset.begin(); it != dataset.end(); ++it){
		string path = folder_path + "\\" + std::to_string(*it);  // �����ļ���·��
		path += "\\Dataset_" + std::to_string(*it).append(".txt");  // ���ݼ��ļ�

		std::ifstream ifs(path);

		if (!ifs.is_open()){
			std::cout << "Can't open " << *it << " file." << std::endl;
			exit(-1);
		}

		string data_to_str;  // ���ݡ�>�ַ���

		while (!ifs.eof()){
			index++;  // ��������+1
			std::getline(ifs, data_to_str, '\n');

			int pos = data_to_str.rfind('\t');  // ���ص�����һ���Ʊ����λ��
			string key = data_to_str.substr(0, pos);  // m + x + (x/d)
			string val = data_to_str.substr(pos + 1); // h

			// keyֵ��ͬ ����hֵС��Ԫ��
			if (data.count(key) != 0) {
				auto h = data.find(key)->second;  // ��ȡhֵ
				if (std::stof(val) < std::stof(h)) {
					data.at(key) = val;
				}
				++same;
				continue;
			}
			// ��ͬ
			data.insert(std::pair<string, string>(key, val));
			++diff;
		}
		ifs.close();
	}
	
	std::cout << "Total��" << index << "\n";
	std::cout << "Different��" << diff << "\n";
	std::cout << "Same��" << same << std::endl;

	return data;
}

/** ȥ��β���ո� */
void ExportFinalData(unordered_map<string, string> lib, const char* path)
{
	int pos = 0;  // ��ǰ��������

	std::ofstream ofs(path);

	for (auto it : lib){
		ofs << it.first << '\t' << it.second;
		pos++;
		if (pos < lib.size()) ofs << "\n";
	}
	ofs.close();
}
