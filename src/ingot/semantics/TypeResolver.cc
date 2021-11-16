#include "TypeResolver.hh"

#include <ingot/semantics/SemanticError.hh>
#include <sstream>

namespace ingot::semantics
{
    TypeResolver::TypeResolver() {}

    ast::Type
    TypeResolver::operator()(const ast::Integer& i) {
        return i.getType();
    }

    ast::Type
    TypeResolver::operator()(const ast::String& str) {
        return str.getType();
    }

    ast::Type
    TypeResolver::operator()(const ast::Operator& op, ast::Type lhsResult, ast::Type rhsResult) {
        if (lhsResult != rhsResult) {
            std::stringstream ss;
            ss << "In expression '" << op << "': Type of lhs '" << lhsResult << "' does not match type of rhs '" << rhsResult << "'";
            throw SemanticError(ss.str());
        }
        return lhsResult;
    }

    ast::Type
    TypeResolver::operator()(const ast::FunctionCall& func) {
        return func.getReturnType();
    }

} // namespace ingot::semantics