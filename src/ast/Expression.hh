#pragma once

#include <variant>
#include "Integer.hh"
#include "Operator.hh"

namespace ingot::ast
{
    // helper type for std::visit
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

    // explicit deduction guide (not needed as of C++20)
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    using ExpressionVariant = std::variant<std::monostate, Integer, Operator>;
    class Expression : public ExpressionVariant {
        using ExpressionVariant::ExpressionVariant;
        friend std::ostream& operator<<(std::ostream&, const Expression&);
    };

    std::ostream& operator<<(std::ostream& str, const Expression& expr);
} // namespace ingot

