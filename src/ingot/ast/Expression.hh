#pragma once

#include <ingot/ast/Integer.hh>
#include <ingot/ast/List.hh>
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

    using ExpressionVariant = std::variant<std::monostate, Integer, List, Operator, FunctionCall, ArgumentReference>;
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
            virtual ~Visitor() {}
            virtual INPUT preop(List& func, INPUT input, size_t index) const {
                return input;
            }
            virtual INPUT preop(Operator& op, INPUT input, OperatorSide side) const {
                return input;
            }
            virtual INPUT preop(FunctionCall& func, INPUT input, size_t index) const {
                return input;
            }
            virtual OUTPUT postop(Integer& i, INPUT input) const = 0;
            virtual OUTPUT postop(List& str, const std::vector<OUTPUT>& elemResults, INPUT input) const = 0;
            virtual OUTPUT postop(Operator& op, const std::pair<OUTPUT, OUTPUT>& argResults, INPUT input) const = 0;
            virtual OUTPUT postop(FunctionCall& func, const std::vector<OUTPUT>& argResults, INPUT input) const = 0;
            virtual OUTPUT postop(ArgumentReference& arg, INPUT input) const = 0;
            OUTPUT postop(std::monostate&, INPUT input) const { throw std::runtime_error("Unexpected monostate in Expression"); }
        };

        template<typename OUTPUT = std::monostate, typename INPUT = std::monostate>
        class ConstVisitor {
        public:
            enum class OperatorSide {
                LEFT,
                RIGHT
            };
            virtual ~ConstVisitor() {}
            virtual INPUT preop(const List& func, INPUT input, size_t index) const {
                return input;
            }
            virtual INPUT preop(const Operator& op, INPUT input, OperatorSide side) const {
                return input;
            }
            virtual INPUT preop(const FunctionCall& func, INPUT input, size_t index) const  {
                return input;
            }
            virtual OUTPUT postop(const Integer& i, INPUT input) const  = 0;
            virtual OUTPUT postop(const List& str, const std::vector<OUTPUT>& elemResults, INPUT input) const  = 0;
            virtual OUTPUT postop(const Operator& op, const std::pair<OUTPUT, OUTPUT>& argResults, INPUT input) const  = 0;
            virtual OUTPUT postop(const FunctionCall& func, const std::vector<OUTPUT>& argResults, INPUT input) const  = 0;
            virtual OUTPUT postop(const ArgumentReference& arg, INPUT input) const  = 0;
            OUTPUT postop(const std::monostate&, INPUT input) const  { throw std::runtime_error("Unexpected monostate in Expression"); }
        };

        template<typename OUTPUT = std::monostate, typename INPUT = std::monostate>
        OUTPUT
        traverse(const Visitor<OUTPUT, INPUT>& visitor, INPUT input = INPUT{}) {
            return std::visit(overloaded{
                [&](auto& expr) { return visitor.postop(expr, input); },
                [&](FunctionCall& funcCall) {
                        std::vector<Expression>& args = funcCall.getArguments();
                        size_t numArgs = args.size();
                        std::vector<INPUT> inputs;
                        for (size_t i = 0; i < numArgs; ++i) {
                            inputs.push_back(visitor.preop(funcCall, input, i));
                        }
                        std::vector<OUTPUT> outputs;
                        for (size_t i = 0; i < numArgs; ++i) {
                            outputs.push_back(args[i].traverse(visitor, inputs[i]));
                        }
                        return visitor.postop(funcCall, outputs, input);
                },
                [&](List& list) {
                        std::vector<Expression>& elements = list.getElements();
                        size_t numElems = elements.size();
                        std::vector<INPUT> inputs;
                        for (size_t i = 0; i < numElems; ++i) {
                            inputs.push_back(visitor.preop(list, input, i));
                        }
                        std::vector<OUTPUT> outputs;
                        for (size_t i = 0; i < numElems; ++i) {
                            outputs.push_back(elements[i].traverse(visitor, inputs[i]));
                        }
                        return visitor.postop(list, outputs, input);
                },
                [&](Operator& op) {
                    INPUT inputLeft = visitor.preop(op, input, Visitor<OUTPUT, INPUT>::OperatorSide::LEFT);
                    INPUT inputRight = visitor.preop(op, input, Visitor<OUTPUT, INPUT>::OperatorSide::RIGHT);
                    std::pair<OUTPUT, OUTPUT> outputs = {
                        op.getLhs().traverse(visitor, inputLeft),
                        op.getRhs().traverse(visitor, inputRight)
                    };
                    return visitor.postop(op, outputs, input);
                }
            }, static_cast<ExpressionVariant&>(*this));
        }

        template<typename OUTPUT = std::monostate, typename INPUT = std::monostate>
        OUTPUT
        traverse(const ConstVisitor<OUTPUT, INPUT>& visitor, INPUT input = INPUT{}) const {
            return std::visit(overloaded{
                [&](const auto& expr) { return visitor.postop(expr, input); },
                [&](const FunctionCall& funcCall) {
                        const std::vector<Expression>& args = funcCall.getArguments();
                        size_t numArgs = args.size();
                        std::vector<INPUT> inputs;
                        for (size_t i = 0; i < numArgs; ++i) {
                            inputs.push_back(visitor.preop(funcCall, input, i));
                        }
                        std::vector<OUTPUT> outputs;
                        for (size_t i = 0; i < numArgs; ++i) {
                            outputs.push_back(args[i].traverse(visitor, inputs[i]));
                        }
                        return visitor.postop(funcCall, outputs, input);
                },
                [&](const List& list) {
                        const std::vector<Expression>& elements = list.getElements();
                        size_t numElems = elements.size();
                        std::vector<INPUT> inputs;
                        for (size_t i = 0; i < numElems; ++i) {
                            inputs.push_back(visitor.preop(list, input, i));
                        }
                        std::vector<OUTPUT> outputs;
                        for (size_t i = 0; i < numElems; ++i) {
                            outputs.push_back(elements[i].traverse(visitor, inputs[i]));
                        }
                        return visitor.postop(list, outputs, input);
                },
                [&](const Operator& op) {
                    INPUT inputLeft = visitor.preop(op, input, ConstVisitor<OUTPUT, INPUT>::OperatorSide::LEFT);
                    INPUT inputRight = visitor.preop(op, input, ConstVisitor<OUTPUT, INPUT>::OperatorSide::RIGHT);
                    std::pair<OUTPUT, OUTPUT> outputs = {
                        op.getLhs().traverse(visitor, inputLeft),
                        op.getRhs().traverse(visitor, inputRight)
                    };
                    return visitor.postop(op, outputs, input);
                }
            }, static_cast<const ExpressionVariant&>(*this));
        }

        void setLocation(ingot::parser::location location);
        const parser::location& getLocation() const;
    };

    std::ostream& operator<<(std::ostream& str, const Expression& expr);
} // namespace ingot

