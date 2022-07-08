#include "../include/circuits.h"
#include <algorithm>
using std::sort;
using std::binary_search;
#include <iostream>
using std::cout;
using std::endl;
ostream& operator<<(ostream& out, const AdjacencyList& obj) {
	for (auto it = obj.adj_list.begin(); it != obj.adj_list.end(); it++)
		out << "d=" << std::dec << it->first << "\n" << it->second << "\n";
	return out;
}
void AdjacencyStructure::insert(int si, int so, Weight w) {
	auto ret = adj_list_map.insert(make_pair(si, AdjacencyList(so, w)));
	if (ret.second == false) ret.first->second.insert(so, w);
}
Weight AdjacencyStructure::get_weight(const vector<int> vertice)const {
	Weight ws = 0;
	for (int i = 0; i < vertice.size(); i++)
		ws += get_weight(vertice[i], vertice[(i + 1) % vertice.size()]);
	return ws;
}
bool AdjacencyStructure::_reduce() {
	map<int, AdjacencyList>::iterator adj_list_map_it;
	map<int, Weight>::iterator adj_list_it;
	set<int> probed;
	bool changed = false;
	for (adj_list_map_it = adj_list_map.begin(); adj_list_map_it != adj_list_map.end();) {
		for (adj_list_it = adj_list_map_it->second.adj_list.begin(); adj_list_it != adj_list_map_it->second.adj_list.end();) {
			auto it_refind = adj_list_map.find(adj_list_it->first);
			if (it_refind == adj_list_map.end()) {
				adj_list_map_it->second.adj_list.erase(adj_list_it++);
				changed = true;
			}
			else {
				for (auto it = it_refind->second.adj_list.begin(); it != it_refind->second.adj_list.end(); it++) {
					probed.insert(it->first);
				}
				adj_list_it++;
			}
		}
		if (adj_list_map_it->second.adj_list.empty()) {
			adj_list_map.erase(adj_list_map_it++);
		}
		else {
			adj_list_map_it++;
		}
	}
	for (adj_list_map_it = adj_list_map.begin(); adj_list_map_it != adj_list_map.end();) {
		if (probed.find(adj_list_map_it->first) == probed.end()) {
			changed = true;
			adj_list_map.erase(adj_list_map_it++);
		}
		else {
			adj_list_map_it++;
		}
	}
	return changed;
}
ostream& operator<<(ostream& out, const AdjacencyStructure& obj) {
	for (auto it = obj.adj_list_map.begin(); it != obj.adj_list_map.end(); it++)
		out << "so=" << std::dec << it->first << "\n" << it->second << "\n";
	return out;
}

void StrongComponentsComputer::reset() {
	i = 0;
	n = adj_s.adj_list_map.size();
	lowlink = vector<int>(n + 1, 0);
	number = vector<int>(n + 1, 0);
	strong_components.clear();
}

void StrongComponentsComputer::compute_ge(int v) {
	least = v;
	reset();
	for (auto it_w = adj_s.adj_list_map.lower_bound(least); it_w != adj_s.adj_list_map.end(); it_w++) {
		if (number[it_w->first] == 0) {
			strong_connect_ge(it_w->first);
		}
	}
	sort(strong_components.begin(), strong_components.end(), [](const vector<int>& a, const vector<int>& b) {return a.front() < b.front(); });
}
void StrongComponentsComputer::strong_connect_ge(int v) {
	i++;
	vertex_stack.emplace_back(v);
	lowlink[v] = i;
	number[v] = i;
	auto it_v = adj_s.adj_list_map.find(v);
	if (it_v != adj_s.adj_list_map.end()) {
		for (auto it_w = it_v->second.adj_list.begin(); it_w != it_v->second.adj_list.end(); it_w++) {
			if (it_w->first == v) continue;
			if (it_w->first < least) continue;
			if (number[it_w->first] == 0) {
				strong_connect_ge(it_w->first);
				lowlink[v] = std::min(lowlink[v], lowlink[it_w->first]);
			}
			else if (number[it_w->first] < number[v]) {
				if (find(vertex_stack.begin(), vertex_stack.end(), it_w->first) != vertex_stack.end()) {
					lowlink[v] = std::min(lowlink[v], number[it_w->first]);
				}
			}
		}
	}
	if (lowlink[v] == number[v]) {
		cur_component.clear();
		while (!vertex_stack.empty() && (number[vertex_stack.back()] >= number[v])) {
			cur_component.emplace_back(vertex_stack.back());
			vertex_stack.pop_back();
		}
		bool valid = false;
		if (cur_component.size() > 1) valid = true;
		else if (cur_component.size() == 1) {
			auto it1 = adj_s.adj_list_map.find(cur_component.front());
			auto it2 = it1->second.adj_list.find(cur_component.front());
			if (it2 != it1->second.adj_list.end()) valid = true;
		}
		if (valid) {
			std::sort(cur_component.begin(), cur_component.end());
			strong_components.emplace_back(cur_component);
		}
	}
}
void StrongComponentsComputer::compute() {
	reset();
	for (auto it_w = adj_s.adj_list_map.begin(); it_w != adj_s.adj_list_map.end(); it_w++)
		if (number[it_w->first] == 0)
			strong_connect(it_w->first);
	sort(strong_components.begin(), strong_components.end(), [](const vector<int>& a, const vector<int>& b) {return a.front() < b.front(); });
}
void StrongComponentsComputer::strong_connect(int v) {
	i++;
	vertex_stack.emplace_back(v);
	lowlink[v] = i;
	number[v] = i;
	auto it_v = adj_s.adj_list_map.find(v);
	if (it_v != adj_s.adj_list_map.end()) {
		for (auto it_w = it_v->second.adj_list.begin(); it_w != it_v->second.adj_list.end(); it_w++) {
			if (it_w->first == v) continue;
			if (number[it_w->first] == 0) {
				strong_connect(it_w->first);
				lowlink[v] = std::min(lowlink[v], lowlink[it_w->first]);
			}
			else if (number[it_w->first] < number[v]) {
				if (find(vertex_stack.begin(), vertex_stack.end(), it_w->first) != vertex_stack.end()) {
					lowlink[v] = std::min(lowlink[v], number[it_w->first]);
				}
			}
		}
	}
	if (lowlink[v] == number[v]) {
		cur_component.clear();
		while (!vertex_stack.empty() && (number[vertex_stack.back()] >= number[v])) {
			cur_component.emplace_back(vertex_stack.back());
			vertex_stack.pop_back();
		}
		std::sort(cur_component.begin(), cur_component.end());
		strong_components.emplace_back(cur_component);
	}
}
bool StrongComponentsComputer::in_same_component(int a, int b)const {
	for (auto it1 = strong_components.begin(); it1 != strong_components.end(); it1++) {
		auto it2 = std::lower_bound(it1->begin(), it1->end(), a);
		if (it2 != it1->end() && *it2 == a) {
			if (std::binary_search(it1->begin(), it1->end(), b)) {
				return true;
			}
			break;
		}
	}
	return false;
}
bool StrongComponentsComputer::in_components(int a)const {
	for (auto it = strong_components.begin(); it != strong_components.end(); it++) {
		if (std::binary_search(it->begin(), it->end(), a)) {
			return true;
		}
	}
	return false;
}
ostream& operator<<(ostream& out, const StrongComponentsComputer& obj) {
	for (auto it1 = obj.strong_components.begin(); it1 != obj.strong_components.end(); it1++) {
		for (auto it2 = it1->begin(); it2 != it1->end(); it2++) {
			out << *it2 << " ";
		}
		out << "\n";
	}
	return out;
}

void CircuitsComputer::reset() {
	n = adj_s.adj_list_map.size();
	B = vector<set<int>>(n);
	blocked = vector<bool>(n, false);
	top_circuits.clear();
}
void CircuitsComputer::compute() {
	reset();
	k = 0;
	s = 0;
	while (s < n) {
		scc.compute_ge(s);
		if (!scc.strong_components.empty()) {
			gen_cur_as(scc.strong_components.front());
			s = scc.strong_components.front().front();
			for (auto it = scc.strong_components.front().begin(); it != scc.strong_components.front().end(); it++) {
				blocked[*it] = false;
				B[*it].clear();
			}
			circuit(s);
			s++;
		}
		else {
			s = n;
		}
	}
}
void CircuitsComputer::gen_cur_as(const vector<int>& component) {
	cur_as.adj_list_map.clear();
	for (auto it1 = component.begin(); it1 != component.end(); it1++) {
		auto it2 = adj_s.adj_list_map.find(*it1);
		for (auto it3 = it2->second.adj_list.begin(); it3 != it2->second.adj_list.end(); it3++) {
			if (std::binary_search(component.begin(), component.end(), it3->first)) {
				cur_as.insert(*it1, it3->first, it3->second);
			}
		}
	}
}

void CircuitsComputer::output() {
	Weight w = adj_s.get_weight(vertex_stack) / vertex_stack.size();
	//top_circuits.insert(make_pair(w, vertex_stack));
	if (top_circuits.size() < topn) {
		top_circuits.insert(make_pair(w, vertex_stack));
	}
	else {
		if (w < top_circuits.rbegin()->first || (w== top_circuits.rbegin()->first&&vertex_stack.size()< top_circuits.rbegin()->second.size())) {
			top_circuits.insert(make_pair(w, vertex_stack));
			top_circuits.erase(top_circuits.rbegin()->first);
		}
	}
}
void CircuitsComputer::unblock(int u) {
	blocked[u] = false;
	for (auto it = B[u].begin(); it != B[u].end();) {
		int w = *it;
		B[u].erase(it++);
		if (blocked[w]) unblock(w);
	}
}

bool CircuitsComputer::circuit(int v) {
	bool f(false);
	vertex_stack.emplace_back(v);
	blocked[v] = true;
	auto it_as_v = cur_as.adj_list_map.find(v);
	for (auto it_w = it_as_v->second.adj_list.begin(); it_w != it_as_v->second.adj_list.end(); it_w++) {
		if (it_w->first == s) {
			output();
			f = true;
		}
		else if (!blocked[it_w->first]) {
			if (vertex_stack.size() <= max_rd) {
				if (circuit(it_w->first)) {
					f = true;
				}
			}
		}
	}
	if (f) {
		unblock(v);
	}
	else {
		for (auto it_w = it_as_v->second.adj_list.begin(); it_w != it_as_v->second.adj_list.end(); it_w++) {
			B[it_w->first].insert(v);
		}
	}
	vertex_stack.pop_back();
	return f;
}

ostream& operator<<(ostream& out, const CircuitsComputer& obj) {
	int i = 0;
	for (auto it1 = obj.top_circuits.begin(); it1 != obj.top_circuits.end(); it1++, i++) {
		out << "No." << std::dec << i << "\n";
		out << it1->first << "\n";
		for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
			out << std::dec << *it2 << "\n";
		}
		out << "\n";
	}
	return out;
}
