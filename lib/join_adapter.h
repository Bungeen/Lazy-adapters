#pragma once

#include <unordered_map>
#include <functional>

#include "base_adapter.h"

template<typename T>
struct ValueExtractor {
    static auto get(const T& obj) {
        return obj;
    }
};

template<typename Key, typename Value>
struct ValueExtractor<KV<Key, Value>> {
    static auto get(const KV<Key, Value>& kv) {
        return kv.value;
    }
};

template<typename LeftRange, typename RightRange,
        typename LeftKeyFunction, typename RightKeyFunction>
class JoinAdapter : public Adapter {
private:
	LeftRange left_range_;
    RightRange right_range_;
    LeftKeyFunction left_key_function_;
    RightKeyFunction right_key_function_;
public:
	JoinAdapter(const LeftRange& left_range, const RightRange& right_range,
                                        LeftKeyFunction left_key_function,
                                        RightKeyFunction right_key_function)
		: left_range_(left_range)
		, right_range_(right_range)
		, left_key_function_(left_key_function)
		, right_key_function_(right_key_function) {}
	
	template<typename IteratorType>
	class Iterator {
	private:
		IteratorType left_it_;
		IteratorType left_end_;
        using KeyType = decltype(std::declval<RightKeyFunction>()
                        (std::declval<typename RightRange::value_type>()));
        
        using ValueType = typename RightRange::value_type;
		std::unordered_map<KeyType, ValueType> right_map_;

		LeftKeyFunction left_key_function_;
	
	public:
		Iterator(LeftRange& left_range, RightRange& right_range,
                                LeftKeyFunction left_key_function,
                RightKeyFunction right_key_function, bool is_end)
		    : left_end_(left_range.end())
		    , left_it_(left_range.end())
		    , left_key_function_(left_key_function) {

			if (!is_end) {
				left_it_ = left_range.begin();
				for (const auto& right_elem : right_range) {
					right_map_[right_key_function(right_elem)] = right_elem;
				}
			}
		}

		Iterator(const LeftRange& left_range, const RightRange& right_range,
                                        LeftKeyFunction left_key_function,
                            RightKeyFunction right_key_function, bool is_end)
		    : left_end_(left_range.end())
		    , left_it_(left_range.end())
		    , left_key_function_(left_key_function) {

			if (!is_end) {
				left_it_ = left_range.begin();
				for (const auto& right_elem : right_range) {
					right_map_[right_key_function(right_elem)] = right_elem;
				}
			}
		}

		auto operator*() const {
            auto key = left_key_function_(*left_it_);
            auto it = right_map_.find(key);

            using FirstType = decltype(ValueExtractor<
                typename LeftRange::value_type>::get(*left_it_));
            using SecondType = decltype(ValueExtractor<
                typename RightRange::value_type>::get(it->second));

            if (it != right_map_.end()) {
                return JoinResult<FirstType, SecondType>{
                    ValueExtractor<
                        typename LeftRange::value_type>::get(*left_it_),
                    ValueExtractor<
                        typename RightRange::value_type>::get(it->second)
                };
            }

			return JoinResult<FirstType, SecondType>{
				ValueExtractor<typename LeftRange::value_type>::get(*left_it_),
                std::nullopt
			};
        }

		Iterator& operator++() {
            ++left_it_;
            return *this;
        }

        bool operator==(const Iterator& rhs) const {
            return left_it_ == rhs.left_it_;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }
	};

	using iterator = Iterator<typename LeftRange::iterator>;
    using value_type = decltype(std::declval<
                    typename JoinAdapter::iterator>().operator*());

	iterator begin() {
		return iterator(left_range_, right_range_,
                left_key_function_, right_key_function_, false);
	}

	iterator end() {
		return iterator(left_range_, right_range_,
                left_key_function_, right_key_function_, true);
	}

	iterator begin() const {
		return iterator(left_range_, right_range_,
                left_key_function_, right_key_function_, false);
	}

	iterator end() const {
		return iterator(left_range_, right_range_,
                left_key_function_, right_key_function_, true);
	}
};


template<typename RightRange, typename LeftKeyFunction =
        std::function<int(const typename RightRange::value_type&)>,
        typename RightKeyFunction = std::function<
        int(const typename RightRange::value_type&)>>
class Join : public Adapter {
private:
	RightRange right_range_;
	LeftKeyFunction left_key_function_;
	RightKeyFunction right_key_function_;
public:
	Join(RightRange right_range)
		: right_range_(right_range)
		, left_key_function_([](const auto& kv) { return kv.key; })
		, right_key_function_([](const auto& kv) { return kv.key; }) {}

	Join(RightRange right_range, LeftKeyFunction left_key_function,
            RightKeyFunction right_key_function)
		: right_range_(right_range)
		, left_key_function_(left_key_function)
		, right_key_function_(right_key_function)
	{}

	template<typename LeftRange>
	auto operator()(const LeftRange& range) const {
		return JoinAdapter(range, right_range_,
                left_key_function_, right_key_function_);
	}
};