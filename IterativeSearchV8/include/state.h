#pragma once
#include <emmintrin.h>
#include "config.h"
#include <vector>
using std::vector;
#include <ostream>
using std::ostream;
#include <string>
using std::string;

class State {
	__m128i word128[WORD128_SIZE];
public:
	State() :word128{ 0 } {}
	State(uint8_t* arr) { set0(); for (int i = 0; i < SBOX_NUM; i++) set(i, arr[i]); }
	State(const string& s);
	State& operator=(const State& rhs);
	const State operator^(const State& rhs)const;
	State& operator^=(const State& rhs);
	const State operator&(const State& rhs)const;
	State& operator&=(const State& rhs);
	State operator<<(int n)const;
	State operator>>(int n)const;
	bool operator>(const State& rhs)const;
	bool operator<(const State& rhs)const;
	bool operator==(const State& rhs)const;
	bool operator>=(const State& rhs)const;
	bool operator<=(const State& rhs)const;
	uint8_t get_word8(size_t pos)const;
	virtual int get_an()const;
	int hw()const;
	vector<int> get_ai()const;
	void set(size_t pos, uint8_t val);
	void set0();
	void exchange(size_t pos_x, size_t pos_y);
	friend std::hash<State>;
	friend ostream& operator<<(ostream& out, const State& obj);
	string compact_exp()const;
};

class StateForPrint :
	public State {
	vector<int> indice;
public:
	StateForPrint(const State& s) :State(s), indice(s.get_ai()) {}
	friend ostream& operator<<(ostream& out, const StateForPrint& obj);
};

int to_repre_rotl_num(const State& s);
int to_repre_always0(const State& s);
#if SYMMETRY == IDENTITY
#define TO_REPRE to_repre_always0
#elif SYMMETRY == ROTATIONAL
#define TO_REPRE to_repre_rotl_num
#endif

namespace std {
	template <class T>
	inline void hash_combine(std::size_t& seed, T const& v) {
		seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	template <>
	struct hash<State> {
		size_t operator()(const State& s)const noexcept {
			size_t hash_value = 0;
			for (int i = 0; i < WORD128_SIZE; i++) {
				hash_combine<uint64_t>(hash_value, s.word128[i].m128i_u64[0]);
				hash_combine<uint64_t>(hash_value, s.word128[i].m128i_u64[1]);
			}
			return hash_value;
		}
	};
}