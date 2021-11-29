#include "IntegerTypeResolver.hh"

#include <ingot/ast/FunctionCall.hh>
#include <ingot/ast/FunctionType.hh>

#include <ingot/semantics/SemanticError.hh>

#include <sstream>

namespace ingot::semantics
{
    size_t
    IntegerTypeResolver::preop(ast::Operator&, size_t input, OperatorSide) const {
        return input;
    }

    size_t
    IntegerTypeResolver::preop(ast::FunctionCall& func, size_t, size_t index) const {
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
    

    std::monostate
    IntegerTypeResolver::postop(ast::Integer& i, size_t input) const {
        i.setSize(input);
        auto value = i.getValue();

        // Right now, unary negation does not count towards the integer literal itself
        // meaning -128 does not "fit" inside of an i8
        // TODO: Update lexer to include unary negation of integer literals
        if (!valueFits(value, input)) {
            std::stringstream ss;
            ss << "Integer literal '" << value << "' "
                << "can not fit in resolved type '" << i.getType() << "'.";
            throw SemanticError(ss.str(), i.getLocation());
        }
        return {};
    }

    std::monostate
    IntegerTypeResolver::postop(ast::String& str, size_t input) const {
        return {};
    }

    std::monostate
    IntegerTypeResolver::postop(ast::Operator& op, const std::pair<std::monostate, std::monostate>& argResults, size_t input) const {
        return {};
    }

    std::monostate
    IntegerTypeResolver::postop(ast::FunctionCall& func, const std::vector<std::monostate>& argResults, size_t input) const {
        return {};
    }

    std::monostate
    IntegerTypeResolver::postop(ast::ArgumentReference& arg, size_t input) const {
        return {};
    }

} // namespace ingot::semantics
