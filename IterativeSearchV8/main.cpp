#include "include/config.h"
#include "include/spn.h"
#include "include/graph.h"
#include <iostream>
using namespace std;
#include <time.h>
#include <fstream>

int main() {

	clock_t start, end;
	ofstream outfile;
	bool read_success;
	BestAnswer ba;
	SPLayer spl_fw(SBOX, DIFFUSION);
	Sbox isbox = spl_fw.inverse();
	SPLayer spl_bw(isbox, DIFFUSION_INV);
	Graph graph;
	Graph graph_fw, graph_bw;
	ofstream cipher_outfile;
	cipher_outfile.open("cipher.txt");
	cipher_outfile << spl_fw << endl;

	int max_an = 3;
	int rd_ext = 2, rd = 16;
	Weight ws_ext = 6;

	string filename_base = "answer/" + cipher_name + "_" + table_type + "_" + std::to_string(max_an) + "_";
	string filename_ext = filename_base + "(" + std::to_string(rd_ext) + "," + std::to_string(ws_ext) + ")_";
	string filename_distinguisher = filename_ext + std::to_string(rd) + "_";
	State x, y, input, output;

	outfile.open(filename_distinguisher + "info.txt");
	read_success = readGraphFromFile(graph, filename_base);
	if (!read_success) {
		generate(graph, max_an, spl_fw, spl_bw);
		reduce(graph);
		printGraphToFile(graph, filename_base);
	}
	else {
		cout << "read success" << endl;
	}
	outfile << "iterative structure size = " << graph.hash_table.size() << endl;

#define FIND_BEST_ITERATIVE_TRAIL 1
#define FIND_BEST_ITERATIVE_CLUSTER 1
#define FIND_BEST_TRAIL 0
#define FIND_BEST_CLUSTER 1

#define EXTEND FIND_BEST_TRAIL || FIND_BEST_CLUSTER
#define PRE_ADD_UP FIND_BEST_CLUSTER
	
#if FIND_BEST_ITERATIVE_TRAIL
	//Note that if max_an is too big, stack will overflow.
	start = clock();
	find_the_best_iterative_trail(graph, filename_base);
	end = clock();
	outfile <<"find the best iterative trail, runtime = "<< (double)(end - start) / CLOCKS_PER_SEC << endl;
#endif

#if FIND_BEST_ITERATIVE_CLUSTER
	start = clock();
	find_the_best_iterative_cluster(graph,filename_base);
	end = clock();
	outfile << "find the best iterative cluster, runtime = " << (double)(end - start) / CLOCKS_PER_SEC << endl;
#endif

#if EXTEND
	read_success = readGraphFromFile(graph_bw, filename_ext + "bw_") && readGraphFromFile(graph_fw, filename_ext + "fw_");
	if (!read_success) {
		
		Extender cf(graph, graph_fw, graph_bw, spl_fw, spl_bw);
		start = clock();
		cf.extend(rd_ext, ws_ext);
		//cf.extend_constr(rd_ext, ws_ext, input, output);
		end = clock();
		outfile << "extend runtime = " << (double)(end - start) / CLOCKS_PER_SEC << endl;
		outfile << "size of s0 = " << graph_bw.hash_table.size() << endl;
		printGraphToFile(graph_fw, filename_ext + "fw_");
		printGraphToFile(graph_bw, filename_ext + "bw_");
	}
	else {
		cout << "read success" << endl;
	}
#endif
	
#if PRE_ADD_UP
	start = clock();
	Graph graph_fw_sum = graph_fw;
	for (int i = 1; i < rd_ext; i++)
		self_add(graph_fw_sum, graph_fw);
	end = clock();
	outfile << "forward runtime = " << (double)(end - start) / CLOCKS_PER_SEC << endl;

	start = clock();
	Graph graph_bw_sum = graph_bw;
	for (int i = 1; i < rd_ext; i++)
		self_add(graph_bw_sum, graph_bw);
	end = clock();
	outfile << "backward runtime = " << (double)(end - start) / CLOCKS_PER_SEC << endl;

	Graph graph_middle_sum = graph;
	start = clock();
	for (int i = 1; i < rd - rd_ext * 2; i++) self_add(graph_middle_sum, graph);
	end = clock();
	outfile << "middle runtime = " << (double)(end - start) / CLOCKS_PER_SEC << endl;
#endif

#if FIND_BEST_TRAIL
	start = clock();
	ba = find_the_best_trail(graph, graph_fw, graph_bw, rd_ext, rd - 2 * rd_ext);
	outfile << (ba.input) << endl;
	outfile << (ba.output >> ba.shift) << endl;
	outfile << (cost2weight(ba.cost)) << endl;
	end = clock();
	outfile << (double)(end - start) / CLOCKS_PER_SEC << endl;
#endif

	/*start = clock();
	ba = find_the_best_distinguisher2(graph, graph_fw, graph_bw, rd_ext, rd - 2 * rd_ext);
	cout << (ba.input) << endl;
	cout << (ba.output >> ba.shift) << endl;
	cout << (cost2weight(ba.cost)) << endl;
	end = clock();
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;*/
#if FIND_BEST_CLUSTER
	start = clock();
	ba=find_the_best_distinguisher(graph_middle_sum, graph_fw_sum, graph_bw_sum);
	outfile << "best distinguisher:" << endl;
	outfile << (ba.input) << endl;
	outfile << (ba.output) << endl;
	outfile << (ba.shift) << endl;
	outfile << (cost2weight(ba.cost)) << endl;
	outfile << "output before P:" << endl;
	outfile << (spl_bw.p_lookup(ba.output >> ba.shift)) << endl;
	end = clock();
	outfile << (double)(end - start) / CLOCKS_PER_SEC << endl;
#endif

#if FIND_BEST_CLUSTER_DETAIL
	start = clock();
	find_the_best_distinguisher_detail2(graph, graph_fw, graph_bw, rd_ext, rd - 2 * rd_ext, input, output, filename_distinguisher);
	end = clock();
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
#endif

	return 0;
}