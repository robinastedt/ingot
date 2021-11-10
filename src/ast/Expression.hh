#pragma once

#include "Integer.hh"
#include "Operator.hh"
#include "FunctionCall.hh"

#include <variant>
#include <exception>

namespace ingot::ast
{

    // helper type for std::visit
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

    // explicit deduction guide (not needed as of C++20)
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    using ExpressionVariant = std::variant<std::monostate, Integer, Operator, FunctionCall>;
    class Expression : public ExpressionVariant {
        using ExpressionVariant::ExpressionVariant;
        friend std::ostream& operator<<(std::ostream&, const Expression&);

    public:
        template<typename T>
        class Visitor {
        public:
            virtual ~Visitor() {}
            virtual T operator()(const Integer& i) = 0;
            virtual T operator()(const Operator& op, T lhsResult, T rhsResult) = 0;
            virtual T operator()(const FunctionCall& func) = 0;
            T operator()(const std::monostate&) { throw std::runtime_error("Unexpected monostate in Expression"); }
        };

        template<typename T>
        T
        reduce(Visitor<T>& visitor) const {
            return std::visit(overloaded{
                [&](const auto& expr) -> T { return visitor(expr); },
                [&](const Operator& op) -> T {
                    T lhsRes = op.getLhs().reduce(visitor);
                    T rhsRes = op.getRhs().reduce(visitor);
                    return visitor(op, lhsRes, rhsRes);
                }
            }, static_cast<const ExpressionVariant&>(*this));
        }
    };

    std::ostream& operator<<(std::ostream& str, const Expression& expr);
} // namespace ingot

