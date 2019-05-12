#include <iostream>

#include <yaml-cpp/yaml.h>

#include <jaegertracing/Tracer.h>
#include <jaegertracing/context/context.h>
#include <jaegertracing/context/with_context.h>

namespace {

void setUpTracer(const char* configFilePath)
{
    auto configYAML = YAML::LoadFile(configFilePath);
    auto config = jaegertracing::Config::parse(configYAML);
    auto tracer = jaegertracing::Tracer::make(
        "example-service", config, jaegertracing::logging::consoleLogger());
    opentracing::Tracer::InitGlobal(
        std::static_pointer_cast<opentracing::Tracer>(tracer));
}

void tracedSubroutine()
{
    jaegertracing::Context current = jaegertracing::Context::Current();
    auto span = opentracing::Tracer::Global()->StartSpan(__FUNCTION__, 
    { opentracing::ChildOf(&current.GetSpanContext()) });
}

void tracedSubroutine2()
{
    jaegertracing::Context current = jaegertracing::Context::Current();
    auto span = opentracing::Tracer::Global()->StartSpan(__FUNCTION__, 
    { opentracing::ChildOf(&current.GetSpanContext()) });
}

void tracedFunction()
{
    auto span = opentracing::Tracer::Global()->StartSpan(__FUNCTION__);
    const jaegertracing::SpanContext* spancontext = dynamic_cast<const jaegertracing::SpanContext*>(&span->context());
    jaegertracing::WithSpanContext wsc(*spancontext);
    tracedSubroutine();
    tracedSubroutine2();
}

}  // anonymous namespace

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <config-yaml-path>\n";
        return 1;
    }
    setUpTracer(argv[1]);
    tracedFunction();
    // Not stricly necessary to close tracer, but might flush any buffered
    // spans. See more details in opentracing::Tracer::Close() documentation.
    opentracing::Tracer::Global()->Close();
    return 0;
}
