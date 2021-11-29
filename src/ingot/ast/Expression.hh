#pragma once

#include <ingot/ast/Integer.hh>
#include <ingot/ast/List.hh>
#include <ingot/ast/Operator.hh>
#include <ingot/ast/FunctionCall.hh>
#include <ingot/ast/Type.hh>
#include <ingot/ast/ArgumentReference.hh>
#include <ingot/ast/Ternary.hh>

#include <variant>
#include <exception>

namespace ingot::ast
{

    // helper type for std::visit
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

    // explicit deduction guide (not needed as of C++20)
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    using ExpressionVariant = std::variant<std::monostate, Integer, List, Operator, FunctionCall, ArgumentReference, Ternary>;
    class Expression : public ExpressionVariant {
        using ExpressionVariant::ExpressionVariant;
        friend std::ostream& operator<<(std::ostream&, const Expression&);

    public:
        template<typename OUTPUT = std::monostate, typename INPUT = std::monostate>
        class Visitor {
        public:
            enum class OperatorSide {
                LEFT,
                RIGHT
            };
            enum class TernaryPosition {
                CONDITION,
                TRUE_BRANCH,
                FALSE_BRANCH
            };
            virtual ~Visitor() {}
            virtual INPUT preop(List& func, INPUT input, size_t index) {
                return input;
            }
            virtual INPUT preop(Operator& op, INPUT input, OperatorSide side) {
                return input;
            }
            virtual INPUT preop(FunctionCall& func, INPUT input, size_t index) {
                return input;
            }
            virtual INPUT preop(Ternary& func, INPUT input, TernaryPosition position) {
                return input;
            }
            virtual OUTPUT postop(Integer& i, INPUT input) = 0;
            virtual OUTPUT postop(List& str, const std::vector<OUTPUT>& elemResults, INPUT input) = 0;
            virtual OUTPUT postop(Operator& op, const std::pair<OUTPUT, OUTPUT>& argResults, INPUT input) = 0;
            virtual OUTPUT postop(FunctionCall& func, const std::vector<OUTPUT>& argResults, INPUT input) = 0;
            virtual OUTPUT postop(ArgumentReference& arg, INPUT input) = 0;
            virtual OUTPUT postop(Ternary& ternary, const std::tuple<OUTPUT, OUTPUT, OUTPUT>& results, INPUT input) = 0;
            OUTPUT postop(std::monostate&, INPUT input) { throw std::runtime_error("Unexpected monostate in Expression"); }
        };

        template<typename OUTPUT = std::monostate, typename INPUT = std::monostate>
        class ConstVisitor {
        public:
            enum class OperatorSide {
                LEFT,
                RIGHT
            };
            enum class TernaryPosition {
                CONDITION,
                TRUE_BRANCH,
                FALSE_BRANCH
            };
            virtual ~ConstVisitor() {}
            virtual INPUT preop(const List& func, INPUT input, size_t index) {
                return input;
            }
            virtual INPUT preop(const Operator& op, INPUT input, OperatorSide side) {
                return input;
            }
            virtual INPUT preop(const FunctionCall& func, INPUT input, size_t index)  {
                return input;
            }
            virtual INPUT preop(const Ternary& func, INPUT input, TernaryPosition position) {
                return input;
            }
            virtual OUTPUT postop(const Integer& i, INPUT input)  = 0;
            virtual OUTPUT postop(const List& str, const std::vector<OUTPUT>& elemResults, INPUT input)  = 0;
            virtual OUTPUT postop(const Operator& op, const std::pair<OUTPUT, OUTPUT>& argResults, INPUT input)  = 0;
            virtual OUTPUT postop(const FunctionCall& func, const std::vector<OUTPUT>& argResults, INPUT input)  = 0;
            virtual OUTPUT postop(const ArgumentReference& arg, INPUT input)  = 0;
            virtual OUTPUT postop(const Ternary& ternary, const std::tuple<OUTPUT, OUTPUT, OUTPUT>& results, INPUT input) = 0;
            OUTPUT postop(const std::monostate&, INPUT input) { throw std::runtime_error("Unexpected monostate in Expression"); }
        };

        template<typename OUTPUT = std::monostate, typename INPUT = std::monostate>
        OUTPUT
        traverse(Visitor<OUTPUT, INPUT>& visitor, INPUT input = INPUT{}) {
            return std::visit(overloaded{
                [&](auto& expr) { return visitor.postop(expr, input); },
                [&](FunctionCall& funcCall) {
                        std::vector<Expression>& args = funcCall.getArguments();
                        std::vector<OUTPUT> outputs;
                        for (size_t i = 0; i < args.size(); ++i) {
                            outputs.push_back(args[i].traverse(visitor, visitor.preop(funcCall, input, i)));
                        }
                        return visitor.postop(funcCall, outputs, input);
                },
                [&](List& list) {
                        std::vector<Expression>& elements = list.getElements();
                        std::vector<OUTPUT> outputs;
                        for (size_t i = 0; i < elements.size(); ++i) {
                            outputs.push_back(elements[i].traverse(visitor, visitor.preop(list, input, i)));
                        }
                        return visitor.postop(list, outputs, input);
                },
                [&](Operator& op) {
                    std::pair<OUTPUT, OUTPUT> outputs = {
                        op.getLhs().traverse(visitor, visitor.preop(op, input, Visitor<OUTPUT, INPUT>::OperatorSide::LEFT)),
                        op.getRhs().traverse(visitor, visitor.preop(op, input, Visitor<OUTPUT, INPUT>::OperatorSide::RIGHT))
                    };
                    return visitor.postop(op, outputs, input);
                },
                [&](Ternary& ternary) {
                    std::tuple<OUTPUT, OUTPUT, OUTPUT> outputs = {
                        ternary.getCondition().traverse(visitor, visitor.preop(ternary, input, Visitor<OUTPUT, INPUT>::TernaryPosition::CONDITION)),
                        ternary.getTrueBranch().traverse(visitor, visitor.preop(ternary, input, Visitor<OUTPUT, INPUT>::TernaryPosition::TRUE_BRANCH)),
                        ternary.getFalseBranch().traverse(visitor, visitor.preop(ternary, input, Visitor<OUTPUT, INPUT>::TernaryPosition::FALSE_BRANCH))
                    };
                    return visitor.postop(ternary, outputs, input);
                }
            }, static_cast<ExpressionVariant&>(*this));
        }

        template<typename OUTPUT = std::monostate, typename INPUT = std::monostate>
        OUTPUT
        traverse(ConstVisitor<OUTPUT, INPUT>& visitor, INPUT input = INPUT{}) const {
            return std::visit(overloaded{
                [&](const auto& expr) { return visitor.postop(expr, input); },
                [&](const FunctionCall& funcCall) {
                        const std::vector<Expression>& args = funcCall.getArguments();
                        std::vector<OUTPUT> outputs;
                        for (size_t i = 0; i < args.size(); ++i) {
                            outputs.push_back(args[i].traverse(visitor, visitor.preop(funcCall, input, i)));
                        }
                        return visitor.postop(funcCall, outputs, input);
                },
                [&](const List& list) {
                        const std::vector<Expression>& elements = list.getElements();
                        std::vector<OUTPUT> outputs;
                        for (size_t i = 0; i < elements.size(); ++i) {
                            outputs.push_back(elements[i].traverse(visitor, visitor.preop(list, input, i)));
                        }
                        return visitor.postop(list, outputs, input);
                },
                [&](const Operator& op) {
                    std::pair<OUTPUT, OUTPUT> outputs = {
                        op.getLhs().traverse(visitor, visitor.preop(op, input, ConstVisitor<OUTPUT, INPUT>::OperatorSide::LEFT)),
                        op.getRhs().traverse(visitor, visitor.preop(op, input, ConstVisitor<OUTPUT, INPUT>::OperatorSide::RIGHT))
                    };
                    return visitor.postop(op, outputs, input);
                },
                [&](const Ternary& ternary) {
                    std::tuple<OUTPUT, OUTPUT, OUTPUT> outputs = {
                        ternary.getCondition().traverse(visitor, visitor.preop(ternary, input, ConstVisitor<OUTPUT, INPUT>::TernaryPosition::CONDITION)),
                        ternary.getTrueBranch().traverse(visitor, visitor.preop(ternary, input, ConstVisitor<OUTPUT, INPUT>::TernaryPosition::TRUE_BRANCH)),
                        ternary.getFalseBranch().traverse(visitor, visitor.preop(ternary, input, ConstVisitor<OUTPUT, INPUT>::TernaryPosition::FALSE_BRANCH))
                    };
                    return visitor.postop(ternary, outputs, input);
                }
            }, static_cast<const ExpressionVariant&>(*this));
        }

        void setLocation(ingot::parser::location location);
        const parser::location& getLocation() const;
    };

    std::ostream& operator<<(std::ostream& str, const Expression& expr);
} // namespace ingot

