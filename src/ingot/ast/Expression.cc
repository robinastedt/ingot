#include "Expression.hh"

namespace ingot::ast
{
    std::ostream& operator<<(std::ostream& str, const Expression& expr) {
        return std::visit([&](const auto& val) -> std::ostream& { return str << val; }
            , static_cast<const ExpressionVariant&>(expr));
    }
} // namespace ingot::ast

