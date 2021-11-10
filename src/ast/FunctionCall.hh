#pragma once

#include <ast/FunctionPrototype.hh>

namespace ingot::ast
{
    class FunctionCall {
        FunctionPrototype m_prototype;
        // TODO: Handle arguments

        friend std::ostream& operator<<(std::ostream& str, const FunctionCall& functionCall);
    public:
        FunctionCall(FunctionPrototype prototype);
        const FunctionPrototype& getPrototype() const;
    };

    std::ostream& operator<<(std::ostream& str, const FunctionCall& functionCall);
} // namespace ingot::ast
