#pragma once

/*******************************************************
* @File name：REACHABLE_TREE_DATA_INPUT_H_
* @Funciton ：Input basic information of timed Petri Net
* @Content  ：m0, delay, goal, C+, C-
* @Update   ：2023/07/18 11:30
*******************************************************/

constexpr auto M0_path = "data/lj/231/m0.txt";
constexpr auto GoalMarking = "data/lj/231/GoalMarking.txt";
constexpr auto Info_path = "data/lj/231/Info.txt";   
constexpr auto output_path_py = "data/lj/231/Dataset_231.txt";
constexpr auto output_path_gv = "data/lj/231/Reachable_Graph_231.gv";
/* 无需更改 */
constexpr auto Pre_path = "data/lj/111/pre.txt";
constexpr auto Post_path = "data/lj/111/post.txt";
constexpr auto Time_path = "data/lj/111/delay.txt";
constexpr auto GoalPlace = "data/lj/111/GoalPlace.txt";

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
