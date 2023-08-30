#pragma once
/*******************************************************
 * @File name：REACHABLE_TREE_NODE_H_
 * @Funciton ：Create class Node, Place, NodePool
 * @Content  ：<1> Store the status of a single place
 *             <2> Vector state_ only stores the status
 *                 of places with a token
 *             <3> Create storage space for nodes in advance
 *                 API: fetch nodes and recycle nodes
 * @Update   ：2023/07/18 11:30
 ******************************************************/

#include <vector>
#include <memory>
#include <tuple>
#include <queue>
#include <string>
#include <unordered_map>

using std::vector;
using std::shared_ptr;

class Node;
class NodePool;

typedef shared_ptr<Node> ptrNode;

/* 存放节点中含有token数的单个库所的信息 */
class Place {
public:
	int8_t tokens_;
	int8_t row_;
	int16_t v_;
	//nums of tokens, id of place , waiting time of place
	Place(int8_t t, int8_t r, int16_t v) :tokens_(t), row_(r), v_(v) {};
};

/* 提前开辟一个节点池 可回收重复利用 */
class NodePool : public std::enable_shared_from_this<NodePool> {  // std::enable_shared_from_this<NodePool> 用于将this指针变成智能指针类型
public:
	int maxNode;  // 最大节点数
	std::queue<ptrNode> buf;    // 单端队列存储待利用节点
	NodePool(int n = 10000000);
	ptrNode GetNode();   // 获取节点；若buf为空，则拓展空间
	void Recycling(ptrNode node);  // 回收旧节点，并初始化
private:
	NodePool() = delete;
};

/* 节点类 */
class Node {
public:
	/* 记录当前取出来的节点标号 */
	static unsigned int num;
	/* 是否抛弃 */
	bool discarded_;
	/* 是否死锁 */
	bool is_deadlock_;
	/* 是否在open表中 */
	bool is_open_;
	/* 初始->当前的代价 */
	unsigned short g_;
	/* 当前->终点的代价 */
	float h_;
	/* 子节点个数 */
	short sons_;
	/* 节点标号 */
	unsigned int id_;
	/* 节点池 */
	NodePool* pool_;
	/* Q */
	std::vector<std::pair<short, float>> Q_;
	/* 父节点（激发变迁、id、激发所需代价、父节点指针） */
	std::vector<std::tuple<unsigned short, unsigned int, unsigned int, ptrNode >> fathers;
	/* 标识（只含有token的place） */
	vector<Place> state_;
	Node(NodePool* pool) :discarded_(false), is_deadlock_(false), is_open_(false), g_(0), sons_(0), h_(FLT_MAX), id_(Node::num), pool_(pool) { ++Node::num; }

	/* 判断节点是否相同 */
	bool operator==(Node& n) {
		if (n.state_.size() != this->state_.size())
			return false;
		for (int i = 0; i < n.state_.size(); ++i) {
			if (n.state_[i].row_ != this->state_[i].row_ ||
				n.state_[i].tokens_ != this->state_[i].tokens_ ||
				n.state_[i].v_ != this->state_[i].v_)
				return false;
		}
		return true;
	}

	/* 标识计算 */
	ptrNode operator+(const vector<int>& v) {
		auto newnode = pool_->GetNode();
		int n = v.size();
		int j = 0;  // 当前标识的某个库所
		int i = 0;  // 处于关联矩阵的某个库所
		while (i <= state_.back().row_) {
			if (i < state_[j].row_) {
				if (v[i] > 0) {
					newnode->state_.emplace_back(Place(v[i], i, 0));
				}
			}
			else {
				if (v[i] + state_[j].tokens_ != 0)
					newnode->state_.emplace_back(Place(v[i] + state_[j].tokens_, i, state_[j].v_));
				++j;
			}
			++i;
		}
		while (i < n) {
			if (v[i] > 0) {
				newnode->state_.emplace_back(Place(v[i], i, 0));
			}
			++i;
		}
		return newnode;
	}

	/* 变迁使能判断、目标判断 */
	bool operator>(const vector<int>& obj) {
		int m = state_.size();
		int n = obj.size();
		int i = 0, j = 0;
		/* state_中最大库所之内部分 */
		while (i < m && j < n) {
			if (state_[i].row_ < j) {
				++i;
			}
			else if (state_[i].row_ == j) {
				if (state_[i].tokens_ < obj[j])
					return false;
				++i; ++j;
			}
			else if (state_[i].row_ > j && obj[j] == 0) {
				++j;
			}
			else {
				return false;
			}
		}
		/* 剩余部分 */
		while (j < n) {
			if (obj[j] > 0)
				return false;
			++j;
		}
		return true;
	}

	/* 查找指定库所的place */
	Place operator[](int i) {
		Place temp(0, -1, 0);
		for (auto p : state_) {
			if (p.row_ < i)
				continue;
			else if (p.row_ == i)
				return p;
			else
				return temp;
		}
		return temp;
	}

	/* 标识转换为字符串 */
	std::string to_string() {
		std::string str;
		for (auto p : state_) {
			str.append(std::to_string(p.row_));
			str += '/';
			str.append(std::to_string(p.tokens_));
			str += '/';
		}
		return str;
	}

private:
	Node() = delete;
};