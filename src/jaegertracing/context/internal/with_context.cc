// Copyright 2018, OpenCensus Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "jaegertracing/context/with_context.h"

#include <cassert>
#include <utility>

#include "jaegertracing/context/context.h"

namespace jaegertracing {

WithSpanContext::WithSpanContext(const SpanContext& ctx, bool cond)
    : swapped_context_(cond ? ctx : SpanContext())
#ifndef NDEBUG
      ,
      original_context_(Context::InternalMutableCurrent())
#endif
      ,
      cond_(cond) {
  ConditionalSwap();
}

WithSpanContext::WithSpanContext(SpanContext&& ctx, bool cond)
    : swapped_context_(cond ? std::move(ctx) : SpanContext())
#ifndef NDEBUG
      ,
      original_context_(Context::InternalMutableCurrent())
#endif
      ,
      cond_(cond) {
  ConditionalSwap();
}

WithSpanContext::~WithSpanContext() {
#ifndef NDEBUG
  assert(original_context_ == Context::InternalMutableCurrent() &&
         "WithContext must be destructed on the same thread as it was "
         "constructed.");
#endif
  ConditionalSwap();
}

void WithSpanContext::ConditionalSwap() {
  if (cond_) {
    using std::swap;
    swap(Context::InternalMutableCurrent()->spancontext_, swapped_context_);
  }
}

}  // namespace jaegertracing
