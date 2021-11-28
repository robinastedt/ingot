#pragma once

#include <ingot/ast/Expression.hh>

namespace ingot::semantics
{
    class IntegerTypeResolver : public ast::Expression::UpdateVisitor<size_t> {
        size_t preop(ast::Operator& op, const size_t& input, OperatorSide side) override;
        size_t preop(ast::FunctionCall& func, const size_t& input, size_t index) override;
        void operator()(ast::Integer& i, const size_t& input) override;
        void operator()(ast::String& str, const size_t& input) override;
        void operator()(ast::Operator& op, const size_t& input) override;
        void operator()(ast::FunctionCall& func, const size_t& input) override;
        void operator()(ast::ArgumentReference& arg, const size_t& input) override;
    };
} // namespace ingot::semantics
