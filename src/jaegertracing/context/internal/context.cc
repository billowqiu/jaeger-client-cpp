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

#include "jaegertracing/context/context.h"

#include <functional>
#include <utility>
#include <sstream>

#include "jaegertracing/context/with_context.h"

namespace jaegertracing {

Context::Context()
{
}

// static
Context& Context::Current() { return *InternalMutableCurrent(); }

std::function<void()> Context::Wrap(std::function<void()> fn) const {
  SpanContext copy(Context::InternalMutableCurrent()->spancontext_);
  return [fn, copy]() {
    WithSpanContext wsc(copy);
    fn();
  };
}

std::string Context::DebugString() const {
  // return absl::StrCat("ctx@", absl::Hex(this),
  //                     " span=", span_.context().ToString(),
  //                     ", tags=", tags_.DebugString());
  std::ostringstream oss;
  spancontext_.print(oss);
  return oss.str();
}

const SpanContext& Context::GetSpanContext() const {
  return spancontext_;
}

void Context::SetSpanContext(const SpanContext& spancontext) {
  spancontext_ = spancontext;
}

// static
Context* Context::InternalMutableCurrent() {
  static thread_local Context* thread_ctx = nullptr;
  if (thread_ctx == nullptr) thread_ctx = new Context;
  return thread_ctx;
}

void swap(Context& a, Context& b) {
  using std::swap;
  swap(a.spancontext_, b.spancontext_);
}

}  // namespace jaegertracing
