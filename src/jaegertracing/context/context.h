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

#ifndef OPENCENSUS_CONTEXT_CONTEXT_H_
#define OPENCENSUS_CONTEXT_CONTEXT_H_

#include <functional>
#include <string>

#include "jaegertracing/SpanContext.h"

namespace jaegertracing {

// Context holds information specific to an operation, such as a TagMap and
// Span. Each thread has a currently active Context. Contexts are conceptually
// immutable: the contents of a Context cannot be modified in-place.
//
// This is a draft implementation of Context, and we chose to depend on TagMap
// and Span directly. In future, the implementation will change, so only rely
// on the public API for manipulating Contexts. In future we may support
// arbitrary keys and values.
class Context {
 public:
  // Returns a const reference to the current (thread local) Context.
  static Context& Current();

  // Context is copiable and movable.
  Context(const Context&) = default;
  Context(Context&&) = default;
  Context& operator=(const Context&) = default;
  Context& operator=(Context&&) = default;

  // Returns an std::function wrapped to run with a copy of this Context.
  std::function<void()> Wrap(std::function<void()> fn) const;

  // Returns a human-readable string for debugging. Do not rely on its format or
  // try to parse it. Do not use the DebugString to retrieve Spans or Tags.
  std::string DebugString() const;

  const SpanContext& GetSpanContext() const;
  void SetSpanContext(const SpanContext& spancontext);

 private:
  // Creates a default Context.
  Context();

  static Context* InternalMutableCurrent();
  friend void swap(Context& a, Context& b);

  friend class WithSpanContext;
  SpanContext spancontext_;

};

}  // namespace jaegertracing

#endif  // OPENCENSUS_CONTEXT_CONTEXT_H_
