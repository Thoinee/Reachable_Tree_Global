#pragma once
/*******************************************************
 * @File name：REACHABLE_TREE_OUTPUT_H_
 * @Funciton ：Generate datasets and graph
 * @Content  ：Dataset, Reachable graph
 * @Update   ：2023/07/18 11:30
 ******************************************************/

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include <queue>
#include <unordered_set>
#include <iomanip>
#include <mutex>
#include "petri.h"
#include "input.h"

static std::mutex mtx;

/* 无需用到的标识库所和已等待时间库所 */
static std::unordered_set<int> ignore_m = { 12, 18, 34 }; // 省略目标库所
static std::unordered_set<int> ignore_v = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 17, 18, 33, 34 }; // 省略初始库所、资源库所、目标库所

static float max_accept_multiple = 999; // 可容许最大倍数

/* 节点计数器 */
static long tree_nodes_num = 0;  // 状态数
static long total_nodes_num = 0; // 数据量

/* 生成全局可达图 */
void globalGraphCreate(PetriNet& tree) {
	/* 加锁 */
	mtx.lock();
	/* 数据保留阈值 */
	auto threshold_value = max_accept_multiple * tree.root_->h_;
	/* 图节点输出流 */
	std::ofstream file(kOutputGvPath);
	file << "digraph {" << '\n';
	/* 设置节点 */
	for (auto& node: tree.goal_nodes_) {
		if (node->g_ + node->h_ <= threshold_value) {
			file << "\""
				 << "m" << node->id_
				 << "_g" << node->g_
				 << "_h" << node->h_
				 << "\""
				 << " [color=red style=filled]"
				 << "\n";
		}
	}
	for (auto& nodes: tree.entire_list_) {
		for (auto& node: nodes.second) {
			if (node->h_ < 9999 && node->g_ + node->h_ <= threshold_value) {
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
	for (auto& node: tree.goal_nodes_) {
		if (node->g_ + node->h_ <= threshold_value) {
			for (auto& father: node->fathers) {
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
	}
	for (auto& nodes: tree.entire_list_) {
		for (auto& node: nodes.second) {
			if (node->h_ < 9999 && node->g_ + node->h_ <= threshold_value) {
				for (auto& father: node->fathers) {
					auto f_node = std::get<3>(father);
					if (f_node->g_ + f_node->h_ <= threshold_value) {
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
			}
		}
	}
	file << "}";
	file.close();
	/* 解锁 */
	mtx.unlock();
}

/* 输出到达目标路径 */
void graphCreate(PetriNet& tree) {
	std::queue<ptrNode> back_node;
	std::unordered_set<unsigned short> already;
	short num_of_goals = 0;
	std::ofstream ofs(kOutputGvPath, std::ios::out);
	ofs << "digraph {" << '\n';
	for (auto& node: tree.goal_nodes_) {
		back_node.push(node);
	}
	while (!back_node.empty()){
		auto node = back_node.front();
		back_node.pop();
		if (num_of_goals < tree.goal_nodes_.size()) {
			num_of_goals++;
			ofs << "m" << node->id_
				<< "_g" << node->g_
				<< "_h" << node->h_ 
				<< " [color=red, style=filled]"
				<< "\n";
		}
		else 
			ofs << "m" << node->id_
				<< "_g" << node->g_
				<< "_h" << node->h_
				<< "\n";
		if (node->fathers.empty()) { continue; }
		auto f_node = std::get<3>(node->fathers[0]);
		ofs << "m" << f_node->id_ 
			<< "_g" << f_node->g_ 
			<< "_h" << f_node->h_
			<< " -> " 
			<< "m" << node->id_ 
			<< "_g" << node->g_ 
			<< "_h" << node->h_
			<< " [label=t" << std::get<0>(node->fathers[0]) 
			<< "]\n";
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
vector<T> toVector(const ptrNode node, const int num_place, const vector<int>& delay) {   // 节点指针  PetriNet库所总数
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
				++j;   // 计数值+1
			}
			else {     // 由于上面有continue，会出现无赋值且i自动加1，导致i位置空出；ignore_m中包含库所存在于state_中；
				--i;   // 所以利用--i跳到空位置；++j补上含有的place计数值，否则j无法==含有token的库所数
				++j;
			}
		}
		else {         // 若计数值=含有token的库所数，则剩余的库所全置0
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
	j = 0;

	/* 加入 v/d 信息 */
	for (int i = 0; i < num_place; ++i) {
		if (ignore_v.count(i) >= 1)
			continue;
		if (j < places.size()) {
			if (i < places[j].row_) {
				ans.push_back(0);
			}
			else if (i == places[j].row_) {
				ans.push_back(places[j].v_ / delay[i]);
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

template <typename T>
/* 生成m，x向量 */
std::pair<vector<T>, vector<T>> toVector(const ptrNode node, const int num_place) {   // 节点指针  PetriNet库所总数
	auto places = node->state_;
	int j = 0;   // 对已赋值的库所进行计数
	std::vector<T> ans_m, ans_x;
	for (int i = 0; i < num_place; ++i) {   //  遍历所有库所，进行赋值
		if (j < places.size()) {       // 计数值未到达需赋值的库所数
			if (i < places[j].row_) {  // 将state_中未记录的库所赋予0
				ans_m.push_back(0);
			}
			else if (i == places[j].row_) {      // 到达state_中包含的place，对token数进行赋值
				ans_m.push_back(places[j].tokens_);
				++j;                             // 计数值+1
			}
		}
		else {                       // 若计数值=含有token的库所数，则剩余的库所全置0
			ans_m.push_back(0);
		}
	}
	j = 0;

	/* 与上述同理 */
	for (int i = 0; i < num_place; ++i) {
		if (j < places.size()) {
			if (i < places[j].row_) {
				ans_x.push_back(0);
			}
			else if (i == places[j].row_) {
				ans_x.push_back(places[j].v_);
				++j;
			}
		}
		else {
			ans_x.push_back(0);
		}
	}

	return std::make_pair(ans_m, ans_x);
}

/* 生成Q向量 */
vector<float> QToVector(const ptrNode node, const int num_transition) {
	vector<float> ans(num_transition, 1);
	auto it = node->Q_;
	for (auto q: it) {
		ans[q.first] = -q.second;
	}
	return ans;
}

/* 生成训练数据集 */
void dataCreateTxt(PetriNet& tree, const int num_place) {
	/* 加锁 */
	mtx.lock();
	/* 数据保留阈值 */
	auto threshold_value = max_accept_multiple * tree.root_->h_;
	/* 数据集输出流 */
	std::ofstream file(kOutputTxtPath);
	/* 保留两位小数格式 */
	file << std::setiosflags(std::ios::fixed) << std::setprecision(2);

	bool begin = true;
	for (auto node: tree.goal_nodes_) {
		if (node->g_ + node->h_ > threshold_value)
			continue;
		if (!begin) {
			file << '\n';
		}
		else
			begin = false;
		auto vec = toVector<float>(node, num_place, tree.delay_);
		for (auto v: vec) {
			file << v << '\t';
		}
		file << node->h_;
		total_nodes_num++;
	}

	for (auto nodes: tree.entire_list_) {
		for (auto node: nodes.second) {
			if (node->h_ > 9999 || node->g_ + node->h_ > threshold_value)
				continue;
			if (!begin) {
				file << '\n';
			}
			else
				begin = false;
			auto vec = toVector<float>(node, num_place, tree.delay_);
			for (auto v: vec) {
				file << v << '\t';
			}
			file << node->h_;
			total_nodes_num++;
		}
	}

	file.close();
	/* 解锁 */
	mtx.unlock();
}

/* 分开生成csv文件 */
void dataCreateCsv(PetriNet& tree, const int num_place, const int num_transition) {
	/* 加锁 */
	mtx.lock();
	/* 数据保留阈值 */
	auto threshold_value = max_accept_multiple * tree.root_->h_;
	/* m.csv */
	std::ofstream file_m(kOutputCsvMPath);
	/* x.csv */
	std::ofstream file_x(kOutputCsvXPath);
	/* h.csv */
	std::ofstream file_h(kOutputCsvHPath);
	/* Q.csv */
	std::ofstream file_Q(kOutputCsvQPath);
	/* 所有节点 */
	vector<ptrNode> total_nodes;

	/* 存入待输出的节点 */
	total_nodes.insert(total_nodes.end(), tree.goal_nodes_.begin(), tree.goal_nodes_.end()); // 目标节点
	for (auto nodes: tree.entire_list_) {
		total_nodes.insert(total_nodes.end(), nodes.second.begin(), nodes.second.end()); // 路径节点
	}

	bool begin = true; // 每条数据之间的换行符
	for (auto node: total_nodes) {
		if (node->h_ > 9999 || node->h_ + node->g_ > threshold_value) 
			continue;
		if (!begin) {
			file_m << '\n';
			file_x << '\n';
			file_h << '\n';
			file_Q << '\n';
		}
		else begin = false;

		auto pair = toVector<int>(node, num_place);  // (m, x)
		auto Q_value = QToVector(node, num_transition);  // (Q_index, Q_value)

		/* 存入m */
		bool begin_in = true; // 数据项之间的分隔符
		for (auto v: pair.first) {
			if (!begin_in) {
				file_m << ',';
			}
			else begin_in = false;
			file_m << v;
		}

		/* 存入x */
		begin_in = true;
		for (auto v: pair.second) {
			if (!begin_in) {
				file_x << ',';
			}
			else begin_in = false;
			file_x << v;
		}

		begin_in = true;
		for (auto v: Q_value) {
			if (!begin_in) {
				file_Q << ',';
			}
			else begin_in = false;
			file_Q << v;
		}

		/* 存入h */
		file_h << node->h_;
	}

	file_m.close();
	file_x.close();
	file_h.close();
	file_Q.close();
	/* 解锁 */
	mtx.unlock();
}

/* 信息输出 */
void infoCreate(PetriNet& tree)
{
	/* 加锁 */
	mtx.lock();
	/* 信息文件输出流 */
	std::ofstream file(kInfoPath, std::ios::out);

	tree_nodes_num = tree.goal_nodes_.size();
	for (auto nodes : tree.entire_list_) {
		tree_nodes_num += nodes.second.size();
	}

	if (!file.is_open()) {
		std::cout << "File open error!" << std::endl;
		exit(-1);
	}
	else {
		file << "Total nodes = " << tree_nodes_num << '\n';
		file << "Target nodes  = " << tree.goal_nodes_.size() << '\n';
		file << "Dataset = " << total_nodes_num << '\n';
		file << "The minimum time = " << tree.root_->h_ << "s" << '\n';
		file << "Node size = " << sizeof(Node);
	}

	for (auto nodes: tree.entire_list_) {
		for (auto node : nodes.second) {
			if (node->sons_) {
				std::cout << "BackTree exception\n" << std::endl;
			}
		}
	}

	std::cout << "Goal nodes  = " << tree.goal_nodes_.size() << '\n';
	for (auto &goal_: tree.goal_nodes_) {
		std::cout << "Mark：" << goal_->toString() << "  " << "g = " << goal_->g_ << '\n';
	}

	file.close();
	/* 解锁 */
	mtx.unlock();
}
