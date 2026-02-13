#pragma once

#include <filesystem>
#include <string>

#include <iostream>
#include "base_adapter.h"

class Dir : public Adapter {
private:
    std::string path_;
    bool flag_;
    
public:
    using FSIterator = std::filesystem::directory_iterator;
    using FSRIterator = std::filesystem::recursive_directory_iterator;

    class Iterator {
    public:
        Iterator(FSIterator current, FSIterator end, bool flag)
            : end_(end)
            , current_(current)
            , flag_(flag)
        {}

        Iterator(FSRIterator current, FSRIterator end, bool flag)
            : end_r_(end)
            , current_r_(current)
            , flag_(flag)
        {}

        std::filesystem::path& operator*() {
            if (flag_) {
                current_path_ = current_r_->path();
                return current_path_;
            }

            current_path_ = current_->path();
            return current_path_;
        }

        Iterator& operator++() {
            if (flag_) {
                ++current_r_;
                return *this;
            }

            ++current_;
            return *this;
        }

        bool operator==(const Iterator& rhs) const {
            if (flag_) {
                return current_r_ == rhs.current_r_;
            }

            return current_ == rhs.current_;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }
    private:
        FSIterator current_;
        FSIterator end_;
        FSRIterator current_r_;
        FSRIterator end_r_;
        bool flag_;
        std::filesystem::path current_path_;
    };

    using iterator = Iterator;
    using value_type = std::filesystem::path;
    Dir(const std::string& path, bool flag=true)
        : path_(path)
        , flag_(flag)
    {}

    Dir()
        : path_("")
        , flag_(true)
    {}

    Iterator begin() {
        if (flag_) {
            return Iterator(FSRIterator(path_), FSRIterator(), true);
        }

        return Iterator(FSIterator(path_), FSIterator(), false);
    }

    Iterator end() {
        if (flag_) {
            return Iterator(FSRIterator(), FSRIterator(), true);
        }

        return Iterator(FSIterator(), FSIterator(), false);
    }

    Iterator begin() const {
        if (flag_) {
            return Iterator(FSRIterator(path_), FSRIterator(), true);
        }

        return Iterator(FSIterator(path_), FSIterator(), false);
    }

    Iterator end() const {
        if (flag_) {
            return Iterator(FSRIterator(), FSRIterator(), true);
        }

        return Iterator(FSIterator(), FSIterator(), false);
    }
};