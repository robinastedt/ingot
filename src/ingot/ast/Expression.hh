#pragma once

#include <ingot/ast/Integer.hh>
#include <ingot/ast/String.hh>
#include <ingot/ast/Operator.hh>
#include <ingot/ast/FunctionCall.hh>
#include <ingot/ast/Type.hh>
#include <ingot/ast/ArgumentReference.hh>

#include <variant>
#include <exception>

namespace ingot::ast
{

    // helper type for std::visit
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

    // explicit deduction guide (not needed as of C++20)
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    using ExpressionVariant = std::variant<std::monostate, Integer, String, Operator, FunctionCall, ArgumentReference>;
    class Expression : public ExpressionVariant {
        using ExpressionVariant::ExpressionVariant;
        friend std::ostream& operator<<(std::ostream&, const Expression&);

    public:
        template<typename T>
        class Visitor {
        public:
            virtual ~Visitor() {}
            virtual T operator()(const Integer& i) = 0;
            virtual T operator()(const String& str) = 0;
            virtual T operator()(const Operator& op, T lhsResult, T rhsResult) = 0;
            virtual T operator()(const FunctionCall& func, const std::vector<T>& argResults) = 0;
            virtual T operator()(const ArgumentReference& arg) = 0;
            T operator()(const std::monostate&) { throw std::runtime_error("Unexpected monostate in Expression"); }
        };

        template<typename T = std::monostate>
        class UpdateVisitor {
        public:
            enum class OperatorSide {
                LEFT,
                RIGHT
            };
            
            virtual ~UpdateVisitor() {}
            virtual T preop(Operator& op, const T& input, OperatorSide side) {
                return input;
            }
            virtual T preop(FunctionCall& func, const T& input, size_t index) {
                return input;
            }
            virtual void operator()(Integer& i, const T& input) = 0;
            virtual void operator()(String& str, const T& input) = 0;
            virtual void operator()(Operator& op, const T& input) = 0;
            virtual void operator()(FunctionCall& func, const T& input) = 0;
            virtual void operator()(ArgumentReference& arg, const T& input) = 0;
            void operator()(const std::monostate&, const T&) { throw std::runtime_error("Unexpected monostate in Expression"); }
        };

        template<typename T>
        T
        reduce(Visitor<T>&& visitor) const {
            return reduce<T>(visitor);
        }

        template<typename T>
        T
        reduce(Visitor<T>& visitor) const {
            return std::visit(overloaded{
                [&](const auto& expr) -> T { return visitor(expr); },
                [&](const FunctionCall& funcCall) -> T {
                    std::vector<T> argumentResults;
                    for (const Expression& argument : funcCall.getArguments()) {
                        argumentResults.emplace_back(argument.reduce(visitor));
                    }
                    return visitor(funcCall, argumentResults);
                },
                [&](const Operator& op) -> T {
                    T lhsRes = op.getLhs().reduce(visitor);
                    T rhsRes = op.getRhs().reduce(visitor);
                    return visitor(op, lhsRes, rhsRes);
                }
            }, static_cast<const ExpressionVariant&>(*this));
        }

    template<typename T = std::monostate>
    void
    update(UpdateVisitor<T>& visitor, const T& input = T{}) {
        return std::visit(overloaded{
            [&](auto& expr) { return visitor(expr, input); },
            [&](FunctionCall& funcCall) {
                    std::vector<Expression>& args = funcCall.getArguments();
                    size_t numArgs = args.size();
                    std::vector<T> outputs;
                    for (size_t i = 0; i < numArgs; ++i) {
                        outputs.push_back(visitor.preop(funcCall, input, i));
                    }
                    for (size_t i = 0; i < numArgs; ++i) {
                        args[i].update(visitor, outputs[i]);
                    }
                    return visitor(funcCall, input);
            },
            [&](Operator& op) {
                T outputLeft = visitor.preop(op, input, UpdateVisitor<T>::OperatorSide::LEFT);
                T outputRight = visitor.preop(op, input, UpdateVisitor<T>::OperatorSide::RIGHT);
                op.getLhs().update(visitor, outputLeft);
                op.getRhs().update(visitor, outputRight);
                visitor(op, input);
            }
        }, static_cast<ExpressionVariant&>(*this));
    }

        void setLocation(ingot::parser::location location);
        const parser::location& getLocation() const;
    };

    std::ostream& operator<<(std::ostream& str, const Expression& expr);
} // namespace ingot

