#pragma once

#include <vector>
#include <iostream>
#include "base_adapter.h"

template<typename Range>
auto AsVectorAdapter(const Range& range_) {
	Range t_range = range_;
	using ValueType = Range::value_type;

	std::vector<ValueType> result;

	for (auto it = t_range.begin(); it != t_range.end(); ++it) {
		result.push_back(*it);
	}

	return result;
}

class AsVector : public Adapter {
public:
	template<typename Range>
	auto operator()(const Range& range) const {
		return AsVectorAdapter<Range>(range);
	}
};