#include <nmmintrin.h>
#include "../include/state.h"

State::State(const string& s) {
	/*int i = 0;
	vector<>
	while (i < s.size()) {
		if (s[i] == '[') {
			string tmp = "";
			i++;
			while (s[i] != ']') {
				tmp+=s[i];
				i++;
			}
		}
	}*/
}

State& State::operator=(const State& rhs) {
	for (int i = 0; i < WORD128_SIZE; i++)
		word128[i] = rhs.word128[i];
	return *this;
}

State& State::operator^=(const State& rhs) {
	for (int i = 0; i < WORD128_SIZE; i++)
		word128[i] = _mm_xor_si128(word128[i], rhs.word128[i]);
	return *this;
}

State& State::operator&=(const State& rhs) {
	for (int i = 0; i < WORD128_SIZE; i++)
		word128[i] = _mm_and_si128(word128[i], rhs.word128[i]);
	return *this;
}

const State State::operator^(const State& rhs)const {
	return State(*this) ^= rhs;
}

const State State::operator&(const State& rhs)const {
	return State(*this) &= rhs;
}
__m128i rotl128(__m128i x, int n) {
#define ROTL128(m) case m:return _mm_slli_si128(x, m);
	switch (n) {
	case 0:break;
		ROTL128(1)	ROTL128(2)	ROTL128(3)	ROTL128(4)	ROTL128(5)	ROTL128(6)	ROTL128(7)	ROTL128(8)
			ROTL128(9)	ROTL128(10) ROTL128(11)	ROTL128(12) ROTL128(13)	ROTL128(14) ROTL128(15)
	case 16:_mm_setzero_si128();
	default:_mm_setzero_si128();
	}
	return x;
}
__m128i rotr128(__m128i x, int n) {
#define ROTR128(m) case m:return _mm_srli_si128(x, m);
	switch (n) {
	case 0:break;
		ROTR128(1)	ROTR128(2)	ROTR128(3)	ROTR128(4)	ROTR128(5)	ROTR128(6)	ROTR128(7)	ROTR128(8)
			ROTR128(9)	ROTR128(10) ROTR128(11)	ROTR128(12) ROTR128(13)	ROTR128(14) ROTR128(15)
	case 16:_mm_setzero_si128();
	default:_mm_setzero_si128();
	}
	return x;
}

State State::operator<<(int n)const {
	if (n < 0) return this->operator>>(-n);
	n %= SBOX_NUM;
	if (n == 0) return *this;
	State out;
	int q = n >> 4;
	int r = n - (q << 4);
	if (r != 0) {
		for (int i = 0; i < WORD128_SIZE; i++) {
			int k = i - q;
			if (k < 0) k += WORD128_SIZE;
			int l = k - 1;
			if (l < 0) l += WORD128_SIZE;
			out.word128[i] = _mm_xor_si128(rotl128(word128[k], r), rotr128(word128[l], 16 - r));
		}
	}
	else {
		for (int i = 0; i < WORD128_SIZE; i++) {
			int k = i - q;
			if (k < 0) k += WORD128_SIZE;
			out.word128[i] = word128[k];
		}
	}
	return out;
}

State State::operator>>(int n)const {
	n %= SBOX_NUM;
	if (n <= 0) return this->operator<<(-n);
	return this->operator<<(SBOX_NUM - n);
}

bool State::operator==(const State& rhs)const {
	for (int i = 0; i < WORD128_SIZE; i++) {
		__m128i tmp = _mm_cmpeq_epi8(word128[i], rhs.word128[i]);
		if (_mm_movemask_epi8(tmp) != (int)0xffff) return false;
	}
	return true;
}
__m128i _mm_cmpge_epu8(__m128i A, __m128i B) {
	return _mm_cmpeq_epi8(_mm_max_epu8(A, B), A);
}
__m128i _mm_cmple_epu8(__m128i A, __m128i B) {
	return _mm_cmpge_epu8(B, A);
}
__m128i _mm_cmpgt_epu8(__m128i A, __m128i B) {
	return _mm_xor_si128(_mm_cmple_epu8(A, B), _mm_set1_epi8(-1));
}
__m128i _mm_cmplt_epu8(__m128i A, __m128i B) {
	return  _mm_cmpgt_epu8(B, A);
}

bool State::operator>(const State& rhs)const {
	for (int i = WORD128_SIZE - 1; i >= 0; i--) {
		__m128i res_gt_128 = _mm_cmpgt_epu8(word128[i], rhs.word128[i]);
		__m128i res_lt_128 = _mm_cmplt_epu8(word128[i], rhs.word128[i]);
		int res_gt = _mm_movemask_epi8(res_gt_128);
		int res_lt = _mm_movemask_epi8(res_lt_128);
		if (res_lt > res_gt) return true;
		else if (res_lt < res_gt) return false;
	}
	return false;
}

bool State::operator>=(const State& rhs)const {
	return (this->operator>(rhs) || this->operator==(rhs));
}

bool State::operator<(const State& rhs)const {
	return !(this->operator>(rhs)) && !(this->operator==(rhs));
}

bool State::operator<=(const State& rhs)const {
	return !(this->operator>(rhs));
}

uint8_t State::get_word8(size_t pos)const {
	int q = pos >> 4;
	int r = pos - (q << 4);
	return word128[WORD128_SIZE - 1 - q].m128i_u8[15 - r];
}

int State::get_an()const {
	int sum = 0;
	for (int i = 0; i < WORD128_SIZE; i++) {
		__m128i tmp = _mm_cmpeq_epi8(word128[i], _mm_setzero_si128());
		unsigned int pattern = _mm_movemask_epi8(tmp);
		pattern = (~pattern) & 0xffff;
		sum += (_mm_popcnt_u32(pattern));
	}
	return sum;
}
static int popcount64d(uint64_t x) {
	int count;
	for (count = 0; x; count++)
		x &= x - 1;
	return count;
}

int State::hw()const {
	int count = 0;
	for (int i = 0; i < WORD128_SIZE; i++) {
		count += popcount64d(word128[i].m128i_u64[0]);
		count += popcount64d(word128[i].m128i_u64[1]);
	}
	return count;
}
#include "w16v.h"

vector<int> State::get_ai()const {
	vector<int> indice;
	for (int i = 0; i < WORD128_SIZE; i++) {
		__m128i tmp = _mm_cmpeq_epi8(word128[WORD128_SIZE - 1 - i], _mm_setzero_si128());
		unsigned int pattern = _mm_movemask_epi8(tmp);
		pattern = (~pattern) & 0xffff;
		int hw = _mm_popcnt_u32(pattern);
		tmp = _mm_add_epi8(*(__m128i*)w16v_r[pattern], _mm_set1_epi8(i << 4));//一共256个S盒以内
		uint8_t* u8_p = (uint8_t*)&tmp;
		indice.insert(indice.end(), u8_p, u8_p + hw);
	}
	return indice;
}

void State::set(size_t pos, uint8_t val) {
	int q = pos >> 4;
	int r = pos - (q << 4);
	word128[WORD128_SIZE - 1 - q].m128i_u8[15 - r] = val;
}

void State::set0() {
	for (int i = 0; i < WORD128_SIZE; i++)
		word128[i] = _mm_setzero_si128();
}

void State::exchange(size_t pos_x, size_t pos_y) {
	uint8_t tmp = get_word8(pos_x);
	set(pos_x, get_word8(pos_y));
	set(pos_y, tmp);
}


#include <iomanip>
ostream& operator<<(ostream& out, const State& obj) {
	out << std::hex;
	for (int i = 0; i < SBOX_NUM; i++)
		out << std::setfill('0') << std::setw(1) << (int)obj.get_word8(i);// << " ";
	return out;
}

ostream& operator<<(ostream& out, const StateForPrint& obj) {
	if (obj.indice.empty()) {
		out << "all zero";
		return out;
	}
	for (int i = 0; i < obj.indice.size(); i++) {
		out << "X[" << std::dec << obj.indice[i] << "]=0x" << std::hex << (int)obj.get_word8(obj.indice[i]);
		if (i != (obj.indice.size() - 1)) out << ",";
	}
	return out;
}

int to_repre_rotl_num(const State& s) {
	vector<int> indice(s.get_ai());
	int an = indice.size();
	if (an == 0) return 0;
	if (an == 1) return indice[0];
	State best_s(s << indice[0]);
	int best_rotl(indice[0]);
	for (int i = 1; i < an; i++) {
		State cur_s(s << indice[i]);
		if (cur_s > best_s) {
			best_s = cur_s;
			best_rotl = indice[i];
		}
	}
	return best_rotl;
}
int to_repre_always0(const State& s) {
	return 0;
}