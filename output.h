#pragma once
/*******************************************************
 * @File name��REACHABLE_TREE_OUTPUT_H_
 * @Funciton ��Generate datasets and graph
 * @Content  ��Dataset, Reachable graph
 * @Update   ��2023/07/18 11:30
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

/* �����õ��ı�ʶ�������ѵȴ�ʱ����� */
static std::unordered_set<int> ignore_m = { 12, 18, 34 }; // ʡ��Ŀ�����
static std::unordered_set<int> ignore_v = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 17, 18, 33, 34 }; // ʡ�Գ�ʼ��������Դ������Ŀ�����

static float max_accept_multiple = 999; // �����������

/* �ڵ������ */
static long tree_nodes_num = 0;  // ״̬��
static long total_nodes_num = 0; // ������

/* ����ȫ�ֿɴ�ͼ */
void globalGraphCreate(PetriNet& tree) {
	/* ���� */
	mtx.lock();
	/* ���ݱ�����ֵ */
	auto threshold_value = max_accept_multiple * tree.root_->h_;
	/* ͼ�ڵ������ */
	std::ofstream file(kOutputGvPath);
	file << "digraph {" << '\n';
	/* ���ýڵ� */
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

	/* ���ýڵ������ӻ� */
	/* ˼�룺�Ӹýڵ��ҵ��丸�ڵ� ���������� */
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
	/* ���� */
	mtx.unlock();
}

/* �������Ŀ��·�� */
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
/* ת��Ϊ���ݼ���ʽ */
vector<T> toVector(const ptrNode node, const int num_place, const vector<int>& delay) {   // �ڵ�ָ��  PetriNet��������
	auto places = node->state_;
	int j = 0;   // ���Ѹ�ֵ�Ŀ������м���
	std::vector<T> ans;
	for (int i = 0; i < num_place; ++i) {   //  �������п��������и�ֵ
		if (ignore_m.count(i) >= 1)    // ���鵽��ignore_m�д洢�����ÿ���������������ֵ
			continue;
		if (j < places.size()) {       // ����ֵδ�����踳ֵ�Ŀ�����
			if (i < places[j].row_) {  // ��state_��δ��¼�Ŀ�������0
				ans.push_back(0);
			}
			else if (i == places[j].row_) {      // ����state_�а�����place����token�����и�ֵ
				ans.push_back(places[j].tokens_);
				++j;   // ����ֵ+1
			}
			else {     // ����������continue��������޸�ֵ��i�Զ���1������iλ�ÿճ���ignore_m�а�������������state_�У�
				--i;   // ��������--i������λ�ã�++j���Ϻ��е�place����ֵ������j�޷�==����token�Ŀ�����
				++j;
			}
		}
		else {         // ������ֵ=����token�Ŀ���������ʣ��Ŀ���ȫ��0
			ans.push_back(0);
		}
	}
	j = 0;

	/* ������ͬ�� */
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

	/* ���� v/d ��Ϣ */
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
/* ����m��x���� */
std::pair<vector<T>, vector<T>> toVector(const ptrNode node, const int num_place) {   // �ڵ�ָ��  PetriNet��������
	auto places = node->state_;
	int j = 0;   // ���Ѹ�ֵ�Ŀ������м���
	std::vector<T> ans_m, ans_x;
	for (int i = 0; i < num_place; ++i) {   //  �������п��������и�ֵ
		if (j < places.size()) {       // ����ֵδ�����踳ֵ�Ŀ�����
			if (i < places[j].row_) {  // ��state_��δ��¼�Ŀ�������0
				ans_m.push_back(0);
			}
			else if (i == places[j].row_) {      // ����state_�а�����place����token�����и�ֵ
				ans_m.push_back(places[j].tokens_);
				++j;                             // ����ֵ+1
			}
		}
		else {                       // ������ֵ=����token�Ŀ���������ʣ��Ŀ���ȫ��0
			ans_m.push_back(0);
		}
	}
	j = 0;

	/* ������ͬ�� */
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

/* ����Q���� */
vector<float> QToVector(const ptrNode node, const int num_transition) {
	vector<float> ans(num_transition, 1);
	auto it = node->Q_;
	for (auto q: it) {
		ans[q.first] = -q.second;
	}
	return ans;
}

/* ����ѵ�����ݼ� */
void dataCreateTxt(PetriNet& tree, const int num_place) {
	/* ���� */
	mtx.lock();
	/* ���ݱ�����ֵ */
	auto threshold_value = max_accept_multiple * tree.root_->h_;
	/* ���ݼ������ */
	std::ofstream file(kOutputTxtPath);
	/* ������λС����ʽ */
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
	/* ���� */
	mtx.unlock();
}

/* �ֿ�����csv�ļ� */
void dataCreateCsv(PetriNet& tree, const int num_place, const int num_transition) {
	/* ���� */
	mtx.lock();
	/* ���ݱ�����ֵ */
	auto threshold_value = max_accept_multiple * tree.root_->h_;
	/* m.csv */
	std::ofstream file_m(kOutputCsvMPath);
	/* x.csv */
	std::ofstream file_x(kOutputCsvXPath);
	/* h.csv */
	std::ofstream file_h(kOutputCsvHPath);
	/* Q.csv */
	std::ofstream file_Q(kOutputCsvQPath);
	/* ���нڵ� */
	vector<ptrNode> total_nodes;

	/* ���������Ľڵ� */
	total_nodes.insert(total_nodes.end(), tree.goal_nodes_.begin(), tree.goal_nodes_.end()); // Ŀ��ڵ�
	for (auto nodes: tree.entire_list_) {
		total_nodes.insert(total_nodes.end(), nodes.second.begin(), nodes.second.end()); // ·���ڵ�
	}

	bool begin = true; // ÿ������֮��Ļ��з�
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

		/* ����m */
		bool begin_in = true; // ������֮��ķָ���
		for (auto v: pair.first) {
			if (!begin_in) {
				file_m << ',';
			}
			else begin_in = false;
			file_m << v;
		}

		/* ����x */
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

		/* ����h */
		file_h << node->h_;
	}

	file_m.close();
	file_x.close();
	file_h.close();
	file_Q.close();
	/* ���� */
	mtx.unlock();
}

/* ��Ϣ��� */
void infoCreate(PetriNet& tree)
{
	/* ���� */
	mtx.lock();
	/* ��Ϣ�ļ������ */
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
		std::cout << "Mark��" << goal_->toString() << "  " << "g = " << goal_->g_ << '\n';
	}

	file.close();
	/* ���� */
	mtx.unlock();
}
