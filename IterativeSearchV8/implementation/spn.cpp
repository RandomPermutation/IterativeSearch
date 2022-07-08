#include "../include/spn.h"
Sbox Sbox::inverse()const {
	Sbox isbox;
	for (int i = 0; i < SBOX_CARD; i++)
		isbox.sbox[sbox[i]] = i;
	return isbox;
}
ostream& operator<<(ostream& out, const Sbox& obj) {
	out << "size:" << SBOX_SIZE << "\n";
	for (int i = 0; i < SBOX_CARD; i++) out << "0x" << std::hex << (int)obj.get_sbox(i) << " ";
	return out;
}
Weight cost2weight(Cost cost) {
	if (is_zero(cost)) return WEIGHT_INF;
	return -log(abs(cost)) / log(2.0);
}
Cost weight2cost(Weight weight,bool sign) {
	if (weight == WEIGHT_INF) return 0;
	Cost cost = pow(2, -weight);
	return sign == 0 ? cost : -cost;
}
bool is_zero(Cost cost) {
	return abs(cost) < 1e-300;
}
bool is_inf(Weight weight) {
	return weight == WEIGHT_INF;
}
Cost amp(Cost cost) {
	return abs(cost);
}
void DistributionTable::gen_table() {
#if TABLE == DDT
	gen_ddt();
#elif TABLE == LAT
	gen_lat();
#elif TABLE == LAT2
	gen_lat2();
#endif
}
void DistributionTable::gen_ddt() {
	for (int dx = 0; dx < SBOX_CARD; dx++)
		for (int dy = 0; dy < SBOX_CARD; dy++)
			count_table[dx][dy] = 0;
	for (int x = 0; x < SBOX_CARD; x++)
		for (int d = 0; d < SBOX_CARD; d++)
			count_table[d][get_sbox(x) ^ get_sbox(x ^ d)]++;

	for (int x = 0; x < SBOX_CARD; x++)
		for (int y = 0; y < SBOX_CARD; y++) {
			if (x == 0 && y == 0) {
				weight_table[x][y] = 0;
				sign_table[x][y] = 1;
			}
			else if (count_table[x][y] == 0) {
				weight_table[x][y] = WEIGHT_INF;
				sign_table[x][y] = 0;
			}
			else {
				weight_table[x][y] = (Weight)SBOX_SIZE - (Weight)(log((Weight)count_table[x][y]) / log(2.0));
				sign_table[x][y] = 1;
			}
		}
}
static int hammingWeight(int z) {
	int i = (int)z;
	i = (i & 0x55555555) + ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	i = (i & 0x0F0F0F0F) + ((i >> 4) & 0x0F0F0F0F);
	i = (i * (0x01010101) >> 24);
	return i;
}
void DistributionTable::gen_lat() {
	for (int in = 0; in < SBOX_CARD; in++)
		for (int out = 0; out < SBOX_CARD; out++)
			for (int x = 0; x < SBOX_CARD; x++)
				if ((hammingWeight(x & in) + hammingWeight(get_sbox(x) & out)) % 2 == 0) count_table[in][out]++;
	for (int in = 0; in < SBOX_CARD; in++)
		for (int out = 0; out < SBOX_CARD; out++) {
			if (count_table[in][out] > SBOX_CARD / 2) {
				count_table[in][out] = count_table[in][out] - SBOX_CARD / 2;
				sign_table[in][out] = 1;
			}
			else if (count_table[in][out] < SBOX_CARD / 2) {
				count_table[in][out] = SBOX_CARD / 2 - count_table[in][out];
				sign_table[in][out] = -1;
			}
			else {
				count_table[in][out] = 0;
				sign_table[in][out] = 0;
			}
		}
	for (int x = 0; x < SBOX_CARD; x++)
		for (int y = 0; y < SBOX_CARD; y++) {
			if (x == 0 && y == 0) {
				weight_table[x][y] = (Weight)SBOX_SIZE - 1 - (Weight)(log((Weight)count_table[x][y]) / log(2.0));
			}
			else if (count_table[x][y] == 0) {
				weight_table[x][y] = WEIGHT_INF;
			}
			else {
				weight_table[x][y] = (Weight)SBOX_SIZE - 1 - (Weight)(log((Weight)count_table[x][y]) / log(2.0));
			}
		}
}
void DistributionTable::gen_lat2() {
	gen_lat();
	for(int x = 0; x < SBOX_CARD; x++)
		for (int y = 0; y < SBOX_CARD; y++) {
			count_table[x][y] *= 2;
			weight_table[x][y] = weight_table[x][y] == WEIGHT_INF ? WEIGHT_INF : weight_table[x][y] * 2;
			sign_table[x][y] = sign_table[x][y] == -1 ? 1 : sign_table[x][y];
		}
}
void DistributionTable::gen_dlct() {
	for (int delta = 0; delta < SBOX_CARD; delta++) {
		for (int lambda = 0; lambda < SBOX_CARD; lambda++) {
			for (int x = 0; x < SBOX_CARD; x++) {
				if (hammingWeight(lambda & (get_sbox(x) ^ get_sbox(x ^ delta))) % 2 == 0) count_table[delta][lambda]++;
			}
		}
	}
	for (int in = 0; in < SBOX_CARD; in++)
		for (int out = 0; out < SBOX_CARD; out++) {
			if (count_table[in][out] > SBOX_CARD / 2) {
				count_table[in][out] = count_table[in][out] - SBOX_CARD / 2;
				sign_table[in][out] = 1;
			}
			else if (count_table[in][out] < SBOX_CARD / 2) {
				count_table[in][out] = SBOX_CARD / 2 - count_table[in][out];
				sign_table[in][out] = -1;
			}
			else {
				count_table[in][out] = 0;
				sign_table[in][out] = 0;
			}
		}

	for (int x = 0; x < SBOX_CARD; x++)
		for (int y = 0; y < SBOX_CARD; y++) {
			if (x == 0 && y == 0) {
				weight_table[x][y] = (Weight)SBOX_SIZE - 1 - (Weight)(log((Weight)count_table[x][y]) / log(2.0));
			}
			else if (count_table[x][y] == 0) {
				weight_table[x][y] = WEIGHT_INF;
			}
			else {
				weight_table[x][y] = (Weight)SBOX_SIZE - 1 - (Weight)(log((Weight)count_table[x][y]) / log(2.0));
			}
		}
}
void DistributionTable::gen_dltd() {
	for (int delta = 0; delta < SBOX_CARD; delta++) {
		for (int lambda = 0; lambda < SBOX_CARD; lambda++) {
			for (int x = 0; x < SBOX_CARD; x++) {
				if (hammingWeight(lambda & (get_sbox(x) ^ get_sbox(x ^ delta))) % 2 == 0) count_table[delta][lambda]++;
			}
		}
	}

	for (int in = 0; in < SBOX_CARD; in++)
		for (int out = 0; out < SBOX_CARD; out++) {
			sign_table[in][out] = 1;
		}

	for (int x = 0; x < SBOX_CARD; x++)
		for (int y = 0; y < SBOX_CARD; y++) {
			if (x == 0 && y == 0) {
				weight_table[x][y] = (Weight)SBOX_SIZE - (Weight)(log((Weight)count_table[x][y]) / log(2.0));
			}
			else if (count_table[x][y] == 0) {
				weight_table[x][y] = WEIGHT_INF;
			}
			else {
				weight_table[x][y] = (Weight)SBOX_SIZE - (Weight)(log((Weight)count_table[x][y]) / log(2.0));
			}
		}
}
Cost DistributionTable::get_cost(uint8_t x, uint8_t y)const {
	return weight2cost(weight_table[x][y], sign_table[x][y]);
}
ostream& operator<<(ostream& out, const DistributionTable& obj) {
	out << "分布表：" << "\n";
	out << std::dec;
	for (int i = 0; i < SBOX_CARD; i++) { for (int j = 0; j < SBOX_CARD; j++) out << obj.count_table[i][j] << " "; out << "\n"; }
	out << "重量表：" << "\n";
	out << std::dec;
	for (int i = 0; i < SBOX_CARD; i++) {
		for (int j = 0; j < SBOX_CARD; j++)
			if (obj.weight_table[i][j] != WEIGHT_INF) out << obj.weight_table[i][j] << " ";
			else out << "- ";
		out << "\n";
	}
	out << "符号表：" << "\n";
	out << std::dec;
	for (int i = 0; i < SBOX_CARD; i++) { for (int j = 0; j < SBOX_CARD; j++) out << obj.sign_table[i][j] << " "; out << "\n"; }
	out << "概率表：" << "\n";
	for (int i = 0; i < SBOX_CARD; i++) {
		for (int j = 0; j < SBOX_CARD; j++)
			out << obj.get_cost(i, j) << "\t";
		out << "\n";
	}
	return out;
}
#include <set>
using std::set;
#include <algorithm>
using std::sort;

void AdvancedDistributionTable::gen() {
	set<Weight> w_set;
	for (int x = 0; x < SBOX_CARD; x++)
		for (int y = 0; y < SBOX_CARD; y++) {
			if (x == 0 && y == 0) continue;
			if (DistributionTable::get_weight(x, y) != WEIGHT_INF)
				w_set.insert(get_weight(x, y));
		}
	weight_vec.assign(w_set.begin(), w_set.end());
	for (int x = 0; x < SBOX_CARD; x++) {
		vector<vector<uint8_t>> word_vec_vec;
		for (int k = 0; k < weight_vec.size(); k++) {
			vector<uint8_t> word_vec;
			for (int y = 0; y < SBOX_CARD; y++) {
				if (get_weight(x, y) == weight_vec[k]) {
					word_vec.emplace_back(y);
				}
			}
			word_vec_vec.emplace_back(word_vec);
		}
		ordered_output_table.emplace_back(word_vec_vec);
	}
	for (int x = 0; x < ordered_output_table.size(); x++)
		for (int w = 0; w < ordered_output_table[x].size(); w++)
			sort(ordered_output_table[x][w].begin(), ordered_output_table[x][w].end(), [](uint8_t a, uint8_t b) {return hammingWeight(a) < hammingWeight(b); });
	for (int x = 0; x < ordered_output_table.size(); x++) {
		vector<vector<bool>> sign_vec_vec;
		for (int w = 0; w < ordered_output_table[x].size(); w++) {
			vector<bool> sign_vec;
			for (int i = 0; i < ordered_output_table[x][w].size(); i++) {
				sign_vec.emplace_back(get_sign(x, ordered_output_table[x][w][i]) == 1 ? 0 : 1);
			}
			sign_vec_vec.emplace_back(sign_vec);
		}
		ordered_sign_table.emplace_back(sign_vec_vec);
	}
}
ostream& operator<<(ostream& out, const AdvancedDistributionTable& obj) {
	out << "排序的分布表：" << "\n";
	for (int x = 0; x < obj.ordered_output_table.size(); x++) {
		out << "x:" << std::hex << x << " ";
		for (int w = 0; w < obj.ordered_output_table[x].size(); w++) {
			out << std::dec << w << "(" << obj.get_output_size(x, w) << "): " << std::hex;
			for (int y = 0; y < obj.ordered_output_table[x][w].size(); y++)
				out << std::hex << (int)obj.get_output_at(x, w, y) << "(" << obj.get_cost_at(x, w, y) << ") ";
			out << ";";
		}
		out << "\n";
	}
	out << "S盒分布表的weight：" << "\n";
	for (int i = 0; i < obj.get_weight_vec_size(); i++) out << obj.get_weight_vec(i) << " "; out << "\n";
	return out;
}

void LinearLayer::gen_p_table_lookup() {
	for (int x = 0; x < SBOX_CARD; x++) {
		for (int i = 0; i < SBOX_NUM; i++) {
			uint8_t input[SBOX_NUM] = { 0 };
			uint8_t output[SBOX_NUM] = { 0 };
			input[i] = x;
			linear_transform(input, output);
			p_table_lookup[x][i] = output;
		}
	}
}
State LinearLayer::p_lookup(const State& s)const {
	State s_out;
	vector<int> index = s.get_ai();
	for (int i = 0; i < index.size(); i++) {
		s_out ^= get_p_at(s.get_word8(index[i]), index[i]);
	}
	return s_out;
}

ostream& operator<<(ostream& out, const LinearLayer& obj) {
	for (int x = 0; x < SBOX_CARD; x++) {
		for (int i = 0; i < SBOX_NUM; i++) {
			out << "[" << std::hex << x << "][" << std::dec << i << "]:\t" << obj.p_table_lookup[x][i] << "\n";
		}
	}
	return out;
}

void SPLayer::gen_table_lookup() {
	for (int sbox_input = 0; sbox_input < SBOX_CARD; sbox_input++) {
		for (int sbox_index = 0; sbox_index < SBOX_NUM; sbox_index++) {
			for (int weight_index = 0; weight_index < get_weight_vec_size(); weight_index++) {
				for (int output_offset = 0; output_offset < get_output_size(sbox_input, weight_index); output_offset++) {
					sp_table_lookup[sbox_input][sbox_index][weight_index].emplace_back(get_p_at(get_output_at(sbox_input, weight_index, output_offset), sbox_index));
					State s;
					s.set(sbox_index, get_output_at(sbox_input, weight_index, output_offset));
					s_table_lookup[sbox_input][sbox_index][weight_index].emplace_back(s);
				}
			}
		}
	}
}

ostream& operator<<(ostream& out, const SPLayer& obj) {
	out << "SP表：" << "\n";
	for (int x = 0; x < SBOX_CARD; x++) {
		for (int i = 0; i < SBOX_NUM; i++) {
			out << std::hex << "input: " << x << std::dec << "\tindex: " << i << "\n";
			State si;
			si.set(i, x);
			for (int w = 0; w < obj.get_weight_vec_size(); w++) {
				out << std::dec << "#w(" << w << ")=" << obj.get_sp_size(x, i, w) << ": ";
				out << std::hex;
				for (int z = 0; z < obj.get_output_size(x, w); z++) out << (int)obj.get_output_at(x, w, z) << " ";
				for (int y = 0; y < obj.get_sp_size(x, i, w); y++) {
					out << "\n\t";
					out << "<" << si << ">";
					out << " S ";
					out << "<" << obj.get_s_at(x, i, w, y) << ">";
					out << " P ";
					out << "<" << obj.get_sp_at(x, i, w, y) << ">";
				}
				out << "\n";
			}
			out << "\n";
		}
	}
	return out;
}