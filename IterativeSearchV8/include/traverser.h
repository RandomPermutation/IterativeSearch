#pragma once
#include "looplessCode.h"
#include "patternGenerator.h"
#include "spn.h"
#include <memory>
using std::unique_ptr;

class InputTraverser {
	unique_ptr<ExchangeCode> active_ptr;
	unique_ptr<GrayCode> offset_ptr;
	State& state;
	bool _end_flag;
	int _an;
	State first_state;
	vector<int> num;
	void init_offset();
	void next_offset();
	void init_active_and_init_offset();
	void next_active_and_init_offset();
public:
	InputTraverser(State& s, int an) :
		state(s), _an(an), _end_flag(false), num(_an, SBOX_CARD - 1) {
		init_active_and_init_offset();
	}
	void next();
	bool end()const { return _end_flag; }
	friend ostream& operator<<(ostream& out, const InputTraverser& obj);
};

class OutputTraverser {
	const State& _input_state;
	vector<int> _input_state_ai;
	const SPLayer& _sp_table;
	bool _end_flag;
	bool _sign;
	Weight _wlb;
	Weight _wub;
	vector<int> gen_len();
	bool init_offset_n_state();
	bool init_weight();
	bool init_pattern();
	bool next_offset_n_state();
	bool next_weight();
	bool next_pattern();
public:
	State first_state;
	State& output_state;
	unique_ptr<PatternGeneratorFixingAn> pattern_ptr;
	unique_ptr<ExchangeGrayCode> weight_ptr;
	unique_ptr<GrayCode> offset_ptr;
	OutputTraverser(State& so, Weight wlb, Weight wub, const State& si, const SPLayer& sp);
	void next();
	bool end()const;
	Cost get_cost()const { return weight2cost(pattern_ptr->weight, _sign); }
	friend ostream& operator<<(ostream& out, const OutputTraverser& obj);
};

class OutputTraverser2 {
	const State& _input_state;
	vector<int> _input_state_ai;
	const SPLayer& _sp_table;
	bool _end_flag;
	bool _sign;
	vector<int> gen_len();
	vector<int> pattern;
	bool init_offset_n_state();
	bool init_weight();
	bool next_offset_n_state();
	bool next_weight();
public:
	State first_state;
	State& output_state;
	unique_ptr<ExchangeGrayCode> weight_ptr;
	unique_ptr<GrayCode> offset_ptr;
	OutputTraverser2(State& so, const vector<int>& p, const State& si, const SPLayer& sp);
	void next();
	bool end()const;
	friend ostream& operator<<(ostream& out, const OutputTraverser& obj);
};
