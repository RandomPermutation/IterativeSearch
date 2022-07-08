#include "../include/graph.h"
#include "../include/traverser.h"

#include <fstream>;
using std::ofstream;
using std::ifstream;
#include <algorithm>
using std::min_element;
#include <iostream>
using std::cout;
using std::endl;
TrailCount& TrailCount::operator+=(const TrailCount& rhs) {
	TrailCount tc = rhs;
	for (auto it = rhs.trail_count.begin(); it != rhs.trail_count.end(); it++) {
		auto ret = trail_count.insert(*it);
		if (ret.second == false)
			ret.first->second += it->second;
	}
	total_cost += rhs.total_cost;
	return *this;
}
TrailCount TrailCount::operator*(const TrailCount& rhs)const {
	TrailCount tc;
	for (auto it1 = trail_count.begin(); it1 != trail_count.end(); it1++) {
		for (auto it2 = rhs.trail_count.begin(); it2 != rhs.trail_count.end(); it2++) {
			Cost cost = it1->first * it2->first;
			long long count = it1->second * it2->second;
			auto ret = tc.trail_count.insert(make_pair(cost, count));
			if (ret.second == false)
				ret.first->second += count;
		}
	}
	tc.total_cost = total_cost * rhs.total_cost;
	return tc;
}
bool TrailCount::operator<(const TrailCount& rhs)const {
	return total_cost < rhs.total_cost;
}
bool TrailCount::operator>(const TrailCount& rhs)const {
	return total_cost > rhs.total_cost;
}
ostream& operator<<(ostream& out, const TrailCount& obj) {
	if (obj.trail_count.empty()) { out << "empty\n"; return out; }
	out << "total cost=\n" << cost2weight(obj.total_cost) << "\n";
	out << "num of trails=\n";
	Cost sum;
	for (auto it = obj.trail_count.begin(); it != obj.trail_count.end(); it++) {
		out << cost2weight(it->first) << "\t" << std::dec << (it->second) << "\n";
		sum += it->first * it->second;
	}
	out << "recomputed total cost=" << cost2weight(sum) << "\n";
	return out;
}


void self_best(Graph& lhs, const Graph& rhs) {
	Graph new_table;
	for (auto it1 = lhs.hash_table.begin(); it1 != lhs.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			auto it3 = rhs.hash_table.find(it2->first);
			if (it3 == rhs.hash_table.end()) continue;
			for (auto it4 = it3->second.hash_table.begin(); it4 != it3->second.hash_table.end(); it4++) {
				for (auto it5 = it2->second.hash_table.begin(); it5 != it2->second.hash_table.end(); it5++) {
					for (auto it6 = it4->second.hash_table.begin(); it6 != it4->second.hash_table.end(); it6++) {
						int shift = (it5->first + it6->first) % SBOX_NUM;
						new_table.insert(it1->first, it4->first, shift, it5->second * it6->second);
					}
				}
			}
		}
	}
	lhs.hash_table.swap(new_table.hash_table);
}

void generate(Graph& graph, int max_an,const SPLayer& spl_fw,const SPLayer& spl_bw) {
	for (int an = 1; an <= max_an; an++) {
		State si;
		for (InputTraverser it(si, an); !it.end(); it.next()) {
			State s_bp = spl_bw.p_lookup(si);
			int an_bp = s_bp.get_an();
			if (an_bp > max_an) continue;
			//if (an_bp > max_an || an_bp < an) continue;
			int si_shift = TO_REPRE(si);
			State so;
			for (OutputTraverser ot(so, 0, WEIGHT_INF, si, spl_fw); !ot.end(); ot.next()) {
				if (ot.output_state.get_an() <= max_an) {
					int so_shift = TO_REPRE(so);
					int d = (so_shift - si_shift);
					if (d < 0) d += SBOX_NUM;
					graph.insert(si << si_shift, so << so_shift, d, ot.get_cost());
				}
			}
		}
	}
}


void printGraphToFile(const Graph& graph, const string& filename_base) {
	ofstream out;
	out.open(filename_base + "graph.txt");
	for (auto it1 = graph.hash_table.begin(); it1 != graph.hash_table.end(); it1++) {
		vector<int> indice_x(it1->first.get_ai());
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			vector<int> indice_y(it2->first.get_ai());
			out << "si=\t";
			out << indice_x.size() << "\t";
			for (int i = 0; i < indice_x.size(); i++) out << indice_x[i] << "\t" << (int)it1->first.get_word8(indice_x[i]) << "\t";
			out << "so=\t";
			out << indice_y.size() << "\t";
			for (int i = 0; i < indice_y.size(); i++) out << indice_y[i] << "\t" << (int)it2->first.get_word8(indice_y[i]) << "\t";
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				out <<"dist=\t"<< it3->first << "\t";
				//out << "weight=\t" << cost2weight(it3->second) << "\t";
				out << "weight=\t" << cost2weight(it3->second) << "\t";
				out << "sign=\t" << (it3->second > 0 ? 0 : 1) << "\n";
			}
		}
		out << "\n";
	}
	out.close();
}
bool readGraphFromFile(Graph& graph, const string& filename_base) {
	ifstream infile;
	infile.open(filename_base + "graph.txt");
	if (!infile.is_open())
		return false;
	State si, so;
	int dist;
	Weight weight;
	bool sign;
	string circum;
	infile >> circum;
	while (!infile.eof()) {
		if (circum == "si=") {
			si.set0();
			int an; infile >> an;
			for (int i = 0; i < an; i++) {
				int pos; infile >> pos;
				int val; infile >> val;
				si.set(pos, val);
			}
		}
		else if (circum == "so=") {
			so.set0();
			int an; infile >> an;
			for (int i = 0; i < an; i++) {
				int pos; infile >> pos;
				int val; infile >> val;
				so.set(pos, val);
			}
		}
		else if (circum == "dist=") {
			infile >> dist;
		}
		else if (circum == "weight=") {
			infile >> weight;
		}
		else if (circum == "sign=") {
			infile >> sign;
			graph.insert(si, so, dist, weight2cost(weight, sign));
		}
		infile >> circum;
	}
	return true;
}


void find_the_best_iterative_trail(const Graph& graph,const string& filename_base) {
	AdjacencyStructure refined_graph;
	CircuitsComputer circuits_searcher(refined_graph);
	map<int, State> mark_state_map;
	map<State, int> state_mark_map;

	set<State> vertices;
	for (auto it_si = graph.hash_table.begin(); it_si != graph.hash_table.end(); it_si++) {
		vertices.insert(it_si->first);
		for (auto it_so = it_si->second.hash_table.begin(); it_so != it_si->second.hash_table.end(); it_so++) {
			vertices.insert(it_so->first);
		}
	}
	int i = 0;
	for (auto it = vertices.begin(); it != vertices.end(); it++, i++) {
		state_mark_map.insert(make_pair(*it, i));
		mark_state_map.insert(make_pair(i, *it));
	}
	for (auto it_si = graph.hash_table.begin(); it_si != graph.hash_table.end(); it_si++) {
		for (auto it_so = it_si->second.hash_table.begin(); it_so != it_si->second.hash_table.end(); it_so++) {
			Cost max_cost = amp(weight2cost(WEIGHT_INF, 1));
			for (auto it_d = it_so->second.hash_table.begin(); it_d != it_so->second.hash_table.end(); it_d++) {
				if (it_d->second > max_cost) max_cost = it_d->second;
			}
			refined_graph.insert(state_mark_map.find(it_si->first)->second, state_mark_map.find(it_so->first)->second, cost2weight(max_cost));
		}
	}

	circuits_searcher.compute();

	ofstream out;
	out.open(filename_base + "iterative_trail.txt");
	i = 0;
	for (auto it1 = circuits_searcher.top_circuits.begin(); it1 != circuits_searcher.top_circuits.end(); it1++, i++) {
		out << "No." << std::dec << i << "\tweight=" << it1->first << "\n";
		for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
			out << (StateForPrint)mark_state_map.find(*it2)->second << "\n";
		}
		out << "\n";
	}
	out.close();

	ofstream outfile;
	outfile.open(filename_base + "iterative_structure.gv");
	StrongComponentsComputer scc(refined_graph);
	scc.compute_ge(0);
	outfile << "digraph G{\n";
	outfile << std::dec;
	for (auto it = refined_graph.adj_list_map.begin(); it != refined_graph.adj_list_map.end(); it++) {
		outfile << std::dec << it->first;
		outfile << " [label=\"" << (StateForPrint)mark_state_map.find(it->first)->second;
		outfile << "\"]";
		if (scc.in_components(it->first)) {
			outfile << " [color=red];\n";
		}
		else {
			outfile << " [color=blue];\n";
		}
	}
	for (auto it1 = refined_graph.adj_list_map.begin(); it1 != refined_graph.adj_list_map.end(); it1++) {
		for (auto it2 = it1->second.adj_list.begin(); it2 != it1->second.adj_list.end(); it2++) {
			outfile << std::dec << it1->first << " -> " << it2->first;
			outfile << " [label=\"" << std::dec;
			auto it3 = graph.hash_table.find(mark_state_map.find(it1->first)->second)->second.hash_table.find(mark_state_map.find(it2->first)->second);
			for (auto it4 = it3->second.hash_table.begin(); it4 != it3->second.hash_table.end(); ) {
				if (SYMMETRY == ROTATIONAL) outfile << "d=" << it4->first << ":";
				outfile << "w=" << cost2weight(it4->second);
				if (++it4 != it3->second.hash_table.end()) outfile << "\\n";
			}
			outfile << "\"]";
			if (scc.in_same_component(it1->first, it2->first)) {
				outfile << " [color=red];";
			}
			else {
				outfile << " [color=blue];";
			}
			outfile << "\n";
		}
	}
	outfile << "}";
	outfile.close();
}


BestAnswer get_best_iterative(const Graph& graph) {
	BestAnswer ba;
	for (auto it1 = graph.hash_table.begin(); it1 != graph.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			if (!(it1->first == it2->first)) continue;
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				ba.insert(BestAnswer(it1->first, it2->first, it3->first, it3->second));
			}
		}
	}
	return ba;
}
BestAnswer get_best(const Graph& graph) {
	BestAnswer ba;
	for (auto it1 = graph.hash_table.begin(); it1 != graph.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				ba.insert(BestAnswer(it1->first, it2->first, it3->first, it3->second));
			}
		}
	}
	return ba;
}
void find_the_best_iterative_cluster(const Graph& graph, const string& filename_base) {
	ofstream out;
	out.open(filename_base + "iterative_cluster.txt");
	vector<Weight> bwpr(100, WEIGHT_INF);
	vector<Weight> bbwpr(100, WEIGHT_INF);
	Graph total = graph;
	int r = 1;
	for (; ; r++) {
		BestAnswer ba = get_best_iterative(total);
		Cost cost = ba.cost;
		Weight wpr;
		if (is_zero(cost)) {
			wpr = WEIGHT_INF;
		}
		else {
			wpr = cost2weight(cost) / r;
		}
		bbwpr[r - 1] = wpr;
		if (r >= 2) {
			bwpr[r - 1] = std::min(wpr, bwpr[r - 2]);
		}
		else if (r == 1) {
			bwpr[r - 1] = wpr;
		}
		if ((bwpr[r - 1] != WEIGHT_INF && bwpr[r - 1] * r > SBOX_NUM * SBOX_SIZE) || r > 60) break;
		self_add(total, graph);
	}
	for (int i = 0; i < r; i++) out << i + 1 << "\t" << bbwpr[i] << "\t" << bwpr[i] << std::endl;

	out << "[";
	for (int i = 0; i < r; i++) if (bbwpr[i] != WEIGHT_INF) if (bbwpr[i] * (i + 1) < SBOX_NUM * SBOX_SIZE) out << i + 1 << ",";
	out << "]" << std::endl;
	out << "[";
	for (int i = 0; i < r; i++) if (bbwpr[i] != WEIGHT_INF) if (bbwpr[i] * (i + 1) < SBOX_NUM * SBOX_SIZE) out << bbwpr[i] << ",";
	out << "]" << std::endl;
	out.close();
}

vector<Weight> gen_wb(int rd, Weight ws, Weight wrmin) {
	vector<Weight> wb(rd);
	wb[rd - 1] = ws;
	for (int i = rd - 2; i >= 0; i--) {
		wb[i] = wb[i + 1] - wrmin;
	}

	return wb;
}
void append(Graph& graph, const State& si, const State& so, const Cost& cost) {
	int __di = TO_REPRE(si);
	int __do = TO_REPRE(so);
	int d = __do - __di;
	if (d < 0) d += SBOX_NUM;
	graph.insert(si << __di, so << __do, d, cost);
}
void Extender::restore(int j,bool fw_flag) {
	if (fw_flag) {
		for (int i = j - 1; i >= 0; i--) {
			//if (i == j - 1) if (output_checker != nullptr && output_checker(_state[j])) return;
			append(graph_fw, _state[i], _state[i + 1], _cost[i]);
		}
	}
	else {
		for (int i = j - 1; i >= 0; i--) {
			//if (i == j - 1) if (input_checker != nullptr && input_checker(cipher.spl_fw.p_lookup(_state[j]))) return;
			append(graph_bw, spl_fw.p_lookup(_state[i + 1]), spl_fw.p_lookup(_state[i]), _cost[i]);
		}
	}
}
bool Extender::extend(int j, Weight ws, const vector<Weight>& wb, bool fw_flag) {
	int rd_r = wb.size() - j;
	if (rd_r < 0) return true;
	Weight w_r = wb[j - 1] - ws;
	bool bottom = false;
	for (OutputTraverser otc(_state[j], 0, w_r + 0.01, _state[j - 1], fw_flag ? spl_fw : spl_bw); !otc.end(); otc.next()) {
		_cost[j - 1] = otc.get_cost();
		bottom |= extend(j + 1, ws + cost2weight(_cost[j - 1]), wb, fw_flag);
		//if(bottom) restore(j, fw_flag);
		//if (fw_flag) append(graph_fw, _state[j], _state[j - 1], _cost[j - 1]);
		if (fw_flag) append(graph_fw, _state[j - 1], _state[j], _cost[j - 1]);
		else append(graph_bw, spl_fw.p_lookup(_state[j]), spl_fw.p_lookup(_state[j - 1]), _cost[j - 1]);
		//if (rd_r > 0) extend(j + 1, ws + cost2weight(_cost[j - 1]), wb, fw_flag);
	}
	return bottom;
}
void Extender::extend(int r_ext, Weight ws_ext) {
	vector<Weight> wb = gen_wb(r_ext, ws_ext, spl_fw.get_min_weight());
	graph_bw = graph;
	graph_fw = graph;
	for (auto it = graph.hash_table.begin(); it != graph.hash_table.end(); it++) {
		_state = vector<State>(r_ext + 1);
		_cost = vector<Cost>(r_ext);
		_state[0] = it->first;
		extend(1, 0, wb, true);

		_state = vector<State>(r_ext + 1);
		_cost = vector<Cost>(r_ext);
		_state[0] = spl_bw.p_lookup(it->first);
		extend(1, 0, wb, false);
	}
}
bool Extender::extend_constr(int j, Weight ws, const vector<Weight>& wb, bool fw_flag, const State& input, const State& output) {
	int rd_r = wb.size() - j;
	if (rd_r < 0) return true;
	Weight w_r = wb[j - 1] - ws;
	bool bottom = false;
	for (OutputTraverser otc(_state[j], 0, w_r + 0.01, _state[j - 1], fw_flag ? spl_fw : spl_bw); !otc.end(); otc.next()) {
		_cost[j - 1] = otc.get_cost();
		bottom |= extend_constr(j + 1, ws + cost2weight(_cost[j - 1]), wb, fw_flag, input, output);
		//if(bottom) restore(j, fw_flag);
		//if (fw_flag) append(graph_fw, _state[j], _state[j - 1], _cost[j - 1]);
		if (fw_flag) {
			if (_state[j] << TO_REPRE(_state[j]) == output) {
				for (int k = 1; k <= j; k++)
					append(graph_fw, _state[k - 1], _state[k], _cost[k - 1]);
			}
		}
		else {
			State x = spl_fw.p_lookup(_state[j]);
			if (x << TO_REPRE(x) == input) {
				for (int k = 1; k <= j; k++)
					append(graph_bw, spl_fw.p_lookup(_state[k]), spl_fw.p_lookup(_state[k - 1]), _cost[k - 1]);
			}
		}
		//if (rd_r > 0) extend(j + 1, ws + cost2weight(_cost[j - 1]), wb, fw_flag);
	}
	return bottom;
}
void Extender::extend_constr(int r_ext, Weight ws_ext, const State& input, const State& output) {
	vector<Weight> wb = gen_wb(r_ext, ws_ext, spl_fw.get_min_weight());
	graph_bw = graph;
	graph_fw = graph;
	for (auto it = graph.hash_table.begin(); it != graph.hash_table.end(); it++) {
		_state = vector<State>(r_ext + 1);
		_cost = vector<Cost>(r_ext);
		_state[0] = it->first;
		extend_constr(1, 0, wb, true, input, output);

		_state = vector<State>(r_ext + 1);
		_cost = vector<Cost>(r_ext);
		_state[0] = spl_bw.p_lookup(it->first);
		extend_constr(1, 0, wb, false, input, output);
	}
}

void flip(const Graph& graph, Graph& res) {
	for (auto it1 = graph.hash_table.begin(); it1 != graph.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				res.insert(it2->first, it1->first, it3->first, it3->second);
			}
		}
	}
}

#include <iostream>
BestAnswer find_the_best_distinguisher(const Graph& graph_sum, const Graph& graph_fw_sum, const Graph& graph_bw_sum) {
	BestAnswer ba;
	int count = 0;
	std::cout << graph_bw_sum.hash_table.size() << std::endl;
	for (auto it = graph_bw_sum.hash_table.begin(); it != graph_bw_sum.hash_table.end(); it++) {
		count++;
		if (count % 100 == 0) std::cout << std::dec << count << std::endl;
		Graph graph1;

		graph1.hash_table.insert(*it);
		self_add(graph1, graph_sum);

		self_add(graph1, graph_fw_sum);
		ba.insert(get_best(graph1));
	}
	return ba;
}

BestAnswer find_the_best_distinguisher2(const Graph& graph, const Graph& graph_fw, const Graph& graph_bw, int rd_e, int rd_m) {
	BestAnswer ba;
	int count = 0;
	std::cout << graph_bw.hash_table.size() << std::endl;
	for (auto it = graph_bw.hash_table.begin(); it != graph_bw.hash_table.end(); it++) {
		count++;
		if (count % 100 == 0) std::cout << std::dec << count << std::endl;
		Graph graph_sum;
		graph_sum.hash_table.insert(*it);
		for (int i = 1; i < rd_e; i++)
			self_add(graph_sum, graph_bw);
		for (int i = 0; i < rd_m; i++)
			self_add(graph_sum, graph);
		for (int i = 0; i < rd_e; i++)
			self_add(graph_sum, graph_fw);
		ba.insert(get_best(graph_sum));
	}
	return ba;
}

BestAnswer find_the_best_trail(const Graph& graph, const Graph& graph_fw, const Graph& graph_bw, int rd_e, int rd_m) {
	BestAnswer ba;
	int count = 0;
	std::cout << graph_bw.hash_table.size() << std::endl;
	for (auto it = graph_bw.hash_table.begin(); it != graph_bw.hash_table.end(); it++) {
		count++;
		if (count % 100 == 0) std::cout << std::dec << count << std::endl;
		Graph graph_sum;
		graph_sum.hash_table.insert(*it);
		for (int i = 1; i < rd_e; i++)
			self_best(graph_sum, graph_bw);
		for (int i = 0; i < rd_m; i++)
			self_best(graph_sum, graph);
		for (int i = 0; i < rd_e; i++)
			self_best(graph_sum, graph_fw);
		ba.insert(get_best(graph_sum));
	}
	return ba;
}

void find_the_best_distinguisher_detail(const Graph& graph, const Graph& graph_fw, const Graph& graph_bw, int rd_e, int rd_m, const State& input,const State& output, string filename) {
	std::cout << "regenerate" << std::endl;
	GraphDetailed gd_fw, gd_bw, gd;
	for (auto it1 = graph_bw.hash_table.begin(); it1 != graph_bw.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				gd_bw.insert(it1->first, it2->first, it3->first, TrailCount(it3->second));
			}
		}
	}
	for (auto it1 = graph_fw.hash_table.begin(); it1 != graph_fw.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				gd_fw.insert(it1->first, it2->first, it3->first, TrailCount(it3->second));
			}
		}
	}
	for (auto it1 = graph.hash_table.begin(); it1 != graph.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				gd.insert(it1->first, it2->first, it3->first, TrailCount(it3->second));
			}
		}
	}

	std::cout << "add up" << std::endl;
	GraphDetailed graph_sum;
	auto it1 = graph_bw.hash_table.find(input);
	for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
		for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
			graph_sum.insert(it1->first, it2->first, it3->first, TrailCount(it3->second));
		}
	}

	for (int i = 1; i < rd_e; i++)
		self_add(graph_sum, gd_bw);
	for (int i = 0; i < rd_m; i++)
		self_add(graph_sum, gd);
	for (int i = 0; i < rd_e; i++)
		self_add(graph_sum, gd_fw);
	
	
	ofstream outfile;
	outfile.open(filename + "detail.txt");
	outfile << "input = " << input << std::endl;
	outfile << "output = " << output << std::endl;
	GraphDetailed& graph_print = graph_sum;
	for (auto it1 = graph_print.hash_table.begin(); it1 != graph_print.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			if (!(it2->first == output)) continue;
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				outfile << "shift = " << it3->first << std::endl;
				outfile << "output = " << (output >> it3->first) << std::endl;
				outfile << it3->second << std::endl;
			}
		}
	}
	outfile.close();
}

void find_the_best_distinguisher_detail2(const Graph& graph, const Graph& graph_fw, const Graph& graph_bw, int rd_e, int rd_m, const State& input, const State& output, string filename) {
	std::cout << "regenerate" << std::endl;
	GraphDetailed gd_fw, gd_bw, gd;
	GraphDetailed gd_fw_sum, gd_bw_sum, graph_sum;
	for (auto it1 = graph_bw.hash_table.begin(); it1 != graph_bw.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				gd_bw.insert(it1->first, it2->first, it3->first, TrailCount(it3->second));
			}
		}
	}
	for (auto it1 = graph.hash_table.begin(); it1 != graph.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				gd.insert(it1->first, it2->first, it3->first, TrailCount(it3->second));
			}
		}
	}
	auto it_bw = gd_bw.hash_table.find(input);
	for (auto it2 = it_bw->second.hash_table.begin(); it2 != it_bw->second.hash_table.end(); it2++) {
		for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
			gd_bw_sum.insert(it_bw->first, it2->first, it3->first, TrailCount(it3->second));
		}
	}
	for (int i = 1; i < rd_e; i++)
		self_add(gd_bw_sum, gd_bw);
	for (int i = 0; i < rd_m; i++)
		self_add(gd_bw_sum, gd);

	for (auto it1 = graph_fw.hash_table.begin(); it1 != graph_fw.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				gd_fw.insert(it2->first, it1->first, it3->first, TrailCount(it3->second));
			}
		}
	}
	auto it_fw = gd_fw.hash_table.find(output);
	for (auto it2 = it_fw->second.hash_table.begin(); it2 != it_fw->second.hash_table.end(); it2++) {
		for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
			gd_fw_sum.insert(it_fw->first, it2->first, it3->first, TrailCount(it3->second));
		}
	}
	for (int i = 1; i < rd_e; i++)
		self_add(gd_fw_sum, gd_fw);

	for (auto it1 = gd_bw_sum.hash_table.begin(); it1 != gd_bw_sum.hash_table.end(); it1++) {
		//std::cout << it1->first << std::endl;
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			//std::cout << "\t" << it2->first << std::endl;
			for (auto it3 = gd_fw_sum.hash_table.begin(); it3 != gd_fw_sum.hash_table.end(); it3++) {
				//std::cout << "\t\t" << it3->first << std::endl;
				auto it4 = it3->second.hash_table.find(it2->first);
				if (it4 == it3->second.hash_table.end()) continue;
				//std::cout << "\t\t" << (it4 == it3->second.hash_table.end()) << std::endl;
				for (auto it5 = it2->second.hash_table.begin(); it5 != it2->second.hash_table.end(); it5++) {
					for (auto it6 = it4->second.hash_table.begin(); it6 != it4->second.hash_table.end(); it6++) {
						int shift = (it5->first + it6->first) % SBOX_NUM;
						graph_sum.add(it1->first, it3->first, shift, it5->second * it6->second);
					}
				}
			}
		}
	}

	ofstream outfile;
	outfile.open(filename + "detail.txt");
	outfile << "input = " << input << std::endl;
	outfile << "output = " << output << std::endl;
	GraphDetailed& graph_print = graph_sum;
	for (auto it1 = graph_print.hash_table.begin(); it1 != graph_print.hash_table.end(); it1++) {
		for (auto it2 = it1->second.hash_table.begin(); it2 != it1->second.hash_table.end(); it2++) {
			if (!(it2->first == output)) continue;
			for (auto it3 = it2->second.hash_table.begin(); it3 != it2->second.hash_table.end(); it3++) {
				outfile << "shift = " << it3->first << std::endl;
				outfile << "output = " << (output >> it3->first) << std::endl;
				outfile << it3->second << std::endl;
			}
		}
	}
	outfile.close();
}