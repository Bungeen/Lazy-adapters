#pragma once

#include "base_adapter.h"

template<typename Range>
class SplitExpectedAdapter : public Adapter {
private:
    Range range_;

public:
	SplitExpectedAdapter(const Range& range)
        : range_(range)
	{}

	template<typename IteratorType>
    class Iterator {
    private:
		IteratorType it_;
		IteratorType end_;

        void GetNext() {
            while (it_ != end_ && !(*it_).has_value()) {
                ++it_;
            }
        }

    public:
        Iterator(Range& range, bool is_end)
			: it_(range.end())
			, end_(range.end()) {
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
			}

            GetNext();
        }

		Iterator(const Range& range, bool is_end)
			: it_(range.end())
			, end_(range.end()) {
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
			}

            GetNext();
        }

        auto operator*() {
            return (*it_).value();
        }

        Iterator& operator++() {
			++it_;
			GetNext();

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

template<typename Range>
class SplitUnexpectedAdapter : public Adapter {
private:
    Range range_;

public:
	SplitUnexpectedAdapter(const Range& range)
        : range_(range)
	{}

	template<typename IteratorType>
    class Iterator {
    private:
		IteratorType it_;
		IteratorType end_;

        void GetNext() {
            while (it_ != end_ && (*it_).has_value()) {
                ++it_;
            }
        }

    public:
        Iterator(Range& range, bool is_end)
			: it_(range.end())
			, end_(range.end()) {
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
			}

            GetNext();
        }

		Iterator(const Range& range, bool is_end)
			: it_(range.end())
			, end_(range.end()) {
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
			}

            GetNext();
        }

        auto operator*() {
            return (*it_).error();
        }

        Iterator& operator++() {
			++it_;
			GetNext();

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


template<typename Predicate>
class SplitExpected : public Adapter {
private:
	Predicate predicate_;
public:
	SplitExpected(Predicate predicate)
		: predicate_(predicate)
	{}

	template<typename Range>
	auto operator()(const Range& range) const {
		return std::make_pair(
            SplitUnexpectedAdapter<Range>(range),
			SplitExpectedAdapter<Range>(range)
		);
	}
};


class DropNullopt : public Adapter {
public:
	template<typename Range>
	auto operator()(const Range& range) const {
		return SplitExpectedAdapter(range);
	}
};