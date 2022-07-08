#include <vector>
using std::vector;
#include <ostream>
using std::ostream;

class ExchangeCode {
	bool is_rotation_symm;
	int N0, M0, X0, Y0, I0, L0, Z0;
	vector<int> A0, T0, F0, H0, C0;
	bool is_end;
	void init();
	bool check_end();
	void update();
public:
	ExchangeCode(bool irs, int sn, int an);
	int get_ai(int i)const { return is_rotation_symm ? C0[i] : C0[i + 1] - 1; }
	bool is_active(int i)const { return is_rotation_symm ? A0[i] : A0[i + 1]; }
	int get_an()const { return is_rotation_symm ? M0 + 1 : M0; };
	int get_sn()const { return is_rotation_symm ? N0 + 1 : N0; };
	int get_exchange_x()const { return is_rotation_symm ? X0 : X0 - 1; }
	int get_exchange_y()const { return is_rotation_symm ? Y0 : Y0 - 1; }
	bool is_duplicate()const;
	void next();
	bool end()const;
	friend ostream& operator<<(ostream& out, const ExchangeCode& obj);
};

class GrayCode {
	int N1;
	vector<int> m, a, d, f, x;
	int j, j_former, a_former;
	int count0, count1;
	bool is_end;
	void init();
	bool check_end();
	void update();
	bool check_empty();
public:
	GrayCode(vector<int> mm);
	int get_an()const { return N1; }
	int get_change_index()const { return (j_former >= 0 && j_former < N1) ? x[j_former] : -1; }
	int get_change_offset_from()const { return a_former; }
	int get_change_offset_to()const { return j_former != -1 ? a[x[j_former]] : -1; }
	int get_offset(int i)const { return a[i]; }
	int get_size(int i)const { return m[i]; }
	void next();
	bool end()const;
	friend ostream& operator<<(ostream& out, const GrayCode& obj);
};

class ExchangeGrayCode {
	int weight_number, activeness_number, k;
	vector<vector<int>> p, l, f;
	vector<int> right, m, n, n_sum, miu, e, A;
	vector<bool> d, rank;
	int j, pos1, pos2, X, Y;
	bool is_end;
	void init(const vector<int>& wn);
	void next_P(bool is_succ, vector<int>& p, vector<int>& l, vector<int>& f, int& right, int& m, int n0, int n1);
	void update();
	bool check_end() { return j == 0; }
public:
	ExchangeGrayCode(const vector<int>& wn);
	int get_an()const { return activeness_number; }
	int get_wn()const { return weight_number; }
	int get_exchange_x()const { return X - 1; }
	int get_exchange_y()const { return Y - 1; }
	int get_weight(int i)const { return A[i + 1]; }
	void next();
	bool end()const;
	friend ostream& operator<<(ostream& out, const ExchangeGrayCode& obj);
};