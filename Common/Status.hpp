#pragma once

namespace zrn
{

enum class ErrorType
{
    None                = 0,
    NoError             = None,
    
    // API errores
    WrongArguments,
    ContextNotProvided,

    // Graphics Device errors
    ResourceCreation,
    ShaderCompilation,
    ShaderLinking,
    FramebufferIncomplete,
};

struct Status
{
public:
    Status() = default;
    Status(ErrorType error)
        : Error(error) { }
    Status(ErrorType error, const char* message)
        : Error(error), Message(message) { }
    
    ErrorType   Error = ErrorType::None;
    const char* Message;

    operator ErrorType() { return Error; }
    operator bool() { return Error == ErrorType::NoError; }
    // static const char* Message() { return "Not implemented yet"; }
};

} // namespace zrn