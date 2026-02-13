#pragma once

#include <string>
#include <unordered_set>

#include "base_adapter.h"

template<typename Range>
class SplitAdapter : public Adapter {
private:
    Range range_;
	std::string chars_str_;

public:
	template<typename IteratorType, typename Stream>
    class Iterator {
    private:
		IteratorType it_;
		IteratorType end_;
		std::string local_str;
		std::unordered_set<char> chars_base_;
		Stream* stream_ = nullptr;

		void extract_next() {
            local_str.clear();
			char ch;

			while (stream_ && stream_->get(ch)) {
				if (chars_base_.count(ch) == 0) {
					local_str += ch;
				} else {
					return;
				}
			}
			
			if (!local_str.empty()) {
				return;
			}

			stream_->clear();
			stream_->seekg(0);
			++it_;
			if (it_ != end_) {
				stream_ = &(*it_);
				extract_next();
			} else {
                stream_ = nullptr;
            }
        }

    public:
        Iterator(Range& range, const std::string& chars_str, bool is_end)
			: it_(range.end())
			, end_(range.end()) {
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
				chars_base_ = std::unordered_set<char>(chars_str.begin(),
                                                        chars_str.end());
				if (it_ != end_) {
					stream_ = &(*it_);
					extract_next();
				}
			}
		}

		Iterator(const Range& range, const std::string& chars_str, bool is_end)
			: it_(range.end())
			, end_(range.end()) {
			if (!is_end) {
				it_ = range.begin();
				end_ = range.end();
				chars_base_ = std::unordered_set<char>(chars_str.begin(),
                                                        chars_str.end());
				
				if (it_ != end_) {
					stream_ = &(*it_);
					extract_next();
				}
			}
		}

        std::string& operator*() {
            return local_str;
        }

        Iterator& operator++() {
			extract_next();
            return *this;
        }

        bool operator==(const Iterator& rhs) const {
            return it_ == rhs.it_ && local_str == rhs.local_str;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }
    };

	using iterator = Iterator<typename Range::iterator,
                                typename Range::value_type>;
	using value_type = typename std::string;

    SplitAdapter(const Range& range, const std::string& splitters)
        : range_(range)
		, chars_str_(splitters) {}

    iterator begin() {
        return iterator(range_, chars_str_, false);
    }

    iterator end() {
        return iterator(range_, chars_str_, true);
    }

	iterator begin() const {
        return iterator(range_, chars_str_, false);
    }

    iterator end() const {
        return iterator(range_, chars_str_, true);
    }
};


class Split : public Adapter {
private:
    std::string splitters_;
public:
    Split(std::string splitters)
        : splitters_(splitters)
    {}

    template<typename Range>
    auto operator()(const Range& range) const {
        return SplitAdapter<Range>(range, splitters_);
    }
};