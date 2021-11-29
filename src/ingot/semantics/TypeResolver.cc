#include "TypeResolver.hh"

#include <ingot/ast/FunctionType.hh>

#include <ingot/semantics/SemanticError.hh>
#include <sstream>

namespace ingot::semantics
{
    TypeResolver::TypeResolver() {}

    size_t
    TypeResolver::preop(ast::Operator&, size_t size, OperatorSide) const {
        return size;
    }

    size_t
    TypeResolver::preop(ast::FunctionCall& func, size_t, size_t index) const {
        ast::Type argType = func.getFunctionType().getArgumentType(index);
        if (argType.getVariant() != ast::Type::Variant::Integer) {
            return 0;
        }
        return argType.getSize();
    }

    namespace
    {
        bool valueFits(int64_t value, size_t size) {
            if (size == 0) {
                return false;
            }
            if (size == 1) {
                return value <= 1 && value >= -1;
            }
            if (value < 0) {
                return valueFits(-(value + 1), size);
            }
            if (size == 2) {
                return value < 2;
            }
            return valueFits(value / 2, size - 1);
        }
    } // namespace

    ast::Type
    TypeResolver::postop(ast::Integer& i, size_t size) const {
        i.setSize(size);
        auto value = i.getValue();

        // Right now, unary negation does not count towards the integer token itself
        // meaning -128 does not "fit" inside of an i8
        // TODO: Update lexer to include unary negation of integer tokens
        if (!valueFits(value, size)) {
            std::stringstream ss;
            ss << "Integer literal '" << value << "' "
                << "can not fit in resolved type '" << i.getType() << "'.";
            throw SemanticError(ss.str(), i.getLocation());
        }
        return i.getType();
    }

    ast::Type
    TypeResolver::postop(ast::String& str, size_t size) const {
        return str.getType();
    }

    ast::Type
    TypeResolver::postop(ast::Operator& op, const std::pair<ast::Type, ast::Type>& results, size_t size) const {
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
    TypeResolver::postop(ast::FunctionCall& func, const std::vector<ast::Type>& args, size_t size) const {
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
    TypeResolver::postop(ast::ArgumentReference& arg, size_t size) const {
        return arg.getType();
    }

} // namespace ingot::semantics
