#pragma once
/**
 * ���ļ���������Petri���������Ϣ
 * ��������ʼ��ʶ��ʱ�ӡ�Ŀ���ʶ����������
 * �Լ���ص������Ϣ
 * ����Infor��Ϣ���ɴ�ͼ�����ݼ�
 */

constexpr auto M0_path = "data/lj/111/m0.txt";
constexpr auto GoalMarking = "data/lj/111/GoalMarking.txt";
constexpr auto Info_path = "data/lj/111/infor_.txt";
constexpr auto output_path_py = "data/lj/111/Dataset_111.txt";
constexpr auto output_path_gv = "data/lj/111/Reachable_Graph_111.gv";
/* ������� */
constexpr auto Pre_path = "data/lj/111/pre.txt";
constexpr auto Post_path = "data/lj/111/post.txt";
constexpr auto Time_path = "data/lj/111/delay.txt";
constexpr auto GoalPlace = "data/lj/111/GoalPlace.txt";

//constexpr auto Module_path = "D:\\SecretFiLe\\Net\\Module.pt";
//constexpr auto output_path2 = ".\\Reachable_tree\\data\\lj\\111\\output2.txt";
//constexpr auto Goal_path = ".\\data\\lj\\111\\m_target.txt";
//constexpr auto Optimal_path = ".\\data\\lj\\211\\optimal.txt";
//constexpr auto Model_path = ".\\model\\latest_model.pt";
//constexpr auto model_path = ".\\data\\sijia\\gpu_model.pt";

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using std::vector;
template<class T>
/* ��ȡ��ʶ����ʱ���� */
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

/* ��ȡ�������� */
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

/* ��ȡĿ��ڵ��ʶ */
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
