#pragma once
#include <functional>
#include <list>
#include <vector>
#include <utility>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <string>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap
{
private:
	std::list<std::pair<const KeyType, ValueType>> storage;
	Hash hasher;
	std::vector<std::list<typename std::list<std::pair<const KeyType, ValueType>>::iterator>>
		hashmap;
	size_t sz = 0;
	size_t capacity = 0;

	void realloc(int cap) {
		capacity = cap;
		hashmap.resize(cap);
		fill(hashmap.begin(), hashmap.end(),
			std::list<typename std::list<std::pair<const KeyType, ValueType>>::iterator>());
		for (auto begin = storage.begin(); begin != storage.end(); begin++) {
			hashmap[hasher(begin->first) % capacity].push_back(begin);
		}
	}

public:
	HashMap(Hash hasher = Hash()) : hasher(hasher) {}

	template<class It>
	HashMap(It begin, It end, Hash hasher = Hash()) : hasher(hasher) {
		for (; begin != end; begin++) {
			insert(*begin);
		}
	}

	HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> init, Hash hasher = Hash())
		: hasher(hasher) {
		for (auto p : init) {
			insert(p);
		}
	}

	HashMap& operator = (HashMap other) {
		clear();
		for (auto elem : other.storage) {
			storage.emplace_back(elem);
		}
		sz = other.sz;
		hasher = other.hasher;
		realloc(sz);
		return *this;
	}

	int size() const {
		return sz;
	}

	bool empty() const {
		return sz == 0;
	}

	Hash hash_function() const {
		return hasher;
	}

	using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;

	using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

	iterator begin() {
		return storage.begin();
	}

	const_iterator begin() const {
		return storage.begin();
	}

	iterator end() {
		return storage.end();
	}

	const_iterator end() const {
		return storage.end();
	}

	iterator find(const KeyType key) {
		if (capacity == 0) {
			return end();
		}
		int hash = hasher(key) % capacity;
		for (auto iter : hashmap[hash]) {
			if (iter->first == key) {
				return iter;
			}
		}
		return end();
	}

	const_iterator find(const KeyType key) const {
		if (capacity == 0) {
			return end();
		}
		int hash = hasher(key) % capacity;
		for (auto iter : hashmap[hash]) {
			if (iter->first == key) {
				return iter;
			}
		}
		return end();
	}

	 void insert(std::pair<const KeyType, ValueType> new_elem) {
		if (find(new_elem.first) == end()) {
			sz++;
			if (capacity * 2 < sz) {
				realloc(sz);
			}
			hashmap[hasher(new_elem.first) % capacity].push_back(
				storage.insert(storage.end(), new_elem)
			);
		}
	}

	void erase(const KeyType key) {
		if (find(key) != end()) {
			int hash = hasher(key) % capacity;
			int adv = 0;
			for (auto iter : hashmap[hash]) {
				if (iter->first == key) {
					storage.erase(iter);
					break;
				}
				adv++;
			}
			auto iter = hashmap[hash].begin();
			std::advance(iter, adv);
			hashmap[hash].erase(iter);
			sz--;
			if (capacity * 2 > sz * 3) {
				realloc(sz);
			}
		}
	}

	ValueType& operator [] (const KeyType key) {
		auto iter = find(key);
		if (iter == end()) {
			insert(std::pair<const KeyType, ValueType>(key, ValueType()));
			return find(key)->second;
		}
		else {
			return iter->second;
		}
	}

	const ValueType& at(const KeyType key) const {
		auto iter = find(key);
		if (iter == end()) {
			throw std::out_of_range("No such element.");
		}
		else {
			return iter->second;
		}
	}

	void clear() {
		sz = 0;
		capacity = 0;
		hashmap.clear();
		storage.clear();
	}
};
