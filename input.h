#pragma once
/********************************************************
 * @File name：REACHABLE_TREE_DATA_INPUT_H_
 * @Funciton ：Input basic information of timed Petri Net
 * @Content  ：m0, delay, goal, C+, C-
 * @Update   ：2023/07/18 11:30
 *******************************************************/

/* 任务量更改 */
constexpr auto kInitMark = "./data/lj/111/m0.txt";
constexpr auto kGoalMarking = "./data/lj/111/GoalMarking.txt";
/* 数据集txt文件 */
constexpr auto kOutputTxtPath = "./data/lj/111/Dataset.txt";
/* lyd 数据集csv文件 */
constexpr auto kOutputCsvMPath = "./data/lj/111/m.csv";
constexpr auto kOutputCsvXPath = "./data/lj/111/x.csv";
constexpr auto kOutputCsvHPath = "./data/lj/111/h.csv";
/* 可视化gv文件 */
constexpr auto kOutputGvPath = "./data/lj/111/Reachable_Graph.gv";
/* 信息文件 */
constexpr auto kInfoPath = "./data/lj/111/Info.txt";
/* 无需更改 */
constexpr auto kPrePath = "./data/lj/Basic/pre.txt";
constexpr auto kPostPath = "./data/lj/Basic/post.txt";
constexpr auto kTimePath = "./data/lj/Basic/delay.txt";
constexpr auto kGoalPlace = "./data/lj/Basic/GoalPlace.txt";

//constexpr auto M0_path = "./data/lzx/M0.txt";
//constexpr auto GoalMarking = "./data/lzx/GoalMarking.txt";
//constexpr auto Pre_path = "./data/lzx/Pre.txt";
//constexpr auto Post_path = "./data/lzx/Post.txt";
//constexpr auto Time_path = "./data/lzx/delays.txt";
//constexpr auto GoalPlace = "./data/lzx/GoalPlace.txt";

#include <iostream>
#include <fstream>
#include <vector> 
#include <string>

using std::vector;
template<class T> 
/* 读取初始标识、延时 */
vector<T> ReadVector(const char* path) {
	T num;
	vector<T> result;
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "fail to oepn file :" << path << std::endl;
		return {};
	}
	while (file >> num)
		result.push_back(num);
	file.close();
	return result;
}

/* 读取关联矩阵 */
vector<vector<int>> ReadMatrix(const char* path, int m)
{
	vector<vector<int>> matrix;
	std::ifstream file(path, std::ios::in);
	if (!file.is_open())
	{
		std::cout << "can not open file:" << path << std::endl;
		exit(1);
	}
	int num = 0;
	vector<int> ans;
	while (file >> num) {
		ans.push_back(num);
	}
	int n = ans.size() / m;
	matrix.resize(m, vector<int>(n, 0));
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			matrix[i][j] = ans[i*n + j];
	file.close();
	return matrix;
}

/* 获取目标节点标识 */
vector<int> GetGoalMark(vector<int>& goalMarking, vector<int>& goalPlace, int nums) {
	vector<int> ans(nums, 0);
	if (goalMarking.size() != goalPlace.size())
	{
		std::cout << "The dimension of goal marking and goal place is not consistent!" << std::endl;
		exit(-1);
	}
	for (int i = 0; i < goalPlace.size(); ++i) {
		ans[goalPlace[i]] = goalMarking[i];
	}
	return ans;
}
