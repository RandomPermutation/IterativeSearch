#include "../include/patternGenerator.h"

PatternGenerator::PatternGenerator(const vector<Weight>& wv, Weight wlb, Weight wub) :_weight_vec(wv), _wlb(wlb), _wub(wub), num(_weight_vec.size(), 0), weight(0), an(0), _backtrack(false), _increment(true), _end_flag(false) {
	next();
}
void PatternGenerator::next() {
	while (true) {
		if (_backtrack == true) {
			weight -= num.back() * _weight_vec[num.size() - 1];
			an -= num.back();
			num.pop_back();
			if (num.empty()) {
				_end_flag = true;
				return;
			}
			_backtrack = false;
			num.back()++;
			weight += _weight_vec[num.size() - 1];
			an++;
			continue;
		}
		if (_increment == true) {
			num.back()++;
			weight += _weight_vec[num.size() - 1];
			an++;
			_increment = false;
			continue;
		}
		if (weight >= _wub) {
			_backtrack = true;
			continue;
		}
		if (num.size() == _weight_vec.size()) {
			_increment = true;
			if (weight >= _wlb) {
				return;
			}
			continue;
		}
		num.emplace_back(0);
	}
}
ostream& operator<<(ostream& out, const PatternGenerator& obj) {
	if (obj.end()) { out << "end of PatternGenerator1" << "\n"; return out; }
	out << "pattern:\t";
	for (auto it = obj.num.begin(); it != obj.num.end(); it++)
		out << std::dec << *it << ", ";
	out << "\tan:" << obj.an << "\tweight:" << obj.weight << "\n------------------------------";
	return out;
}

PatternGeneratorFixingAn::PatternGeneratorFixingAn(const vector<Weight>& wv, Weight wlb, Weight wub, int fan) :_weight_vec(wv), _wlb(wlb), _wub(wub), num(_weight_vec.size(), 0), weight(0), an(0), _fan(fan), _backtrack(true), _end_flag(false) {
	num.back() = _fan;
	an = _fan;
	weight = _weight_vec.back() * _fan;
	if (weight >= _wub || weight < _wlb)
		next();
}
void PatternGeneratorFixingAn::next() {
	while (true) {
		if (_backtrack == true) {
			weight -= num.back() * _weight_vec[num.size() - 1];
			an -= num.back();
			num.pop_back();
			if (num.empty()) {
				_end_flag = true;
				return;
			}
			_backtrack = false;
			num.back()++;
			weight += _weight_vec[num.size() - 1];
			an++;
			continue;
		}
		if (weight >= _wub || an > _fan) {
			_backtrack = true;
			continue;
		}
		if (num.size() == _weight_vec.size() - 1) {
			int anr = _fan - an;
			num.emplace_back(anr);
			weight += anr * _weight_vec[num.size() - 1];
			an += anr;
			_backtrack = true;
			if (weight < _wub && weight >= _wlb) {
				return;
			}
			continue;
		}
		num.emplace_back(0);
	}
}
ostream& operator<<(ostream& out, const PatternGeneratorFixingAn& obj) {
	if (obj.end()) { out << "end of PatternGenerator2" << "\n"; return out; }
	out << "pattern:\t";
	for (auto it = obj.num.begin(); it != obj.num.end(); it++)
		out << std::dec << *it << ", ";
	out << "\tan:" << obj.an << "\tweight:" << obj.weight << "\n------------------------------";
	return out;
}