#pragma once

#include <stdexcept>

namespace ingot::semantics
{
    class SemanticError : public std::runtime_error {
    public:
        SemanticError(const std::string& what);
    };
} // namespace ingot::semantics
