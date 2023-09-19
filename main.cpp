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
	std::vector<int> m0 = readVector<int>(kInitMark);
	std::vector<int> d0 = readVector<int>(kTimePath);
	std::vector<int> goals = readVector<int>(kGoalPlace);
	std::vector<int> goal_marking = readVector<int>(kGoalMarking);
	std::vector<int> goal_vector = getGoalMark(goal_marking, goals, m0.size());
	std::vector<vector<int>> pre = readMatrix(kPrePath, m0.size());
	std::vector<vector<int>> post = readMatrix(kPostPath, m0.size());

	/* 正向树与反向树 */
	PetriNet petri(m0, d0, pre, post, goal_vector, 1);
	petri.forwardTree();
	petri.backTree();
	petri.deadLockNodeDeal();

	/* 信息打印 */
	std::thread t1(globalGraphCreate, std::ref(petri));
	std::thread t2(dataCreateTxt, std::ref(petri), m0.size());
	//std::thread t3(dataCreateCsv, std::ref(petri), m0.size(), petri.num_transition_);
	std::thread t4(infoCreate, std::ref(petri));
	t1.join();
	t2.join();
	//t3.join();
	t4.join();

	return 0;
}
