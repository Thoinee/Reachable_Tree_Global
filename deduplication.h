#pragma once
/**************************************************
* @Function  : be used to delete same data.
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

typedef unordered_map<string, string> Mymap;
typedef unsigned int Uint;

//static const vector<string> dataset = { "001", "010", "011", "100", "101", "110", "111", 
//										"112", "121", "122", "211", "212", "221", "222",
//										"223", "232", "233", "322", "323", "332", "333"};  // ָ�����ں����ݼ��ļ�

static const vector<string> dataset = { "122_"/*, "011"*/ };  // ָ�����ں����ݼ��ļ�

/** ����ȥ�� */
unordered_map<string, string>* Deduplication(const string folder_path) 
{
	Mymap* data = new Mymap;  // �������
	Uint diff = 0;  // ��ͬ��������
	Uint same = 0;  // ��ͬ��������
	Uint index = 0; // ��ǰ��������

	for(auto it = dataset.begin(); it != dataset.end(); ++it){
		string path = folder_path + "\\" + *it + "\\Dataset.txt";  // �����ļ���·��

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
			if (data->find(key) != data->end()) {
				auto h = data->find(key)->second;  // ��ȡhֵ
				/*if (std::stof(val) == std::stof(h)) {
					same++;
				}*/
				if (std::stof(val) < std::stof(h)) {
					data->at(key) = val;
				}
				same++;
				continue;
			}
			// ��ͬ
			data->insert(std::pair<string, string>(key, val));
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
void ExportFinalData(unordered_map<string, string>* lib, const char* path)
{
	int pos = 0;  // ��ǰ��������

	std::ofstream ofs(path); // ȥ�����������

	for (auto it : *lib){
		ofs << it.first << '\t' << it.second;
		pos++;
		if (pos < lib->size()) ofs << "\n";
	}
	ofs.close();
}
