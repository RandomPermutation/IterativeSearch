#include "../include/looplessCode.h"

ExchangeCode::ExchangeCode(bool irs, int sn, int an) :is_rotation_symm(irs), N0(irs ? sn - 1 : sn), M0(irs ? an - 1 : an), A0(N0 + 1), T0(N0 + 1), F0(N0 + 1), H0(N0 + 1), C0(M0 + 1), is_end(false), X0(-1), Y0(-1) {
	if (an == 0) {
		is_end = true; return;
	}
	init();
	if (irs) {
		C0[0] = 0; A0[0] = 1;
	}
}
void ExchangeCode::init() {
	for (int i = 0; i <= (N0 - M0); i++) A0[i] = 0;
	for (int i = N0 - M0 + 1; i <= N0; i++) A0[i] = 1;
	for (int i = 1; i <= M0; i++) { C0[i] = N0 - M0 + i; H0[N0 - M0 + i] = i; }
	T0[N0 - M0] = -1; T0[1] = 0; F0[N0] = N0 - M0 + 1; I0 = N0 - M0; L0 = N0;
}
bool ExchangeCode::check_end() {
	return (I0 % N0) == 0;
}
bool ExchangeCode::end()const {
	return is_end;
}
void ExchangeCode::update() {
	if (T0[I0] < 0) {
		if ((-T0[I0]) != (I0 - 1)) {
			T0[I0 - 1] = T0[I0];
		}
		T0[I0] = I0 - 1;
	}
	if (A0[I0] == 0) {
		X0 = I0; Y0 = F0[L0];
		if (A0[I0 - 1] == 1) {
			F0[I0] = F0[I0 - 1];
		}
		else {
			F0[I0] = I0;
		}
		if (F0[L0] == L0) {
			L0 = I0; I0 = T0[I0]; goto JMP2;
		}
		if (L0 == N0) {
			T0[F0[N0]] = -I0 - 1; T0[I0 + 1] = T0[I0]; I0 = F0[N0]; F0[N0] = F0[N0] + 1; goto JMP2;
		}
		T0[L0] = -I0 - 1; T0[I0 + 1] = T0[I0]; F0[L0] = F0[L0] + 1; I0 = L0; goto JMP2;
	}
	Y0 = I0;
	if (I0 != L0) {
		F0[L0] = X0 = F0[L0] - 1; F0[I0 - 1] = F0[I0];
		if (L0 == N0) {
			if (I0 == (F0[N0] - 1)) {
				I0 = T0[I0]; goto JMP2;
			}
			T0[F0[N0] - 1] = -I0 - 1; T0[I0 + 1] = T0[I0]; I0 = F0[N0] - 1; goto JMP2;
		}
		T0[L0] = -I0 - 1; T0[I0 + 1] = T0[I0]; I0 = L0; goto JMP2;
	}
	X0 = N0; F0[L0 - 1] = F0[L0]; F0[N0] = N0; L0 = N0;
	if (I0 == N0 - 1) {
		I0 = T0[N0 - 1]; goto JMP2;
	}
	T0[N0 - 1] = -I0 - 1; T0[I0 + 1] = T0[I0]; I0 = N0 - 1;
JMP2:
	A0[X0] = 1; A0[Y0] = 0; H0[X0] = Z0 = H0[Y0]; C0[Z0] = X0;
}
void ExchangeCode::next() {
	if (check_end()) is_end = true;
	else if (!end()) update();
}
bool ExchangeCode::is_duplicate()const {
	int an = M0 + 1;
	if (an <= 1) return false;
	int sn = N0 + 1;
	for (int shift = 1; shift < an; shift++) {
		bool skip = false;
		int j = 1;
		for (int i = shift + 1; i < an; i++, j++) {
			int C0_tmp = C0[i] - C0[shift];
			if (C0_tmp < C0[j]) return true;
			else if (C0_tmp > C0[j]) {
				skip = true; break;
			}
		}
		if (!skip) {
			for (int i = 0; i < shift; i++, j++) {
				int C0_tmp = C0[i] - C0[shift] + sn;
				if (C0_tmp < C0[j]) return true;
				else if (C0_tmp > C0[j]) break;
			}
		}
	}
	return false;
}
ostream& operator<<(ostream& out, const ExchangeCode& obj) {
	if (obj.end()) { out << "end of ExchangeCode" << std::endl; }
	else {
		out << "exchange\t" << obj.get_exchange_x() << "\tand\t" << obj.get_exchange_y() << "\n";
		out << std::dec << "activeness_vector: " << "\t";
		for (int i = 0; i < obj.get_sn(); i++) out << obj.is_active(i) << " ";
		out << "\n";
		out << "activeness_index: " << "\t";
		for (int i = 0; i < obj.get_an(); i++) out << obj.get_ai(i) << " ";
		out << "\nduplicate:" << obj.is_duplicate();
	}
	out << "\n------------------------------";
	return out;
}

GrayCode::GrayCode(vector<int> mm) :N1((int)mm.size()), m(mm), a(N1, 0), d(N1, 1), f(N1 + 1), x(N1), j(0), j_former(-1), a_former(-1), count0(0), count1(0), is_end(false) {
	init();
	if (check_empty()) is_end = true;
}
void GrayCode::init() {
	for (int i = 0; i < N1; i++) { f[i] = i; }
	f[N1] = N1;
	count0 = 0; count1 = 0;
	for (int i = 0; i < N1; i++) {
		if (m[i] > 0) count0++;
		if (m[i] > 1) { x[count1] = i; count1++; }
	}
	j = f[0];
	f[0] = 0;
}
bool GrayCode::check_end() {
	if (j == count1) return true;
	return false;
}
void GrayCode::update() {
	a[x[j]] = a[x[j]] + d[j];
	if ((a[x[j]] == 0) || (a[x[j]] == m[x[j]] - 1))
	{
		d[j] = -d[j];	f[j] = f[j + 1];	f[j + 1] = j + 1;
	}
	j = f[0];
	f[0] = 0;
}
bool GrayCode::check_empty() {
	return count0 != N1;
}
void GrayCode::next() {
	if (check_end()) { is_end = true; return; }
	j_former = j;
	a_former = a[x[j]];
	update();
}
bool GrayCode::end()const {
	return is_end;
}
ostream& operator<<(ostream& out, const GrayCode& obj) {
	if (obj.end()) { out << "end of GrayCode" << std::endl; return out; }
	out << "change_index:" << obj.get_change_index() << " from:" << obj.get_change_offset_from() << " to:" << obj.get_change_offset_to() << "\n";
	out << std::dec << "offset: ";
	for (int i = 0; i < obj.get_an(); i++) out << obj.get_offset(i) << " ";
	out << "\n***************************";
	return out;
}

ExchangeGrayCode::ExchangeGrayCode(const vector<int>& wn) :activeness_number(0), weight_number(0), X(0), Y(0), is_end(false) {
	init(wn);
}
void ExchangeGrayCode::init(const vector<int>& wn) {
	A.push_back(-1);
	for (int i = 0; i < wn.size(); i++) {
		if (wn[i] != 0) {
			n.push_back(wn[i]);
			weight_number++;
			activeness_number += wn[i];
			for (int l = 0; l < wn[i]; l++) A.push_back(i);
		}
	}
	if (activeness_number == 0) {
		is_end = true;
		return;
	}
	k = weight_number - 1;
	j = k;
	n_sum.push_back(n[0]);
	for (int i = 1; i < weight_number; i++)
		n_sum.push_back(n_sum[i - 1] + n[i]);

	vector<int> a(weight_number);
	for (int i = 1; i <= k; i++) {
		int ii;
		for (ii = i; ii <= k; ii++)
			if ((n[ii] + n_sum[ii - 1]) != (n[ii] ^ n_sum[ii - 1])) break;
		a[i] = ii <= k ? ii : k;
	}
	miu.resize(weight_number);
	m.resize(weight_number + 1);
	right.resize(weight_number + 1);
	d.resize(weight_number);
	e.resize(weight_number + 1);
	rank.resize(weight_number + 1);
	p.resize(weight_number + 1, vector<int>(activeness_number + 1));
	l.resize(weight_number + 1, vector<int>(activeness_number + 1));
	f.resize(weight_number + 1, vector<int>(activeness_number + 1));
	for (int i = 1; i <= k; i++) {
		miu[i] = n_sum[a[i]] - n_sum[i - 1];
		m[i] = n_sum[i - 1];
		right[i] = n_sum[i - 1] + 1;
		for (int ii = 1; ii <= n_sum[i - 1]; ii++) {
			p[i][ii] = ii;
			l[i][ii] = 0;
			f[i][ii] = ii - 1;
		}
		d[i] = 0;
		e[i] = i - 1;
		rank[i] = 0;
	}
}
void ExchangeGrayCode::next_P(bool is_succ, vector<int>& p, vector<int>& l, vector<int>& f, int& right, int& m, int n0, int n1) {
	pos1 = p[m];
	if (l[m] == 0) {
		l[m] = (m > 1) ? n1 + m - p[m - 1] - 1 : n1;
	}
	if (p[m] == n1 + m && right <= n0) {
		right += 1;
	}
	if (is_succ == 1) {
		p[m] = (m % 2 == 1) ? ((p[m] < n1 + m) ? p[m] + 1 : p[m - 1] + 1) : ((p[m] > p[m - 1] + 1) ? p[m] - 1 : n1 + m);//?inc:dec
	}
	else {
		p[m] = (m % 2 == 1) ? ((p[m] > p[m - 1] + 1) ? p[m] - 1 : n1 + m) : ((p[m] < n1 + m) ? p[m] + 1 : p[m - 1] + 1);//?dec:inc
	}
	pos2 = p[m];
	l[m] -= 1;
	f[right] = right - 1;
	if (l[m] == 0) {
		if (p[m] == n1 + m) {
			right -= 1;
		}
		else {
			f[m + 1] = f[m];
		}
		f[m] = m - 1;
	}
	m = f[right];
}
void ExchangeGrayCode::update() {
	next_P(d[j] == 0, p[j], l[j], f[j], right[j], m[j], n_sum[j - 1], n[j]);
	if (rank[j] == 0 && j != k) {
		X = pos1 + miu[j + 1];
		Y = pos2 + miu[j + 1];
	}
	else {
		X = pos1;
		Y = pos2;
	}

	int tmp = A[X];
	A[X] = A[Y];
	A[Y] = tmp;

	e[k + 1] = k;
	if (m[j] == 0) {
		if (d[j] == 0) {
			d[j] = 1;
			m[j] = 1;
		}
		else {
			d[j] = 0;
			m[j] = n_sum[j - 1];
		}
		right[j] = m[j] + 1;
		e[j + 1] = e[j];
		e[j] = j - 1;
		rank[j] = rank[j] ^ 1;
	}
	rank[j] = rank[j] ^ 1;
	j = e[k + 1];
}
void ExchangeGrayCode::next() {
	if (check_end()) is_end = true;
	else update();
}
bool ExchangeGrayCode::end()const {
	return is_end;
}
ostream& operator<<(ostream& out, const ExchangeGrayCode& obj) {
	if (obj.end()) { out << "end of ExchangeGrayCode"; return out; }
	out << std::dec << "exchange_x:" << obj.get_exchange_x() << " exchange_y:" << obj.get_exchange_y() << "\n";
	out << "weight_index: ";
	for (int i = 0; i < obj.get_an(); i++)
		out << obj.get_weight(i) << " ";
	out << "\n###########################";
	return out;
}