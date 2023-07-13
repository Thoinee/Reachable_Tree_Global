#pragma once
#include "petri_net.h"
#include "data_input.h"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include <queue>
#include <unordered_set>

/* 无需用到的标识库所和已等待时间库所 */
static std::unordered_set<int> ignore_m = { 12,18,34 };
static std::unordered_set<int> ignore_v = { 0,1,2,3,4,5,6,7,8,9,10,11,12,17,18,33,34 };
/* 节点计数器 */
static long total_nodes_num = 0;

/* 生成全局可达图 */
void GlobalGraphCreate(PetriNet& tree) {
	std::ofstream file(output_path_gv);
	file << "digraph {" << '\n';
	/* 设置节点 */
	for (auto& node : tree.goal_nodes_) {
		file << "\"" 
			 << "m" << node->id_ 
			 << "_g" << node->g_ 
			 << "_h" << node->h_ 
			 << "\"" 
			 << " [color=red style=filled]" 
			 << "\n";
	}
	for (auto& nodes : tree.entire_list) {
		for (auto& node : nodes.second) {
			if (node->h_ < 9999) {
				if (tree.deadlock_nodes_.count(node->id_) > 0) {
					file << "\""
						<< "m" << node->id_
						<< "_g" << node->g_
						<< "_h" << node->h_
						<< "\""
						<< " [color=blue]"
						<< "\n";
				}
				else if (tree.leaf_nodes_.count(node->id_) > 0) {
					file << "\""
						<< "m" << node->id_
						<< "_g" << node->g_
						<< "_h" << node->h_
						<< "\""
						<< " [color=green]"
						<< "\n";
				}
				else {
					file << "\""
						<< "m" << node->id_
						<< "_g" << node->g_
						<< "_h" << node->h_
						<< "\""
						<< '\n';
				}
			}
		}
	}

	/* 设置节点间的连接弧 */
	/* 思想：从该节点找到其父节点 并进行连接 */
	for (auto& node : tree.goal_nodes_) {
		for (auto& father : node->fathers) {
			auto f_node = std::get<3>(father);
			file << "\"" 
				 << "m" << f_node->id_ 
				 << "_g" << f_node->g_ 
				 << "_h" << f_node->h_ 
				 << "\"" 
				 << " -> " 
				 << "\"" 
				 << "m" << node->id_ 
				 << "_g" << node->g_ 
				 << "_h" << node->h_ 
				 << "\"" 
				 << " [label=t" << std::get<0>(father) << "]\n";
		}
	}
	for (auto& nodes : tree.entire_list) {
		for (auto& node : nodes.second) {
			if (node->h_ < 9999) {
				for (auto& father : node->fathers) {
					auto f_node = std::get<3>(father);
					file << "\"" 
						 << "m" << f_node->id_ 
						 << "_g" << f_node->g_ 
						 << "_h" << f_node->h_ 
						 << "\"" 
						 << " -> " 
						 << "\"" 
						 <<"m" << node->id_ 
						 << "_g" << node->g_ 
						 << "_h" << node->h_ 
						 << "\"" 
						 << " [label=t" << std::get<0>(father) << "]\n";
				}
			}
		}
	}
	file << "}";
	file.close();
}

/* 输出到达目标路径 */
void GraphCreate(PetriNet& tree) {
	std::queue<ptrNode> back_node;
	std::unordered_set<unsigned short> already;
	short num_of_goals = 0;
	std::ofstream ofs(output_path_gv, std::ios::out);
	ofs << "digraph {" << '\n';
	for (auto &node : tree.goal_nodes_) {
		back_node.push(node);
	}
	while (!back_node.empty()){
		auto node = back_node.front();
		back_node.pop();
		if (num_of_goals < tree.goal_nodes_.size()) {
			num_of_goals++;
			ofs << "m" << node->id_ << "_g" << node->g_ << "_h" << node->h_ 
				<< " [color=red, style=filled]" << "\n";
		}
		else 
			ofs << "m" << node->id_ << "_g" << node->g_ << "_h" << node->h_ << "\n";
		if (node->fathers.empty()) { continue; }
		auto f_node = std::get<3>(node->fathers[0]);
		ofs << "m" << f_node->id_ << "_g" << f_node->g_ << "_h" << f_node->h_
			<< " -> " << "m" << node->id_ << "_g" << node->g_ << "_h" << node->h_
			<< " [label=t" << std::get<0>(node->fathers[0]) << "]\n";
		if (already.count(f_node->id_) == 0) {
			back_node.push(f_node);
			already.emplace(f_node->id_);
		}
	}
	ofs << "}";
	ofs.close();
}

template <typename T>
/* 转换为数据集格式 */
vector<T> ToVector(const ptrNode node, const int num_place) {   // 节点指针  PetriNet库所总数
	auto places = node->state_;
	int j = 0;   // 对已赋值的库所进行计数
	std::vector<T> ans;
	for (int i = 0; i < num_place; ++i) {   //  遍历所有库所，进行赋值
		if (ignore_m.count(i) >= 1)    // 若查到找ignore_m中存储的无用库所，则跳过不赋值
			continue;
		if (j < places.size()) {       // 计数值未到达需赋值的库所数
			if (i < places[j].row_) {  // 将state_中未记录的库所赋予0
				ans.push_back(0);
			}
			else if (i == places[j].row_) {      // 到达state_中包含的place，对token数进行赋值
				ans.push_back(places[j].tokens_);
				++j;                             // 计数值+1
			}
			else {                    // 由于上面有continue，会出现无赋值且i自动加1，导致i位置空出；ignore_m中包含库所存在于state_中；
				--i;                  // 所以利用--i跳到空位置；++j补上含有的place计数值，否则j无法==含有token的库所数
				++j;
			}
		}
		else {                       // 若计数值=含有token的库所数，则剩余的库所全置0
			ans.push_back(0);
		}
	}
	j = 0;

	/* 与上述同理 */
	for (int i = 0; i < num_place; ++i) {
		if (ignore_v.count(i) >= 1)
			continue;
		if (j < places.size()) {
			if (i < places[j].row_) {
				ans.push_back(0);
			}
			else if (i == places[j].row_) {
				ans.push_back(places[j].v_);
				++j;
			}
			else {
				--i;
				++j;
			}
		}
		else {
			ans.push_back(0);
		}
	}
	return ans;
}

/* 生成训练数据集 */
void DataCreate(PetriNet& tree, const int num_place) {
	std::ofstream file(output_path_py);
	bool begin = true;
	for (auto node : tree.goal_nodes_) {
		if (!begin) {
			file << '\n';
		}
		else
			begin = false;
		auto vec = ToVector<int>(node, num_place);
		for (auto v : vec) {
			file << v << '\t';
		}
		file << node->h_;
	}
	for (auto nodes : tree.entire_list) {
		for (auto node : nodes.second) {
			if (node->h_ > 99999)
				continue;
			if (!begin) {
				file << '\n';
			}
			else
				begin = false;
			auto vec = ToVector<int>(node, num_place);
			for (auto v : vec) {
				file << v << '\t';
			}
			file << node->h_;
			total_nodes_num++;
		}
	}
	file.close();
}

/* 信息输出 */
void InfoCreate(PetriNet& tree)
{
	std::ofstream file(Info_path, std::ios::out);
	if (!file.is_open())
	{
		std::cout << "File open error!" << std::endl;
		exit(-1);
	}
	else
	{
		file << "Total nodes = " << total_nodes_num + tree.goal_nodes_.size() << '\t';
		file << "Target nodes  = " << tree.goal_nodes_.size() << '\n';
		file << "The minimum time = " << tree.root_->h_ << "s" << '\n';
		file << "Node size = " << sizeof(Node);
	}
	for (auto &goal_ : tree.goal_nodes_)
		std::cout << "Mark：" << goal_->to_string() << "  " << "g = " << goal_->g_ << std::endl;
	file.close();
}
