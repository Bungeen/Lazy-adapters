#pragma once

#include "base_adapter.h"
#include "dir_adapter.h"
#include "openfiles_adapter.h"
#include "split_adapter.h"
#include "write_adapters.h"
#include "asvector_adapter.h"
#include "asdata_flow.h"
#include "filter_adapter.h"
#include "transform_adapter.h"
#include "expect_adapters.h"
#include "join_adapter.h"
#include "aggregate_adapter.h"

template<typename Range, typename AdapterType>
requires std::derived_from<AdapterType, Adapter> && std::derived_from<Range, Adapter>
auto operator|(const Range& range, const AdapterType& openFiles) {
    return openFiles(range);
}
