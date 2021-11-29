#include "TypeResolver.hh"

#include <ingot/ast/FunctionType.hh>

#include <ingot/semantics/SemanticError.hh>
#include <sstream>

namespace ingot::semantics
{
    TypeResolver::TypeResolver() {}

    ast::Type
    TypeResolver::preop(ast::List& list, ast::Type input, size_t index) const {
        if (input.getVariant() != ast::Type::Variant::List) {
            std::stringstream ss;
            ss << "Expression '" << list << "' is a list and does not match expected type '" << input << "'.";
            throw SemanticError(ss.str(), list.getLocation());
        }
        return input.getSubtype();
    }

    ast::Type
    TypeResolver::preop(ast::Operator&, ast::Type input, OperatorSide) const {
        return input;
    }

    ast::Type
    TypeResolver::preop(ast::FunctionCall& func, ast::Type, size_t index) const {
        return func.getFunctionType().getArgumentType(index);
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
    TypeResolver::postop(ast::Integer& i, ast::Type requiredType) const {
        if (requiredType.getVariant() != ast::Type::Variant::Integer) {
            std::stringstream ss;
            ss << "Expression '" << i << "' "
               << "does not match the expected type '" << requiredType << "'.";
            throw SemanticError(ss.str(), i.getLocation());
        }
        i.setSize(requiredType.getSize());
        auto value = i.getValue();

        // Right now, unary negation does not count towards the integer token itself
        // meaning -128 does not "fit" inside of an i8
        // TODO: Update lexer to include unary negation of integer tokens
        if (!valueFits(value, requiredType.getSize())) {
            std::stringstream ss;
            ss << "Integer literal '" << value << "' "
                << "can not fit in resolved type '" << i.getType() << "'.";
            throw SemanticError(ss.str(), i.getLocation());
        }
        return i.getType();
    }

    ast::Type
    TypeResolver::postop(ast::List& list, const std::vector<ast::Type>& elemResults, ast::Type requiredType) const {
        if (requiredType.getVariant() != ast::Type::Variant::List) {
            std::stringstream ss;
            ss << "Expression '" << list << "' is a list and does not match expected type '" << requiredType << "'.";
            throw SemanticError(ss.str(), list.getLocation());
        }
        ast::Type subtype = requiredType.getSubtype();
        size_t index = 0;
        for (const ast::Type& elemType : elemResults) {
            if (elemType != subtype) {
                std::stringstream ss;
                ss << "Expression '" << list.getElements()[index] << "' of type '" << elemType << "' does not match expected element type '" << subtype << "' of list '" << list << "'.";
                throw SemanticError(ss.str(), list.getLocation());
            }
            ++index;
        }

        list.setElementType(subtype);
        return list.getType();
    }

    ast::Type
    TypeResolver::postop(ast::Operator& op, const std::pair<ast::Type, ast::Type>& results, ast::Type) const {
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
    TypeResolver::postop(ast::FunctionCall& func, const std::vector<ast::Type>& args, ast::Type) const {
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
    TypeResolver::postop(ast::ArgumentReference& arg, ast::Type) const {
        return arg.getType();
    }

} // namespace ingot::semantics
