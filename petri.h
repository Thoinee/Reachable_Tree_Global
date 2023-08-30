#pragma once
/*******************************************************
 * @File name：REACHABLE_TREE_PETRI_H_
 * @Funciton ：Create class PetriNet
 * @Content  ：<1> Implete the operation rules of Timed
 *                 Petri Net
 *             <2> Introduce new and old nodes judgement
 * @Update   ：2023/08/21 13:25
 ******************************************************/

#include <queue>
#include <map>
#include <string>
#include <algorithm>
#include <ctime>
#include <mutex>
#include <list>
#include <unordered_map>
#include <stack>
#include "thread_pool.h"
#include "node.h"

using std::string;
using std::list;

/* 搜索策略 g值优先 */
class open_cmp {
public:
	bool operator()(const ptrNode n1, const ptrNode n2) {
		return n1->g_ > n2->g_;
	}
};

class PetriNet :public ThreadPool {
public:
	/* 库所个数 */
	int num_place_;
	/* 变迁个数 */
	int num_transition_;
	/* 节点池 */
	NodePool pool_;
	/* 根节点 */
	ptrNode root_;
	/* 目标节点 */
	ptrNode goal_node_;
	/* 前置矩阵转置 */
	vector<vector<int>> Tpre_;
	/* 后置矩阵转置 */
	vector<vector<int>> Tpost_;
	/* 关联矩阵转置 */
	vector<vector<int>> C;
	/* 初始标识 */
	vector<int> m0_;
	/* 延时 */
	vector<int> delay_;
	/* 目标标识 */
	vector<int> goal_;
	/* 存储目标节点 */
	vector<ptrNode> goal_nodes_;
	/* 存储死锁节点 */
	std::unordered_map<unsigned, ptrNode> deadlock_nodes_;
	/* 存储叶子节点 */
	std::unordered_map<unsigned, ptrNode> leaf_nodes_;
	/* 存储新节点 open表 */
	std::priority_queue<ptrNode, vector<ptrNode>, open_cmp> open_list_;
	/* 存储扩展过的节点 close + open表 */
	std::map<string, list<ptrNode>> entire_list_;

	/* 初始化参数 */
	PetriNet(vector<int>& m, vector<int >& d, vector<vector<int>>& p, vector<vector<int>>& q, vector<int>& goal, unsigned int num_threads = 1) :
		ThreadPool(num_threads), pool_(10000000), m0_(m), delay_(d), goal_(goal)
	{
		if (p.size() == 0 || m0_.size() == 0 || delay_.size() == 0 || goal_.size() == 0 || q.size() == 0)
		{
			printf("input files error!\n");
			exit(-1);
		}
		num_place_ = m.size();
		num_transition_ = p[0].size();
		Tpre_.resize(num_transition_, vector<int>(num_place_, 0));
		Tpost_.resize(num_transition_, vector<int>(num_place_, 0));
		C.resize(num_transition_, vector<int>(num_place_, 0));
		for (int i = 0; i < num_place_; ++i) {
			int num_of_place = 0;
			for (int j = 0; j < num_transition_; ++j) {
				Tpre_[j][i] = p[i][j];
				Tpost_[j][i] = q[i][j];
				C[j][i] = q[i][j] - p[i][j];
				if (p[i][j] != 0) {
					++num_of_place;
				}
			}

		}

		root_ = pool_.GetNode();
		for (int i = 0; i < num_place_; ++i) {
			if (m0_[i] != 0)
				root_->state_.emplace_back(Place(m0_[i], i, 0));
		}
		goal_node_ = pool_.GetNode();
		for (int i = 0; i < num_place_; ++i) {
			if (goal_[i] != 0)
				goal_node_->state_.emplace_back(Place(goal_[i], i, 0));
		}
		open_list_.push(root_);
		std::list<ptrNode> temp = { root_ };
		entire_list_.emplace(root_->to_string(), std::move(temp));  // move移动资源，原地址存放的资源不存在
		return;
	}

	/* 使能变迁判断 */
	vector<int> EnableTrans(ptrNode node) {
		vector<int> ans;
		for (int i = 0; i < num_transition_; ++i) {
			if (*node > Tpre_[i])
				ans.push_back(i);
		}
		node->sons_ = ans.size();
		return ans;
	}

	/* 目标节点判断 */
	bool IsGoalNode(ptrNode curnode) {
		if (*(curnode) > goal_) {
			return 1;
		}
		return 0;
	}

	/* 计算已等待时间v、还需等待最长时间λ */
	int UpdateVk(ptrNode newnode, ptrNode curnode, int t) {
		int lambda = 0;
		for (int i = 0; i < num_place_; ++i) {
			if (Tpre_[t][i] != 0) {
			// [i]为Node类中的符号重载
			// 意为查看第i个库所是否为空
			// 若为空，返回place->row_ = -1
			// 否则库所i的状态
				if ((*curnode)[i].row_ == -1) {
					printf("something wrong in 110 rows\n");
					exit(-1);
				}
				int diff = delay_[i] - (*curnode)[i].v_;
				lambda = std::max(lambda, diff);
			}
		}
		for (int i = 0; i < newnode->state_.size(); ++i) {

			/** 
			* 激发变迁后，place有两种情况：
			* 情况一：原先就有token
			* 情况二：激发后后置矩阵给予的token
			* 以下分析为情况一
			*/

			if (Tpost_[t][newnode->state_[i].row_] == 0)
				// 已等待时间v不超过延迟，否则会出现状态无穷多
				newnode->state_[i].v_ = std::min(delay_[newnode->state_[i].row_], newnode->state_[i].v_ + lambda);
		}
		return lambda;
	}

	/* 变迁激发过程 */
	void Fire(ptrNode curnode, int t) {
		auto newnode = *curnode + C[t];
		newnode->discarded_ = false;
		int waiting_time = UpdateVk(newnode, curnode, t);
		newnode->g_ = curnode->g_ + waiting_time;
		newnode->fathers.emplace_back(std::make_tuple(t, curnode->id_, waiting_time, curnode));
		if (IsGoalNode(newnode)) {
			newnode->h_ = 0;
			goal_nodes_.emplace_back(newnode);
			return;
		}
		auto str = newnode->to_string();
		auto pair = IsNewNode(newnode);   // return <bool,list<ptrNode>>  bool = 1 or 为新节点  bool = 0 旧节点
		if (pair.second == nullptr)
		{
			list<ptrNode> temp;
			temp.push_back(newnode);
			entire_list_.emplace(str, std::move(temp));
			open_list_.push(newnode);
			newnode->is_open_ = true;
		}
		else
		{    /* 不是新节点，回收 */
			if (!pair.first) {
				if ((--curnode->sons_) == 0)
					leaf_nodes_.emplace(curnode->id_, curnode);
				pool_.Recycling(newnode);
				return;
			}
			/* 是新节点，放入open_list和entire_list */
			else {
				pair.second->emplace_back(newnode);
				open_list_.push(newnode);
				newnode->is_open_ = true;
			}
		}
	}

	/* 传统判断(是否为旧节点) */
	bool IsSame(ptrNode& newnode, ptrNode& oldnode) {
		for (int i = 0; i < newnode->state_.size(); ++i) {
			if (newnode->state_[i].v_ != oldnode->state_[i].v_)
				return false;
		}
		oldnode->fathers.push_back(newnode->fathers[0]);
		std::get<3>(newnode->fathers[0])->sons_++;
		return true;
	}

	/* 先 g 后 v 进行判断(是否为新节点) */   
	bool IsNew(ptrNode& newnode, ptrNode& oldnode) {
		if (newnode->g_ > oldnode->g_) {
			return false;
		}
		short equal = 0;  // equal：相同v的库所
		for (int i = 0; i < newnode->state_.size(); ++i) {
			if (newnode->state_[i].v_ < oldnode->state_[i].v_) {
				return false;
			}
			else if (newnode->state_[i].v_ == oldnode->state_[i].v_) {
				equal++;
			}              
		}
		if (equal == newnode->state_.size()) {
			for (auto f : oldnode->fathers) {
				newnode->fathers.push_back(f);
				std::get<3>(f)->sons_++;
			}
			/*auto f_node = oldnode->fathers[0];
			newnode->fathers.push_back(f_node);
			std::get<3>(f_node)->sons_++;*/
		}

		return true;
	}

	/* 时间轴判断(是否为旧节点) */
	bool IsOld(ptrNode& newnode, ptrNode& oldnode) {
		short equal = 0;
		for (int i = 0; i < newnode->state_.size(); ++i) {
			if (newnode->g_ - newnode->state_[i].v_ < oldnode->g_ - oldnode->state_[i].v_)
				return false;
			else if (newnode->state_[i].v_ == oldnode->state_[i].v_) {
				equal++;
			}
		}
		if (equal == oldnode->state_.size()) {
			oldnode->fathers.push_back(newnode->fathers[0]);
			std::get<3>(newnode->fathers[0])->sons_++;
		}
		return true;
	}

	/* 新旧节点判断 */
 	std::pair<bool, list<ptrNode>*> IsNewNode(ptrNode newnode) {
		auto str = newnode->to_string();
		if (entire_list_.count(str) <= 0) {
			return std::make_pair(1, nullptr);
		}
		auto it = entire_list_.find(str);

		/* 先 g 后 v */
		//for (auto itor = it->second.begin(); itor != it->second.end();) {
		//	auto oldnode = *itor;
		//	/* 判断新拓展出来节点的新旧性 */
		//	if (IsOld(newnode, oldnode)) {
		//		return std::make_pair(0, &(it->second));
		//	}
		//	/* 判断是否删除旧节点 */
		//	if (IsOld(oldnode, newnode)) {
		//		if (oldnode->sons_ == 0) { 
		//			for (auto f : oldnode->fathers) {
		//				auto f_node = std::get<3>(f);
		//				if ((--f_node->sons_) == 0) {
		//					leaf_nodes_.emplace(f_node->id_, f_node);
		//				}
		//			}

		//			if (leaf_nodes_.count(oldnode->id_)) {
		//				leaf_nodes_.erase(oldnode->id_);
		//			} // 在叶子节点表中删除oldnode
		//			else if (deadlock_nodes_.count(oldnode->id_)) {
		//				deadlock_nodes_.erase(oldnode->id_);
		//			} // 在死锁节点表中删除oldnode

		//			if (!oldnode->is_open_) {  // 只回收不在open表中的节点
		//				pool_.Recycling(oldnode);
		//			}
		//			else { oldnode->discarded_ = true; }      

		//			itor = it->second.erase(itor); // 从close表中删除
		//		}
		//		else { itor++; }
		//	}
		//	else { itor++; }
		//}

		/* 传统判断 */
		for (auto itor = it->second.begin(); itor != it->second.end(); ++itor) {
			auto oldnode = *itor;
			if (IsSame(newnode, oldnode)) {
				return std::make_pair(0, &it->second);
			}
		}

		return std::make_pair(1, &(it->second));
	}

	/* 正向树 */
	void ForwardTree() {
		std::cout << "\nBegin forward tree -> ";
		clock_t start = clock();
		while (!open_list_.empty()) {
			auto curnode = open_list_.top();
			open_list_.pop();
			if (curnode->discarded_) { continue; }
			auto enables = EnableTrans(curnode);
			if (enables.empty()) {
				curnode->is_deadlock_ = true;
				deadlock_nodes_.emplace(curnode->id_, curnode);
				continue;
			}
			for (auto t : enables) {
				Fire(curnode, t); 
			}
		}
		clock_t end = clock();
		auto programTimes = end - start;
		std::cout << "Forward tree finish（" << programTimes << "ms）->";
	}

	/* 反向树 */
	void BackTree(){
		std::cout << "Begin back tree -> ";
		clock_t start = clock();
		vector<ptrNode> back_node = goal_nodes_;
		for (auto& node_ : deadlock_nodes_) {
			back_node.insert(back_node.end(), node_.second);
		}
		for (auto& node_ : leaf_nodes_) {
			back_node.insert(back_node.end(), node_.second);
		}
		while (!back_node.empty()) {
			auto node = back_node.back();
			back_node.pop_back();
			for (auto f : node->fathers) {
				auto fnode = std::get<3>(f);
				fnode->h_ = std::min(node->h_ + std::get<2>(f), fnode->h_);
				if (node->h_ < 9999) {
					fnode->Q_.push_back(std::make_pair(std::get<0>(f), node->h_));
				}
				if ((--fnode->sons_) == 0) {
					back_node.push_back(fnode);
				}
			}
		}
		clock_t end = clock();
		auto programTimes = end - start;
		std::cout << "Back tree finish（" << programTimes << "ms）\n" << std::endl;
	}

	/* 死锁节点处理 */
	void DeadLockNodeDeal() {
		for (auto &obj : deadlock_nodes_) {
			auto node = obj.second;
			node->h_ = root_->h_ * 1.2;
			if (node->fathers.empty()) {
				continue; 
			}
			auto f_node = std::get<3>(node->fathers[0]);
			while(f_node->h_ > 9999){
				f_node->h_ = std::get<2>(node->fathers[0]) + node->h_;
				if (f_node->fathers.empty())
					break;
				node = f_node;
				f_node = std::get<3>(f_node->fathers[0]);
			}
		}
	}

	~PetriNet() {}
};