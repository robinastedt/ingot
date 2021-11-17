#pragma once

#include <ingot/ast/Type.hh>

#include <vector>

namespace ingot::ast
{
    class FunctionType : public Node {
        ast::Type m_returnType;
        std::vector<ast::Type> m_argumentTypes;

        friend std::ostream& operator<<(std::ostream& str, const FunctionType& functionType);
    public:
        FunctionType() = default;
        FunctionType(ast::Type returnType, std::vector<ast::Type> argumentTypes);

        const Type& getReturnType() const;
    };

    std::ostream& operator<<(std::ostream& str, const FunctionType& functionType);
} // namespace ingot::ast
