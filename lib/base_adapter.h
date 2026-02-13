#pragma once

#include <optional>

template <typename Key, typename Value>
struct KV {
	Key key;
	Value value;

	bool operator==(const KV& other) const {
        return key == other.key && value == other.value;
    }
};

template <typename Base, typename Joined>
struct JoinResult {
	Base base;
	std::optional<Joined> joined;

	bool operator==(const JoinResult& other) const {
        return base == other.base && joined == other.joined;
    }
};

class Adapter {
public:
	virtual ~Adapter() = default;
};