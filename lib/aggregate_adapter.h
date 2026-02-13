#pragma once

#include <vector>
#include <unordered_map>

template<typename Range, typename InitialValue,
        typename Aggregator, typename KeyFunction>
class AggregateByKeyAdapter : public Adapter {
private:
	using KeyType = decltype(std::declval<KeyFunction>()
                    (std::declval<typename Range::value_type>()));

	using ValueType = InitialValue;

    std::vector<std::pair<KeyType, ValueType>> aggregated_data_;

public:
    AggregateByKeyAdapter(
        Range range,
        InitialValue initial_value,
        Aggregator aggregator,
        KeyFunction key_function
    ) {
        std::unordered_map<
            decltype(std::declval<KeyFunction>()(
            std::declval<typename Range::value_type>())),
            InitialValue> aggregated_map;

        for (const auto& elem : range) {
            auto key = key_function(elem);
            if (aggregated_map.find(key) == aggregated_map.end()) {
                aggregated_map[key] = initial_value;
            }

            aggregator(elem, aggregated_map[key]);
        }

        for (const auto& [key, value] : aggregated_map) {
            aggregated_data_.emplace_back(key, value);
        }
    }
	using iterator = std::vector<std::pair<KeyType, ValueType>>::iterator;
	using value_type = std::pair<KeyType, ValueType>;

    auto begin() const { return aggregated_data_.begin(); }
    auto end() const { return aggregated_data_.end(); }
	auto begin() { return aggregated_data_.begin(); }
    auto end() { return aggregated_data_.end(); }
};

template<typename InitialValue, typename Aggregator, typename KeyFunction>
class AggregateByKey : public Adapter {
private:
    InitialValue initial_value_;
    Aggregator aggregator_;
    KeyFunction key_function_;

public:
    AggregateByKey(InitialValue initial_value,
                Aggregator aggregator, KeyFunction key_function)
        : initial_value_(initial_value)
        , aggregator_(aggregator)
        , key_function_(key_function) {}

    template<typename Range>
    auto operator()(const Range& range) const {
        return AggregateByKeyAdapter<
            Range, InitialValue, Aggregator, KeyFunction>(
            range, initial_value_, aggregator_, key_function_
            );
    }
};