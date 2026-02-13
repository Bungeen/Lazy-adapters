#pragma once

#include "base_adapter.h"

template<typename Range>
class AsDataFlow : public Adapter {
private:
    Range& range_;

public:
	AsDataFlow(Range& range)
        : range_(range) 
	{}
	template<typename IteratorType>
    class Iterator {
    private:
		IteratorType it_;
		IteratorType end_;

    public:
        Iterator(Range& range, bool is_end)
			: it_(range.end())
			, end_(range.end())
		{
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
			}
		}

		Iterator(const Range& range, bool is_end)
			: it_(range.end())
			, end_(range.end())
		{
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
			}
		}

        auto& operator*() {
            return *it_;
        }

		auto& operator*() const {
            return *it_;
        }

        Iterator& operator++() {
            ++it_;
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
        return iterator(range_, false);
    }

    iterator end() {
        return iterator(range_, true);
    }

	iterator begin() const {
        return iterator(range_, false);
    }

    iterator end() const {
        return iterator(range_, true);
    }
};