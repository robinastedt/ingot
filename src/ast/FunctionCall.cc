#include "FunctionCall.hh"

namespace ingot::ast
{
    FunctionCall::FunctionCall(FunctionPrototype prototype)
    : m_prototype(std::move(prototype)) {}

    const FunctionPrototype&
    FunctionCall::getPrototype() const {
        return m_prototype;
    }

    std::ostream& operator<<(std::ostream& str, const FunctionCall& functionCall) {
        return str << functionCall.m_prototype;
    }
} // namespace ingot::ast
