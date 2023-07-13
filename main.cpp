#include <iostream>
#include <thread>
#include "data_input.h"
#include "node.h"
#include "petri_net.h"
#include "data_output.h"


int main(void)
{
	/* 读数据 */
	std::vector<int> m0 = ReadVector<int>(M0_path);
	std::vector<int> d0 = ReadVector<int>(Time_path);
	std::vector<vector<int>> pre = ReadMatrix(Pre_path, m0.size());
	std::vector<vector<int>> post = ReadMatrix(Post_path, m0.size());
	std::vector<int> goals = ReadVector<int>(GoalPlace);
	std::vector<int> goal_marking = ReadVector<int>(GoalMarking);
	std::vector<int> goal_vector = GetGoalMark(goal_marking, goals, m0.size());

	/* 正向数与反向树 */
	PetriNet petri(m0, d0, pre, post, goal_vector, 1);
	petri.ForwardTree();
	petri.BackTree();
	/*petri.LeafNodeDeal();
	petri.DeadLockNodeDeal();*/

	/* 信息打印 */
	std::thread t1(GlobalGraphCreate, std::ref(petri));
	std::thread t2(DataCreate, std::ref(petri), m0.size());
	t1.join();
	t2.join();
	InfoCreate(petri);
}
