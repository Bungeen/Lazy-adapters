#pragma once

#include "base_adapter.h"

template<typename Range, typename Predicate>
class FilterAdapter : public Adapter {
private:
    Range range_;
    Predicate predicate_;

public:
    FilterAdapter(Predicate predicate, const Range& range)
        : range_(range)
		, predicate_(predicate)
	{}

	template<typename IteratorType>
    class Iterator {
    private:
		IteratorType it_;
		IteratorType end_;
        Predicate predicate_;

    public:
        Iterator(Range& range, Predicate predicate, bool is_end)
            : predicate_(predicate)
			, it_(range.end())
			, end_(range.end()) {
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end() ;
			}

            while (it_ != end_ && !predicate_(*it_)) {
                ++it_;
            }
        }

		Iterator(const Range& range, Predicate predicate, bool is_end)
            : predicate_(predicate)
			, it_(range.end())
			, end_(range.end()) {
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
			}
            
            while (it_ != end_ && !predicate_(*it_)) {
                ++it_;
            }
            
        }

        auto operator*() {
            return *it_;
        }

        Iterator& operator++() {
			++it_;
			while (it_ != end_ && !predicate_(*it_)) {
				++it_;
			}

			return *this;
        }

        bool operator==(const Iterator& rhs) const {
            return it_ == rhs.it_;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }
    };

    using iterator = Iterator<typename Range::iterator>;
	using value_type = typename Range::value_type;

    iterator begin() {
        return iterator(range_, predicate_, false);
    }

    iterator end() {
        return iterator(range_, predicate_, true);
    }

	iterator begin() const {
        return iterator(range_, predicate_, false);
    }

    iterator end() const {
        return iterator(range_, predicate_, true);
    }
};

template<typename Predicate>
class Filter : public Adapter {
private:
	Predicate predicate_;
public:
	Filter(Predicate predicate)
		: predicate_(predicate)
	{}

	template<typename Range>
	auto operator()(const Range& range) const {
		return FilterAdapter<Range, Predicate>(predicate_, range);
	}
};