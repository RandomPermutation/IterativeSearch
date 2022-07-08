#pragma once
#include "config.h"
#include <vector>
using std::vector;
#include <ostream>
using std::ostream;


class PatternGenerator {
	vector<Weight> _weight_vec;
	Weight _wlb;
	Weight _wub;
	bool _backtrack;
	bool _increment;
	bool _end_flag;
public:
	vector<int> num;
	Weight weight;
	int an;
	PatternGenerator(const vector<Weight>& wv, Weight wlb, Weight wub);
	void next();
	bool end()const { return _end_flag; }
	friend ostream& operator<<(ostream& out, const PatternGenerator& obj);
};

class PatternGeneratorFixingAn {
	vector<Weight> _weight_vec;
	Weight _wlb;
	Weight _wub;
	bool _backtrack;
	bool _end_flag;
	int _fan;
public:
	vector<int> num;
	Weight weight;
	int an;
	PatternGeneratorFixingAn(const vector<Weight>& wv, Weight wlb, Weight wub, int fan);
	void next();
	bool end()const { return _end_flag; }
	friend ostream& operator<<(ostream& out, const PatternGeneratorFixingAn& obj);
};