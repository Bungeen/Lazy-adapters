#pragma once

#include "base_adapter.h"

template<typename Range, typename Predicate>
class TransformAdapter : public Adapter {
private:
    Range range_;
    Predicate predicate_;

public:
    TransformAdapter(Predicate predicate, const Range& range)
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
			, end_(range.end())
		{
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
			}
		}
        Iterator(const Range& range, Predicate predicate, bool is_end)
			: predicate_(predicate)
			, it_(range.end())
			, end_(range.end())
		{
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
			}
		}

        auto operator*() {
            return predicate_(*it_);
        }

        auto operator*() const {
            return predicate_(*it_);
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
	using value_type = std::invoke_result_t<Predicate,
                                typename Range::value_type&>;

    Iterator<typename Range::iterator> begin() {
        return Iterator<typename Range::iterator>(range_, predicate_, false);
    }

    Iterator<typename Range::iterator> end() {
        return Iterator<typename Range::iterator>(range_, predicate_, true);
    }

	Iterator<typename Range::iterator> begin() const {
        return Iterator<typename Range::iterator>(range_, predicate_, false);
    }

    Iterator<typename Range::iterator> end() const {
        return Iterator<typename Range::iterator>(range_, predicate_, true);
    }
};


template<typename Predicate>
class Transform : public Adapter {
private:
	Predicate predicate_;
public:
	Transform(Predicate predicate)
		: predicate_(predicate)
	{}

	template<typename Range>
	auto operator()(const Range& range) const {
		return TransformAdapter<Range, Predicate>(predicate_, range);
	}
};