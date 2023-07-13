#pragma once
#include "petri_net.h"
#include "data_input.h"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include <queue>
#include <unordered_set>

/* �����õ��ı�ʶ�������ѵȴ�ʱ����� */
static std::unordered_set<int> ignore_m = { 12,18,34 };
static std::unordered_set<int> ignore_v = { 0,1,2,3,4,5,6,7,8,9,10,11,12,17,18,33,34 };
/* �ڵ������ */
static long total_nodes_num = 0;

/* ����ȫ�ֿɴ�ͼ */
void GlobalGraphCreate(PetriNet& tree) {
	std::ofstream file(output_path_gv);
	file << "digraph {" << '\n';
	/* ���ýڵ� */
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

	/* ���ýڵ������ӻ� */
	/* ˼�룺�Ӹýڵ��ҵ��丸�ڵ� ���������� */
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

/* �������Ŀ��·�� */
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
/* ת��Ϊ���ݼ���ʽ */
vector<T> ToVector(const ptrNode node, const int num_place) {   // �ڵ�ָ��  PetriNet��������
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
				++j;                             // ����ֵ+1
			}
			else {                    // ����������continue��������޸�ֵ��i�Զ���1������iλ�ÿճ���ignore_m�а�������������state_�У�
				--i;                  // ��������--i������λ�ã�++j���Ϻ��е�place����ֵ������j�޷�==����token�Ŀ�����
				++j;
			}
		}
		else {                       // ������ֵ=����token�Ŀ���������ʣ��Ŀ���ȫ��0
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
	return ans;
}

/* ����ѵ�����ݼ� */
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

/* ��Ϣ��� */
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
		std::cout << "Mark��" << goal_->to_string() << "  " << "g = " << goal_->g_ << std::endl;
	file.close();
}
