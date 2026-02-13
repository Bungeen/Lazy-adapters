#pragma once

#include <iostream>

#include "base_adapter.h"

template<typename Range>
class WriteAdapter {
private:
    Range range_;

public:
	WriteAdapter(const Range& range, std::ostream& stream, char ch)
        : range_(range) 
	{
		for (auto item : range_) {
			stream << item << ch;
		}
		
	}
};


class Out : public Adapter {
private:
    std::ostream& stream_;
public:
    Out(std::ostream& stream)
        : stream_(stream)
    {}

    template<typename Range>
    auto operator()(const Range& range) const {
        return WriteAdapter<Range>(range, stream_, ' ');
    }
};

class Write : public Adapter {
private:
    std::ostream& stream_;
    char ch_;
public:
    Write(std::ostream& stream, char ch)
        : stream_(stream)
        , ch_(ch)
    {}

    template<typename Range>
    auto operator()(const Range& range) const {
        return WriteAdapter<Range>(range, stream_, ch_);
    }
};