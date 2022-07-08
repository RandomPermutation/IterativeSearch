#pragma once
#include <unordered_map>
using std::unordered_map;
using std::make_pair;
#include <map>
using std::map;

template <typename Key, typename Value>
struct HashTable {
	map<Key, Value> hash_table;
	//unordered_map<Key, Value> hash_table;
	HashTable() {}
	HashTable(const Key& key, const Value& val) { insert(key, val); }
	bool insert(const Key& key, const Value& val) {
		auto ret = hash_table.insert(make_pair(key, val));
		if (!ret.second)
			if (val > ret.first->second)
				ret.first->second = val;
		return ret.second;
	}
	void add(const Key& key, const Value& val) {
		auto ret = hash_table.insert(make_pair(key, val));
		if (!ret.second)
			ret.first->second += val;
	}
	bool search(const Key& key) {
		return hash_table.find(key) != hash_table.end();
	}
};

template<typename Key1, typename Key2, typename Value>
struct TwoKeyHashTable {
	map<Key1, HashTable<Key2, Value>> hash_table;
	//unordered_map<Key1, HashTable<Key2, Value>> hash_table;
	TwoKeyHashTable() {}
	TwoKeyHashTable(const Key1& key1, const Key2& key2, const Value& val) { insert(key1, key2, val); }
	bool insert(const Key1& key1, const Key2& key2, const Value& val) {
		auto ret = hash_table.insert(make_pair(key1, HashTable<Key2, Value>(key2, val)));
		if (!ret.second)
			return ret.first->second.insert(key2, val);
		else return ret.second;
	}
	void add(const Key1& key1, const Key2& key2, const Value& val) {
		auto ret = hash_table.insert(make_pair(key1, HashTable<Key2, Value>(key2, val)));
		if (!ret.second)
			ret.first->second.add(key2, val);
	}
	bool search(const Key1& key1, const Key2& key2) {
		auto it = hash_table.find(key1);
		if (it == hash_table.end()) return false;
		else return it->search(key2);
	}
};

template<typename Key1, typename Key2, typename Key3, typename Value>
struct ThreeKeyHashTable {	//unordered_map < Key1, TwoKeyHashTable<Key2, Key3, Value>> hash_table;
	map < Key1, TwoKeyHashTable<Key2, Key3, Value>> hash_table;

	ThreeKeyHashTable() {}
	ThreeKeyHashTable(const Key1& key1, const Key2& key2, const Key3& key3, const Value& val) { insert(key1, key2, key3, val); }
	bool insert(const Key1& key1, const Key2& key2, const Key3& key3, const Value& val) {
		auto ret = hash_table.insert(make_pair(key1, TwoKeyHashTable<Key2, Key3, Value>(key2, key3, val)));
		if (!ret.second)
			return ret.first->second.insert(key2, key3, val);
		else return ret.second;
	}
	void add(const Key1& key1, const Key2& key2, const Key3& key3, const Value& val) {
		auto ret = hash_table.insert(make_pair(key1, TwoKeyHashTable<Key2, Key3, Value>(key2, key3, val)));
		if (!ret.second)
			ret.first->second.add(key2, key3, val);
	}
	bool search(const Key1& key1, const Key2& key2, const Key3& key3) {
		auto it = hash_table.find(key1);
		if (it == hash_table.end()) return false;
		else return it->search(key1, key2);
	}
};
