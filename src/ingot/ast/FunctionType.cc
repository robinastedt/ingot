#include "FunctionType.hh"

#include <iostream>

namespace ingot::ast
{
    FunctionType::FunctionType(ast::Type returnType, std::vector<ast::Type> argumentTypes)
    : Node()
    , m_returnType(std::move(returnType))
    , m_argumentTypes(std::move(argumentTypes)) {}

    const Type&
    FunctionType::getReturnType() const {
        return m_returnType;
    }

    std::ostream& operator<<(std::ostream& str, const FunctionType& functionType) {
        str << "(";
        bool first = true;
        for (const Type& type : functionType.m_argumentTypes) {
            if (!first) {
                str << ",";
            }
            str << type;
            first = false;
        }
        str << ")->" << functionType.m_returnType;
        return str;
    }
} // namespace ingot::ast
