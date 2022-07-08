#pragma once
#include "State.h"

class Sbox {
	uint8_t sbox[SBOX_CARD];
public:
	Sbox() {}
	Sbox(uint8_t* s) { memcpy(sbox, s, SBOX_CARD); }
	uint8_t get_sbox(uint8_t input)const { return sbox[input]; }
	Sbox inverse()const;
	friend ostream& operator<<(ostream& out, const Sbox& obj);
};

Cost weight2cost(Weight, bool);
Weight cost2weight(Cost);
bool is_zero(Cost);
Cost amp(Cost);
bool is_inf(Weight);
constexpr Weight WEIGHT_INF = 100000.0;

class DistributionTable
	:public Sbox {
	int count_table[SBOX_CARD][SBOX_CARD];
	Weight weight_table[SBOX_CARD][SBOX_CARD];
	int sign_table[SBOX_CARD][SBOX_CARD];
	void gen_ddt();
	void gen_lat();
	void gen_lat2();
	void gen_dlct();
	void gen_dltd();
	void gen_table();
public:
	DistributionTable(const Sbox& sbox) :Sbox(sbox), count_table{ 0 }, weight_table{ 0 }, sign_table{ 0 } {gen_table(); }
	Cost get_cost(uint8_t x, uint8_t y)const;
	Weight get_weight(uint8_t x, uint8_t y)const { return weight_table[x][y]; }
	int get_sign(uint8_t x, uint8_t y)const { return sign_table[x][y]; }
	friend ostream& operator<<(ostream& out, const DistributionTable& obj);
};

using WeightIndex = int;
using OutputOffset = int;
class AdvancedDistributionTable
	:public DistributionTable {
	vector<vector<vector<uint8_t>>> ordered_output_table;
	vector<vector<vector<bool>>> ordered_sign_table;
	vector<Weight> weight_vec;
	void gen();
public:
	AdvancedDistributionTable(const Sbox& sbox) :DistributionTable(sbox) { gen(); }
	size_t get_output_size(uint8_t input, size_t weight_index)const { return ordered_output_table[input][weight_index].size(); }
	uint8_t get_output_at(uint8_t input, WeightIndex weight_index, OutputOffset offset)const { return ordered_output_table[input][weight_index][offset]; }
	Cost get_cost_at(uint8_t input, WeightIndex weight_index, OutputOffset offset)const { return get_cost(input, get_output_at(input, weight_index, offset)); }
	bool get_sign_at(uint8_t input, WeightIndex weight_index, OutputOffset offset)const { return ordered_sign_table[input][weight_index][offset]; }
	Weight get_weight_at(uint8_t input, WeightIndex weight_index, OutputOffset offset)const { return get_weight(input, get_output_at(input, weight_index, offset)); }
	size_t get_weight_vec_size()const { return weight_vec.size(); }
	const vector<Weight>& get_weight_vec()const { return weight_vec; }
	Weight get_weight_vec(WeightIndex index)const { return weight_vec[index]; }
	Weight get_min_weight()const { return weight_vec.front(); }
	Weight get_max_weight()const { return weight_vec.back(); }
	friend ostream& operator<<(ostream& out, const AdvancedDistributionTable& obj);
};

#include <functional>
using std::function;
class LinearLayer {
	State p_table_lookup[SBOX_CARD][SBOX_NUM];
	function<void(uint8_t*,uint8_t*)> linear_transform;
	void gen_p_table_lookup();
public:
	LinearLayer(function<void(uint8_t*, uint8_t*)> f) :linear_transform(f) { gen_p_table_lookup(); }
	const State& get_p_at(uint8_t value, size_t index)const { return p_table_lookup[value][index]; }
	State p_lookup(const State& s)const;
	friend ostream& operator<<(ostream& out, const LinearLayer& obj);
};

class SPLayer
	:public AdvancedDistributionTable, public LinearLayer {
	vector<vector<vector<vector<State>>>> sp_table_lookup;
	vector<vector<vector<vector<State>>>> s_table_lookup;
	void gen_table_lookup();
public:
	SPLayer(const Sbox& sbox, function<void(uint8_t*, uint8_t*)> f) :
		AdvancedDistributionTable(sbox), LinearLayer(f)
		, sp_table_lookup(SBOX_CARD, vector<vector<vector<State>>>(SBOX_NUM, vector<vector<State>>(get_weight_vec_size())))
		, s_table_lookup(SBOX_CARD, vector<vector<vector<State>>>(SBOX_NUM, vector<vector<State>>(get_weight_vec_size())))
	{
		gen_table_lookup();
	}
	size_t get_sp_size(int input, int index, int weight)const { return sp_table_lookup[input][index][weight].size(); }
	const State& get_sp_at(int input, int index, int weight, int offset)const { return sp_table_lookup[input][index][weight][offset]; }
	size_t get_s_size(int input, int index, int weight)const { return s_table_lookup[input][index][weight].size(); }
	const State& get_s_at(int input, int index, int weight, int offset)const { return s_table_lookup[input][index][weight][offset]; }
	friend ostream& operator<<(ostream& out, const SPLayer& obj);
};
