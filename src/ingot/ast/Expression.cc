#include "Expression.hh"

#include <ingot/Error.hh>

namespace ingot::ast
{
    void
    Expression::update(UpdateVisitor&& visitor) {
        update(visitor);
    }

    void
    Expression::update(UpdateVisitor& visitor) {
        return std::visit(overloaded{
            [&](auto& expr) { return visitor(expr); },
            [&](Operator& op) {
                op.getLhs().update(visitor);
                op.getRhs().update(visitor);
                visitor(op);
            }
        }, static_cast<ExpressionVariant&>(*this));
    }

    void
    Expression::setLocation(parser::location location) {
        std::visit(overloaded{
            [](std::monostate&){throw internal_error("Found monostate in expression."); },
            [=](auto& expr){ expr.setLocation(location);}
         }, static_cast<ExpressionVariant&>(*this));
    }

    const parser::location&
    Expression::getLocation() const {
        return std::visit(overloaded{
            [](const auto& expr) -> const parser::location& { return expr.getLocation();},
            [](const std::monostate&) -> const parser::location& {throw internal_error("Found monostate in expression."); }
            
         }, static_cast<const ExpressionVariant&>(*this));
    }

    std::ostream& operator<<(std::ostream& str, const Expression& expr) {
        return std::visit([&](const auto& val) -> std::ostream& { return str << val; }
            , static_cast<const ExpressionVariant&>(expr));
    }
} // namespace ingot::ast

