#pragma once

#include <fstream>

#include "base_adapter.h"

template<typename Range>
class OpenFilesAdapter : public Adapter {
private:
    Range range_;

public:
    template<typename IteratorType>
    class Iterator {
    private:
        IteratorType it_;
        IteratorType end_;
        std::shared_ptr<std::ifstream> file;

        void CloseSelf() {
            if (!file) {
                return;
            }
            file.reset();
        }

    public:
        ~Iterator() {
            CloseSelf();
        }
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

        std::ifstream& operator*() {
            CloseSelf();
            file = std::make_shared<std::ifstream>(*it_);
            return *file;
        }

        Iterator& operator++() {
            CloseSelf();
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
    using value_type = typename std::ifstream;

    OpenFilesAdapter(const Range& range)
        : range_(range) {
    }

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


class OpenFiles : public Adapter {
public:
    OpenFiles() = default;

    template<typename Range>
    auto operator()(const Range& range) const {
        return OpenFilesAdapter<Range>(range);
    }
};