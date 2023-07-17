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

/** 数据去重 */
unordered_set<string> Deduplication(const string folder_path) 
{
	string result;  // 数据格式—>字符串
	unordered_set<string> data_lib;  // 存放字符串
	unsigned int pos = 0;   // 当前数据索引
	unsigned int diff = 0;  // 异同的数据量
	unsigned int same = 0;  // 相同的数据量
	vector<int> index = {111, 112, 121};  // 指定待融合数据集文件

	for(auto it  = index.begin(); it != index.end(); ++it)
	{
		string path = folder_path + "\\" + std::to_string(*it);  // 任务文件夹路径
		path += "\\Dataset_" + std::to_string(*it).append(".txt");  // 数据集文件

		std::ifstream ifs(path);

		if (!ifs.is_open())
		{
			std::cout << "Can't open " << *it << " file." << std::endl;
			continue;
		}

		while (!ifs.eof())
		{
			pos++;  // 数据索引+1
			std::getline(ifs, result, '\n');
			std::replace(result.begin(), result.end(), '\t', '/');  // '\t' -> '/'

			// 相同
			if (data_lib.count(result) != 0)
			{
				++same;
				continue;
			}
			// 异同
			data_lib.emplace(result);
			++diff;
		}
		ifs.close();
	}
	
	std::cout << "Total：" << pos << "\n";
	std::cout << "Differener：" << diff << "\n";
	std::cout << "Same：" << same << std::endl;

	return data_lib;
}

/** 去除尾部空格 */
void ExportFinalData(unordered_set<string> &lib, const char* path)
{
	int pos = 0;  // 当前数据索引
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
