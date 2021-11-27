#include "FunctionType.hh"

#include <ingot/Error.hh>

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

    const size_t
    FunctionType::getArgumentCount() const {
        return m_argumentTypes.size();
    }

    const Type&
    FunctionType::getArgumentType(size_t index) const {
        if (index >= m_argumentTypes.size()) {
            throw internal_error("Out of bounds type check. index=" + std::to_string(index) + ", size=" + std::to_string(m_argumentTypes.size()));
        }
        return m_argumentTypes[index];
    }

    const std::vector<ast::Type>&
    FunctionType::getArgumentTypes() const {
        return m_argumentTypes;
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
