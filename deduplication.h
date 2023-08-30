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
//										"223", "232", "233", "322", "323", "332", "333"};  // 指定待融合数据集文件

static const vector<string> dataset = { "122_"/*, "011"*/ };  // 指定待融合数据集文件

/** 数据去重 */
unordered_map<string, string>* Deduplication(const string folder_path) 
{
	Mymap* data = new Mymap;  // 存放数据
	Uint diff = 0;  // 异同的数据量
	Uint same = 0;  // 相同的数据量
	Uint index = 0; // 当前数据索引

	for(auto it = dataset.begin(); it != dataset.end(); ++it){
		string path = folder_path + "\\" + *it + "\\Dataset.txt";  // 任务文件夹路径

		std::ifstream ifs(path);

		if (!ifs.is_open()){
			std::cout << "Can't open " << *it << " file." << std::endl;
			exit(-1);
		}

		string data_to_str;  // 数据—>字符串

		while (!ifs.eof()){
			index++;  // 数据索引+1
			std::getline(ifs, data_to_str, '\n');

			int pos = data_to_str.rfind('\t');  // 返回倒数第一个制表符的位置
			string key = data_to_str.substr(0, pos);  // m + x + (x/d)
			string val = data_to_str.substr(pos + 1); // h

			// key值相同 保留h值小的元素
			if (data->find(key) != data->end()) {
				auto h = data->find(key)->second;  // 获取h值
				/*if (std::stof(val) == std::stof(h)) {
					same++;
				}*/
				if (std::stof(val) < std::stof(h)) {
					data->at(key) = val;
				}
				same++;
				continue;
			}
			// 异同
			data->insert(std::pair<string, string>(key, val));
			++diff;
		}
		ifs.close();
	}
	
	std::cout << "Total：" << index << "\n";
	std::cout << "Different：" << diff << "\n";
	std::cout << "Same：" << same << std::endl;

	return data;
}

/** 去除尾部空格 */
void ExportFinalData(unordered_map<string, string>* lib, const char* path)
{
	int pos = 0;  // 当前数据索引

	std::ofstream ofs(path); // 去重数据输出流

	for (auto it : *lib){
		ofs << it.first << '\t' << it.second;
		pos++;
		if (pos < lib->size()) ofs << "\n";
	}
	ofs.close();
}
