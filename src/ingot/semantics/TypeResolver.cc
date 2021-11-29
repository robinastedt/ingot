#include "TypeResolver.hh"

#include <ingot/ast/FunctionType.hh>

#include <ingot/semantics/SemanticError.hh>

#include <ingot/Error.hh>

#include <sstream>

namespace ingot::semantics
{
    TypeResolver::TypeResolver() {}

    ast::Type
    TypeResolver::preop(ast::List& list, ast::Type input, size_t index) {
        if (input.getVariant() != ast::Type::Variant::List) {
            std::stringstream ss;
            ss << "Expression '" << list << "' is a list and does not match expected type '" << input << "'.";
            throw SemanticError(ss.str(), list.getLocation());
        }
        return input.getSubtype();
    }

    ast::Type
    TypeResolver::preop(ast::Operator& op, ast::Type input, OperatorSide side) {
        switch (op.getVariant()) {
            case ast::Operator::Variant::Eq:
            case ast::Operator::Variant::Neq: {
                switch (side) {
                    case OperatorSide::LEFT: {
                        return ast::Type::reference(&op.getRhs());
                    } break;
                    case OperatorSide::RIGHT: {
                        return ast::Type::reference(&op.getLhs());
                    } break;
                }
            }
            default: return input;
        }
    }

    ast::Type
    TypeResolver::preop(ast::FunctionCall& func, ast::Type, size_t index) {
        return func.getFunctionType().getArgumentType(index);
    }

    ast::Type
    TypeResolver::preop(ast::Ternary&, ast::Type input, TernaryPosition position) {
        switch (position) {
            case TernaryPosition::CONDITION: return ast::Type::integer(1);
            case TernaryPosition::TRUE_BRANCH: return input;
            case TernaryPosition::FALSE_BRANCH: return input;
        }
        __builtin_unreachable();
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
    TypeResolver::postop(ast::Integer& i, ast::Type requiredType) {
        if (requiredType.getVariant() == ast::Type::Variant::Reference) {
            requiredType = requiredType.getReference().getType();
            if (requiredType == ast::Type::integer(0)) {
                // We reference an integer constant which has not yet been assigned a size
                requiredType = ast::Type::integer(64); // Default to 64 bit.TODO: maybe automatically scale this depending on values?
            }
        }
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
    TypeResolver::postop(ast::List& list, const std::vector<ast::Type>& elemResults, ast::Type requiredType) {
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
    TypeResolver::postop(ast::Operator& op, const std::pair<ast::Type, ast::Type>& results, ast::Type) {
        const ast::Type& lhsResult = results.first;
        const ast::Type& rhsResult = results.second;
        if (lhsResult != rhsResult) {
            std::stringstream ss;
            ss << "In expression '" << op << "': Type of lhs '" << lhsResult << "' does not match type of rhs '" << rhsResult << "'";
            throw SemanticError(ss.str(), op.getLocation());
        }
        switch (op.getVariant()) {
            case ast::Operator::Variant::Eq:
            case ast::Operator::Variant::Neq: return ast::Type::integer(1);
            default: return lhsResult;
        }
    }

    ast::Type
    TypeResolver::postop(ast::FunctionCall& func, const std::vector<ast::Type>& args, ast::Type) {
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
    TypeResolver::postop(ast::ArgumentReference& arg, ast::Type) {
        return arg.getType();
    }

    ast::Type
    TypeResolver::postop(ast::Ternary& ternary, const std::tuple<ast::Type, ast::Type, ast::Type>& results, ast::Type requiredType) {
        const ast::Type& condType = std::get<0>(results);
        const ast::Type& trueBranchType = std::get<1>(results);
        const ast::Type& falseBranchType = std::get<2>(results);
        if (condType != ast::Type::integer(1)) {
            std::stringstream ss;
            ss << "Expression '" << ternary.getCondition() << "' of type '" << condType << "' "
               << "does not match expected type '" << ast::Type::integer(1) << "' (bool).";
            throw SemanticError(ss.str(), ternary.getCondition().getLocation());
        }
        if (trueBranchType != requiredType) {
            std::stringstream ss;
            ss << "Expression '" << ternary.getTrueBranch() << "' of type '" << trueBranchType << "' "
               << "does not match expected type '" << requiredType << "'.";
            throw SemanticError(ss.str(), ternary.getTrueBranch().getLocation());
        }
        if (falseBranchType != requiredType) {
            std::stringstream ss;
            ss << "Expression '" << ternary.getFalseBranch() << "' of type '" << falseBranchType << "' "
               << "does not match expected type '" << requiredType << "'.";
            throw SemanticError(ss.str(), ternary.getFalseBranch().getLocation());
        }
        return requiredType;
    }

} // namespace ingot::semantics
