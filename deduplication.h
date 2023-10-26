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

/** 指定待融合数据集文件 */
static const vector<string> dataset = { "001", "010", "011", "100", "101", "110", "111", 
										"112", "211", "121", "122", "212", "221", "222",
										"223", "232", "233", "322", "323", "332", "333",
										"334", "343", "344", "433", "434", "443", "444",
										"445", "454", "455", "544", "545", "554", "555",
										"556", "565", "566", "655", "656", "665", "666"};

// static const vector<string> dataset = { "221", "001" };  // 指定待融合数据集文件

/** 数据去重 */
MymapPtr deduplication(const string folder_path) {
	MymapPtr data = std::make_shared<Mymap>();  // 存放数据
	Uint diff = 0;  // 异同的数据量
	Uint same_data_diff_h = 0;  // 相同数据不同标签的数据量
	Uint same_data_and_h = 0;  // 相同数据标签的数据量
	Uint index = 0; // 当前数据索引

	for(auto it = dataset.begin(); it != dataset.end(); ++it) {
		string path = folder_path + "\\" + *it + "\\Dataset.txt";  // 任务文件夹路径

		std::ifstream ifs(path);

		if (!ifs.is_open()) {
			std::cout << "Can't open " << *it << " file." << std::endl;
			exit(-1);
		}

		string data_to_str;  // 数据—>字符串

		while (!ifs.eof()) {
			++index;  // 数据索引+1
			std::getline(ifs, data_to_str, '\n');

			int pos = data_to_str.rfind('\t');  // 返回倒数第一个制表符的位置
			string key = data_to_str.substr(0, pos);  // m + x + (x/d)   
			string val = data_to_str.substr(pos + 1); // h

			// key值相同 保留h值小的元素
			if (data->find(key) != data->end()) {
				auto h = data->find(key)->second;  // 获取h值
				if (std::stof(val) == std::stof(h)) {
					++same_data_and_h;
				} else {
					++same_data_diff_h;
				}
				continue;
			}
			// 异同
			data->insert(std::pair<string, string>(key, val));
			++diff;
		}
		ifs.close();
	}
	
	std::cout << "Total：" << index << "\n";
	std::cout << "Dataset：" << diff << "\n";
	std::cout << "Same：" << same_data_and_h << "\n";
	std::cout << "Different：" << same_data_diff_h << std::endl;

	return data;
}

/** 去除尾部空格 */
void exportFinalData(MymapPtr lib, const char* path)
{
	int pos = 0;  // 当前数据索引

	std::ofstream ofs(path); // 去重数据输出流

	for (auto it : *lib){
		ofs << it.first << '\t' << it.second;
		if (++pos < lib->size()) ofs << "\n";
	}

	ofs.close();
}
