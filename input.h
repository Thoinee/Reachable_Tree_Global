#pragma once
/********************************************************
 * @File name：REACHABLE_TREE_INPUT_H_
 * @Funciton ：Input basic information of timed Petri Net
 * @Content  ：m0, delay, goal, C+, C-
 * @Update   ：2023/07/18 11:30
 *******************************************************/

/* 任务量更改 */
constexpr auto kInitMark = "./data/lj/111/m0.txt";
constexpr auto kGoalMarking = "./data/lj/111/GoalMarking.txt";

/* 数据集txt 可视化gv 信息info文件 */
constexpr auto kOutputTxtPath = "./data/lj/111/Dataset.txt";
constexpr auto kOutputGvPath = "./data/lj/111/Reachable_Graph.gv";
constexpr auto kInfoPath = "./data/lj/111/Info.txt";

/* lyd 数据集csv文件 */
constexpr auto kOutputCsvMPath = "./data/lj/111/m.csv";
constexpr auto kOutputCsvXPath = "./data/lj/111/x.csv";
constexpr auto kOutputCsvHPath = "./data/lj/111/h.csv";
constexpr auto kOutputCsvQPath = "./data/lj/111/Q.csv";

/* 无需更改 */
constexpr auto kPrePath = "./data/lj/Basic/pre.txt";
constexpr auto kPostPath = "./data/lj/Basic/post.txt";
constexpr auto kTimePath = "./data/lj/Basic/delay.txt";
constexpr auto kGoalPlace = "./data/lj/Basic/GoalPlace.txt";

//constexpr auto kInitMark = "./data/zjz/m0.txt";
//constexpr auto kOutputGvPath = "./data/zjz/Reachable_Graph.gv";
//constexpr auto kGoalMarking = "./data/zjz/GoalMarking.txt";
//constexpr auto kPrePath = "./data/zjz/pre.txt";
//constexpr auto kPostPath = "./data/zjz/post.txt";
//constexpr auto kTimePath = "./data/zjz/delay.txt";
//constexpr auto kGoalPlace = "./data/zjz/GoalPlace.txt";

//constexpr auto kInitMark = "./data/lzx/M0.txt";
//constexpr auto kGoalMarking = "./data/lzx/GoalMarking.txt";
//constexpr auto kPrePath = "./data/lzx/Pre.txt";
//constexpr auto kPostPath = "./data/lzx/Post.txt";
//constexpr auto kTimePath = "./data/lzx/delays.txt";
//constexpr auto kGoalPlace = "./data/lzx/GoalPlace.txt";

#include <iostream>
#include <fstream>
#include <vector> 
#include <string>

using std::vector;
template<class T>
/* 读取初始标识、延时 */
vector<T> readVector(const char* path) {
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
vector<vector<int>> readMatrix(const char* path, int m)
{
	vector<vector<int>> matrix;
	std::ifstream file(path, std::ios::in);
	if (!file.is_open()) {
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
vector<int> getGoalMark(vector<int>& goalMarking, vector<int>& goalPlace, int nums) {
	vector<int> ans(nums, 0);
	if (goalMarking.size() != goalPlace.size()){
		std::cout << "The dimension of goal marking and goal place is not consistent!" << std::endl;
		exit(-1);
	}
	for (int i = 0; i < goalPlace.size(); ++i) {
		ans[goalPlace[i]] = goalMarking[i];
	}
	return ans;
}
