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
#include <memory>

using std::vector;
using std::string;
using std::unordered_map;

typedef unordered_map<string, string> Mymap;
typedef unsigned int Uint;
typedef std::shared_ptr<Mymap> MymapPtr;

/** ָ�����ں����ݼ��ļ� */
static const vector<string> dataset = { "001", "010", "011", "100", "101", "110", "111", 
										"112", "211", "121", "122", "212", "221", "222",
										"223", "232", "233", "322", "323", "332", "333",
										"334", "343", "344", "433", "434", "443", "444",
										"445", "454", "455", "544", "545", "554", "555",
										"556", "565", "566", "655", "656", "665", "666"};

// static const vector<string> dataset = { "221", "001" };  // ָ�����ں����ݼ��ļ�

/** ����ȥ�� */
MymapPtr deduplication(const string folder_path) {
	MymapPtr data = std::make_shared<Mymap>();  // �������
	Uint diff = 0;  // ��ͬ��������
	Uint same_data_diff_h = 0;  // ��ͬ���ݲ�ͬ��ǩ��������
	Uint same_data_and_h = 0;  // ��ͬ���ݱ�ǩ��������
	Uint index = 0; // ��ǰ��������

	for(auto it = dataset.begin(); it != dataset.end(); ++it) {
		string path = folder_path + "\\" + *it + "\\Dataset.txt";  // �����ļ���·��

		std::ifstream ifs(path);

		if (!ifs.is_open()) {
			std::cout << "Can't open " << *it << " file." << std::endl;
			exit(-1);
		}

		string data_to_str;  // ���ݡ�>�ַ���

		while (!ifs.eof()) {
			++index;  // ��������+1
			std::getline(ifs, data_to_str, '\n');

			int pos = data_to_str.rfind('\t');  // ���ص�����һ���Ʊ����λ��
			string key = data_to_str.substr(0, pos);  // m + x + (x/d)   
			string val = data_to_str.substr(pos + 1); // h

			// keyֵ��ͬ ����hֵС��Ԫ��
			if (data->find(key) != data->end()) {
				auto h = data->find(key)->second;  // ��ȡhֵ
				if (std::stof(val) == std::stof(h)) {
					++same_data_and_h;
				} else {
					++same_data_diff_h;
				}
				continue;
			}
			// ��ͬ
			data->insert(std::pair<string, string>(key, val));
			++diff;
		}
		ifs.close();
	}
	
	std::cout << "Total��" << index << "\n";
	std::cout << "Dataset��" << diff << "\n";
	std::cout << "Same��" << same_data_and_h << "\n";
	std::cout << "Different��" << same_data_diff_h << std::endl;

	return data;
}

/** ȥ��β���ո� */
void exportFinalData(MymapPtr lib, const char* path)
{
	int pos = 0;  // ��ǰ��������

	std::ofstream ofs(path); // ȥ�����������

	for (auto it : *lib){
		ofs << it.first << '\t' << it.second;
		if (++pos < lib->size()) ofs << "\n";
	}

	ofs.close();
}
