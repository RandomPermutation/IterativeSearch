#pragma once
#include "config.h"
#include <map>
using std::map;
using std::multimap;
using std::make_pair;
#include <ostream>
using std::ostream;
#include <vector>
using std::vector;
#include <set>
using std::set;


struct AdjacencyList {
	map<int, Weight> adj_list;
	AdjacencyList(int so, Weight w) { insert(so, w); }
	void insert(int so, Weight w) { adj_list.insert(make_pair(so, w)); }
	friend ostream& operator<<(ostream& out, const AdjacencyList& obj);
};

struct AdjacencyStructure {
private:
	bool _reduce();
public:
	map<int, AdjacencyList> adj_list_map;
	void insert(int si, int so, Weight w);
	void reduce() { while (_reduce()) {} }
	Weight get_weight(int si, int so)const { return adj_list_map.find(si)->second.adj_list.find(so)->second; }
	Weight get_weight(const vector<int> vertice)const;
	friend ostream& operator<<(ostream& out, const AdjacencyStructure& obj);
};

class StrongComponentsComputer {
	int n;
	const AdjacencyStructure& adj_s;
	vector<int> cur_component;
	vector<int> vertex_stack;
	vector<int> lowlink;
	vector<int> number;
	int i;
	int least;
	void strong_connect_ge(int v);
	void strong_connect(int v);
	void reset();
public:
	vector<vector<int>> strong_components;
	StrongComponentsComputer(const AdjacencyStructure& as) :adj_s(as) {}
	void compute_ge(int v);
	void compute();
	bool in_same_component(int a, int b)const;
	bool in_components(int a)const;
	const vector<int>& get_component(int a)const;
	friend ostream& operator<<(ostream& out, const StrongComponentsComputer& obj);
};

class CircuitsComputer {
	const AdjacencyStructure& adj_s;
	int n;
	int s;
	mutable StrongComponentsComputer scc;
	AdjacencyStructure cur_as;
	vector<set<int>> B;
	vector<bool> blocked;
	vector<int> vertex_stack;
	int k;
	int max_rd;
	int topn;
	void gen_cur_as(const vector<int>& component);
	void unblock(int u);
	bool circuit(int v);
	void output();
	void reset();
public:
	multimap<Weight, vector<int>> top_circuits;
	CircuitsComputer(const AdjacencyStructure& as) :adj_s(as), n(as.adj_list_map.size()), scc(as), topn(10), max_rd(10) {}
	void compute();
	void set_max_rd(int r) { max_rd = r; }
	void set_topn(int n) { topn = n; }
	friend ostream& operator<<(ostream& out, const CircuitsComputer& obj);
};