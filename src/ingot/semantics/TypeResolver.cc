#include "TypeResolver.hh"

#include <ingot/ast/FunctionType.hh>

#include <ingot/semantics/SemanticError.hh>
#include <sstream>

namespace ingot::semantics
{
    TypeResolver::TypeResolver() {}

    ast::Type
    TypeResolver::postop(const ast::Integer& i, std::monostate) const {
        return i.getType();
    }

    ast::Type
    TypeResolver::postop(const ast::String& str, std::monostate) const {
        return str.getType();
    }

    ast::Type
    TypeResolver::postop(const ast::Operator& op, const std::pair<ast::Type, ast::Type>& results, std::monostate) const {
        const ast::Type& lhsResult = results.first;
        const ast::Type& rhsResult = results.second;
        if (lhsResult != rhsResult) {
            std::stringstream ss;
            ss << "In expression '" << op << "': Type of lhs '" << lhsResult << "' does not match type of rhs '" << rhsResult << "'";
            throw SemanticError(ss.str(), op.getLocation());
        }
        return lhsResult;
    }

    ast::Type
    TypeResolver::postop(const ast::FunctionCall& func, const std::vector<ast::Type>& args, std::monostate) const {
        const ast::FunctionType& functionType = func.getFunctionType();
        const std::vector<ast::Type>& declTypes = functionType.getArgumentTypes();
        if (declTypes.size() != args.size()) {
            std::stringstream ss;
            ss << "In expression '" << func << "': Wrong number of arguments. "
               << "Expected " << declTypes.size() << ", "
               << "Got " << args.size() << ".";
            throw SemanticError(ss.str(), func.getLocation());
        }
        for (size_t index = 0; index < args.size(); ++index) {
            const ast::Type& declaredType = declTypes[index];
            const ast::Type& expressionType = args[index];
            if (declaredType != expressionType) {
                const ast::Expression& expr = func.getArguments()[index];
                std::stringstream ss;
                ss << "Expression '" << expr << "' "
                   << "of type '" << expressionType << "' "
                   << "does not match declared argument type '" << declaredType << "'.";
                throw SemanticError(ss.str(), expr.getLocation());
            }
        }
        return functionType.getReturnType();
    }

    ast::Type
    TypeResolver::postop(const ast::ArgumentReference& arg, std::monostate) const {
        return arg.getType();
    }

} // namespace ingot::semantics
