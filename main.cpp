/*******************************************************
 * @File name：REACHABLE_TREE_MAIN_CPP_
 * @Funciton ：Run the timed Petri Net
 * @Content  ：instantiation, operation, display result
 * @Update   ：2023/07/18 11:30
 ******************************************************/

#include <iostream>
#include <thread>
#include "input.h"
#include "node.h"
#include "petri.h"
#include "output.h"


int main(int argc, char* argv[]){
	/* 读数据 */
	std::vector<int> m0 = ReadVector<int>(kInitMark);
	std::vector<int> d0 = ReadVector<int>(kTimePath);
	std::vector<vector<int>> pre = ReadMatrix(kPrePath, m0.size());
	std::vector<vector<int>> post = ReadMatrix(kPostPath, m0.size());
	std::vector<int> goals = ReadVector<int>(kGoalPlace);
	std::vector<int> goal_marking = ReadVector<int>(kGoalMarking);
	std::vector<int> goal_vector = GetGoalMark(goal_marking, goals, m0.size());

	/* 正向树与反向树 */
	PetriNet petri(m0, d0, pre, post, goal_vector, 1);
	petri.ForwardTree();
	petri.BackTree();
	petri.DeadLockNodeDeal();

	/* 信息打印 */
	std::thread t1(GlobalGraphCreate, std::ref(petri));
	std::thread t2(DataCreateTxt, std::ref(petri), m0.size());
	std::thread t3(DataCreateCsv, std::ref(petri), m0.size(), petri.num_transition_);
	t1.join();
	t2.join();
	t3.join();
	InfoCreate(petri);

	return 0;
}
