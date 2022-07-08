#include "../include/traverser.h"
void InputTraverser::next() {
	if (offset_ptr == nullptr || offset_ptr->end()) {
		if (active_ptr->end()) {
			_end_flag = true;
			return;
		}
		else {
			next_active_and_init_offset();
		}
	}
	else {
		next_offset();
	}
}
void InputTraverser::init_offset() {
	offset_ptr = unique_ptr<GrayCode>(new GrayCode(num));
	if (offset_ptr->end()) next();
	else {
		state = first_state;
	}
}
void InputTraverser::next_offset() {
	offset_ptr->next();
	if (offset_ptr->end()) next();
	else {
		int change_index = offset_ptr->get_change_index();
		state.set(active_ptr->get_ai(change_index), offset_ptr->get_change_offset_to() + 1);
	}
}
void InputTraverser::init_active_and_init_offset() {
	active_ptr = unique_ptr<ExchangeCode>(new ExchangeCode(SYMMETRY == ROTATIONAL, SBOX_NUM, _an));
	if (active_ptr->end()) next();
	else {
		first_state.set0();
		for (int i = 0; i < _an; i++) first_state.set(active_ptr->get_ai(i), 1);
		if (SYMMETRY == ROTATIONAL && active_ptr->is_duplicate()) next();
		else init_offset();
	}
}
void InputTraverser::next_active_and_init_offset() {
	active_ptr->next();
	if (active_ptr->end()) next();
	else {
		int x = active_ptr->get_exchange_x();
		int y = active_ptr->get_exchange_y();
		uint8_t tmp = first_state.get_word8(x);
		first_state.set(x, first_state.get_word8(y));
		first_state.set(y, tmp);
		if (SYMMETRY==ROTATIONAL && active_ptr->is_duplicate()) next();
		else init_offset();
	}
}
ostream& operator<<(ostream& out, const InputTraverser& obj) {
	if (obj.end()) { out << "end of InputTraverseCode" << "\n"; return out; }
	out << obj.state << "\n";
	out << *obj.active_ptr << "\n";
	out << *obj.offset_ptr << "\n";
	return out;
}

OutputTraverser::OutputTraverser(State& so, Weight wlb, Weight wub, const State& si, const SPLayer& sp) : output_state(so), _wlb(wlb), _wub(wub),
_input_state(si), _input_state_ai(si.get_ai()), _sp_table(sp), _end_flag(false), _sign(0) {
	pattern_ptr = unique_ptr<PatternGeneratorFixingAn>(new PatternGeneratorFixingAn(_sp_table.get_weight_vec(), wlb, wub, _input_state_ai.size()));
	if (pattern_ptr->end()) { _end_flag = true; return; }
	init_weight();
}
vector<int> OutputTraverser::gen_len() {
	vector<int> len;
	for (int i = 0; i < _input_state_ai.size(); i++)
		len.emplace_back(_sp_table.get_sp_size(_input_state.get_word8(_input_state_ai[i]), _input_state_ai[i], weight_ptr->get_weight(i)));
	return len;
}
bool OutputTraverser::init_offset_n_state() {
	offset_ptr = unique_ptr<GrayCode>(new GrayCode(gen_len()));
	if (offset_ptr->end()) { next(); return 1; }
	output_state.set0();
	for (int i = 0; i < _input_state_ai.size(); i++) {
		output_state ^= _sp_table.get_sp_at(_input_state.get_word8(_input_state_ai[i]), _input_state_ai[i], weight_ptr->get_weight(i), offset_ptr->get_offset(i));
		_sign ^= _sp_table.get_sign_at(_input_state.get_word8(_input_state_ai[i]), weight_ptr->get_weight(i), offset_ptr->get_offset(i));
	}
	return 0;
}
bool OutputTraverser::next_offset_n_state() {
	offset_ptr->next();
	if (offset_ptr->end()) { next(); return 1; }
	int change_index = offset_ptr->get_change_index();
	int offset_from = offset_ptr->get_change_offset_from();
	int offset_to = offset_ptr->get_change_offset_to();
	output_state ^= _sp_table.get_sp_at(_input_state.get_word8(_input_state_ai[change_index]), _input_state_ai[change_index], weight_ptr->get_weight(change_index), offset_from);
	output_state ^= _sp_table.get_sp_at(_input_state.get_word8(_input_state_ai[change_index]), _input_state_ai[change_index], weight_ptr->get_weight(change_index), offset_to);
	_sign ^= _sp_table.get_sign_at(_input_state.get_word8(_input_state_ai[change_index]), weight_ptr->get_weight(change_index), offset_from);
	_sign ^= _sp_table.get_sign_at(_input_state.get_word8(_input_state_ai[change_index]), weight_ptr->get_weight(change_index), offset_to);
	return 0;
}
bool OutputTraverser::init_weight() {
	weight_ptr = unique_ptr<ExchangeGrayCode>(new ExchangeGrayCode(pattern_ptr->num));
	if (weight_ptr->end()) { next(); return 1; }
	if (init_offset_n_state()) return 1;
	return 0;
}
bool OutputTraverser::next_weight() {
	weight_ptr->next();
	if (weight_ptr->end()) { next(); return 1; }
	if (init_offset_n_state()) return 1;
	return 0;
}
bool OutputTraverser::init_pattern() {
	pattern_ptr = unique_ptr<PatternGeneratorFixingAn>(new PatternGeneratorFixingAn(_sp_table.get_weight_vec(), _wlb, _wub, _input_state_ai.size()));
	if (pattern_ptr->end()) { next(); return 1; }
	if (init_weight()) return 1;
	return 0;
}
bool OutputTraverser::next_pattern() {
	pattern_ptr->next();
	if (pattern_ptr->end()) { next(); return 1; }
	if (init_weight()) return 1;
	return 0;
}
void OutputTraverser::next() {
	if (offset_ptr == nullptr || offset_ptr->end()) {
		if (weight_ptr->end()) {
			if (pattern_ptr->end()) {
				_end_flag = true;
				return;
			}
			else {
				if (next_pattern()) return;
			}
		}
		else {
			if (next_weight()) return;
		}
	}
	else {
		if (next_offset_n_state()) return;
	}
}
bool OutputTraverser::end()const {
	return _end_flag;
}
ostream& operator<<(ostream& out, const OutputTraverser& obj) {
	if (obj.end()) { out << "end of OutputTraverser" << "\n"; return out; }
	out << *(obj.pattern_ptr);
	out << *(obj.weight_ptr);
	out << *(obj.offset_ptr);
	out << obj.output_state << "\n";
	return out;
}
OutputTraverser2::OutputTraverser2(State& so, const vector<int>& p, const State& si, const SPLayer& sp) : output_state(so),
_input_state(si), _input_state_ai(si.get_ai()), _sp_table(sp), _end_flag(false), _sign(0), pattern(p) {
	init_weight();
}
vector<int> OutputTraverser2::gen_len() {
	vector<int> len;
	for (int i = 0; i < _input_state_ai.size(); i++)
		len.emplace_back(_sp_table.get_sp_size(_input_state.get_word8(_input_state_ai[i]), _input_state_ai[i], weight_ptr->get_weight(i)));
	return len;
}
bool OutputTraverser2::init_offset_n_state() {
	offset_ptr = unique_ptr<GrayCode>(new GrayCode(gen_len()));
	if (offset_ptr->end()) { next(); return 1; }
	output_state.set0();
	for (int i = 0; i < _input_state_ai.size(); i++) {
		output_state ^= _sp_table.get_sp_at(_input_state.get_word8(_input_state_ai[i]), _input_state_ai[i], weight_ptr->get_weight(i), offset_ptr->get_offset(i));
		_sign ^= _sp_table.get_sign_at(_input_state.get_word8(_input_state_ai[i]), weight_ptr->get_weight(i), offset_ptr->get_offset(i));
	}
	return 0;
}
bool OutputTraverser2::next_offset_n_state() {
	offset_ptr->next();
	if (offset_ptr->end()) { next(); return 1; }
	int change_index = offset_ptr->get_change_index();
	int offset_from = offset_ptr->get_change_offset_from();
	int offset_to = offset_ptr->get_change_offset_to();
	output_state ^= _sp_table.get_sp_at(_input_state.get_word8(_input_state_ai[change_index]), _input_state_ai[change_index], weight_ptr->get_weight(change_index), offset_from);
	output_state ^= _sp_table.get_sp_at(_input_state.get_word8(_input_state_ai[change_index]), _input_state_ai[change_index], weight_ptr->get_weight(change_index), offset_to);
	_sign ^= _sp_table.get_sign_at(_input_state.get_word8(_input_state_ai[change_index]), weight_ptr->get_weight(change_index), offset_from);
	_sign ^= _sp_table.get_sign_at(_input_state.get_word8(_input_state_ai[change_index]), weight_ptr->get_weight(change_index), offset_to);
	return 0;
}
bool OutputTraverser2::init_weight() {
	weight_ptr = unique_ptr<ExchangeGrayCode>(new ExchangeGrayCode(pattern));
	if (weight_ptr->end()) { next(); return 1; }
	if (init_offset_n_state()) return 1;
	return 0;
}
bool OutputTraverser2::next_weight() {
	weight_ptr->next();
	if (weight_ptr->end()) { next(); return 1; }
	if (init_offset_n_state()) return 1;
	return 0;
}
void OutputTraverser2::next() {
	if (offset_ptr == nullptr || offset_ptr->end()) {
		if (weight_ptr->end()) {
			_end_flag = true;
			return;
		}
		else {
			if (next_weight()) return;
		}
	}
	else {
		if (next_offset_n_state()) return;
	}
}
bool OutputTraverser2::end()const {
	return _end_flag;
}
template<int IB, int OB, int SN>
ostream& operator<<(ostream& out, const OutputTraverser2& obj) {
	if (obj.end()) { out << "end of OutputTraverser2" << "\n"; return out; }
	out << *(obj.weight_ptr);
	out << *(obj.offset_ptr);
	out << obj.output_state << "\n";
	return out;
}