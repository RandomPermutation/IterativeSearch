#pragma once
#include "HashTable.h"
#include "spn.h"
#include "circuits.h"


struct TrailCount {
	map<Cost, long long> trail_count;
	Cost total_cost;
	TrailCount() {}
	TrailCount(Cost c) { insert(c); }
	TrailCount& operator+=(const TrailCount& rhs);
	TrailCount operator*(const TrailCount& rhs)const;
	bool operator<(const TrailCount& rhs)const;
	bool operator>(const TrailCount& rhs)const;
	void insert(Cost c) { trail_count.insert(make_pair(c, 1)); total_cost = c; }
	friend ostream& operator<<(ostream& out, const TrailCount& obj);
};

using Graph = ThreeKeyHashTable<State, State, int, Cost>;
using GraphDetailed = ThreeKeyHashTable<State, State, int, TrailCount>;

template<class T>
void self_add(T& lhs, const T& rhs) {
	T new_table;
	for (auto it1 = lhs.hash_table.begin(); it1 != lhs.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			auto it3 = rhs.hash_table.find(it2->first);
			if (it3 == rhs.hash_table.end()) continue;
			for (auto it4 = it3->second.hash_table.begin(); it4 != it3->second.hash_table.end(); it4++) {
				for (auto it5 = it2->second.hash_table.begin(); it5 != it2->second.hash_table.end(); it5++) {
					for (auto it6 = it4->second.hash_table.begin(); it6 != it4->second.hash_table.end(); it6++) {
						int shift = (it5->first + it6->first) % SBOX_NUM;
						new_table.add(it1->first, it4->first, shift, it5->second * it6->second);
					}
				}
			}
		}
	}
	lhs.hash_table.swap(new_table.hash_table);
}


void generate(Graph& graph, int max_an, const SPLayer& spl_fw, const SPLayer& spl_bw);

#include <unordered_set>
using std::unordered_set;
template<class T>
bool _reduce(T& graph) {
	unordered_set<State> probed;
	bool changed = false;
	for (auto it1 = graph.hash_table.begin(); it1 != graph.hash_table.end();) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end();) {
			auto it_refind = graph.hash_table.find(it2->first);
			if (it_refind == graph.hash_table.end()) {
				it1->second.hash_table.erase(it2++);
				changed = true;
			}
			else {
				for (auto it3 = it_refind->second.hash_table.begin(); it3 != it_refind->second.hash_table.end(); it3++) {
					probed.insert(it3->first);
				}
				it2++;
			}
		}
		if (it1->second.hash_table.empty()) {
			graph.hash_table.erase(it1++);
		}
		else {
			it1++;
		}
	}
	for (auto it1 = graph.hash_table.begin(); it1 != graph.hash_table.end();) {
		if (probed.find(it1->first) == probed.end()) {
			changed = true;
			graph.hash_table.erase(it1++);
		}
		else {
			it1++;
		}
	}
	return changed;
}
template<class T>
void reduce(T& graph) {
	while (_reduce(graph)) {}
}

struct BestAnswer {
	State input;
	State output;
	int shift;
	Cost cost;
	BestAnswer() :cost(0) {}
	BestAnswer(const State& in, const State& out, int s, const Cost& c) :
		input(in), output(out), shift(s), cost(c) {}
	void insert(const BestAnswer& ba) {
		if (amp(ba.cost) > amp(cost)) {
			input = ba.input;
			output = ba.output;
			shift = ba.shift;
			cost = ba.cost;
		}
	}
};
BestAnswer get_best(const Graph& graph);

class Extender {
	const Graph& graph;
	const SPLayer& spl_fw;
	const SPLayer& spl_bw;
	vector<State> _state;
	vector<Cost> _cost;
	Graph& graph_fw;
	Graph& graph_bw;
	void restore(int j, bool fw_flag);
	bool extend(int j, Weight ws, const vector<Weight>& wb, bool fw_flag);
	bool extend_constr(int j, Weight ws, const vector<Weight>& wb, bool fw_flag, const State& input, const State& output);
public:
	Extender(const Graph& __graph, Graph& __graph_fw, Graph& __graph_bw, const SPLayer& __spl_fw, const SPLayer& __spl_bw) :
		graph(__graph), graph_fw(__graph_fw), graph_bw(__graph_bw), spl_fw(__spl_fw), spl_bw(__spl_bw) {}
	void extend(int r_ext, Weight ws);
	void extend_constr(int r_ext, Weight ws, const State& input, const State& output);
};

void printGraphToFile(const Graph& graph, const string& filename_base);
bool readGraphFromFile(Graph& graph, const string& filename_base);


void find_the_best_iterative_trail(const Graph& graph, const string& filename_base);
void find_the_best_iterative_cluster(const Graph& graph, const string& filename_base);

BestAnswer find_the_best_trail(const Graph& graph, const Graph& graph_fw, const Graph& graph_bw, int rd_e, int rd_m);
BestAnswer find_the_best_distinguisher(const Graph& graph_sum, const Graph& graph_fw_sum, const Graph& graph_bw_sum);
BestAnswer find_the_best_distinguisher2(const Graph& graph, const Graph& graph_fw, const Graph& graph_bw, int rd_e, int rd_m);
void find_the_best_distinguisher_detail(const Graph& graph, const Graph& graph_fw, const Graph& graph_bw, int rd_e, int rd_m, const State& input, const State& output, string filename);
void find_the_best_distinguisher_detail2(const Graph& graph, const Graph& graph_fw, const Graph& graph_bw, int rd_e, int rd_m, const State& input, const State& output, string filename);

