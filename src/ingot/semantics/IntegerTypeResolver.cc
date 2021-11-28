#include "IntegerTypeResolver.hh"

#include <ingot/ast/FunctionCall.hh>
#include <ingot/ast/FunctionType.hh>

#include <ingot/semantics/SemanticError.hh>

#include <sstream>

namespace ingot::semantics
{
    size_t
    IntegerTypeResolver::preop(ast::Operator&, const size_t& input, OperatorSide) {
        return input;
    }

    size_t
    IntegerTypeResolver::preop(ast::FunctionCall& func, const size_t&, size_t index) {
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
    

    void
    IntegerTypeResolver::operator()(ast::Integer& i, const size_t& input) {
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
    }

    void
    IntegerTypeResolver::operator()(ast::String& str, const size_t& input) {
    }

    void
    IntegerTypeResolver::operator()(ast::Operator& op, const size_t& input) {
    }

    void
    IntegerTypeResolver::operator()(ast::FunctionCall& func, const size_t& input) {
    }

    void
    IntegerTypeResolver::operator()(ast::ArgumentReference& arg, const size_t& input) {
    }

} // namespace ingot::semantics
