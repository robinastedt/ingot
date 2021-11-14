#include "Type.hh"

#include <Error.hh>

#include <cassert>

namespace ingot::ast
{
    Type::Type(Variant variant, std::unique_ptr<Type> subtype)
    : m_variant(variant)
    , m_subtype(std::move(subtype)) {}

    Type::Type()
    : m_variant(Variant::Unspecified)
    , m_subtype() {}

    Type::Type(const Type& other)
    : m_variant(other.m_variant)
    , m_subtype(other.m_subtype ? std::make_unique<Type>(*other.m_subtype) : nullptr) {}

    Type&
    Type::operator=(const Type& other) {
        m_variant = other.m_variant;
        m_subtype = other.m_subtype ? std::make_unique<Type>(*other.m_subtype) : nullptr;
        return *this;
    }

    Type
    Type::getSubtype() const {
        if (!m_subtype) {
            throw internal_error("Type '" + getName() + "' does not have subtype");
        }
        return *m_subtype;
    }

    Type
    Type::int8() {
        return Type{Variant::i8};;
    }

    Type
    Type::int64() {
        return Type{Variant::i64};
    }

    Type
    Type::list(const Type& subtype) {
        return Type{Variant::List, std::make_unique<Type>(subtype)};
    }


    bool
    Type::operator==(const Type& rhs) const {
        // if variants are same, then both or none have subtypes
        assert(m_variant != rhs.m_variant || (!m_subtype) == (!rhs.m_subtype)); 
        return m_variant == rhs.m_variant && (!m_subtype || *m_subtype == *rhs.m_subtype); // lhs has subtype -> subtypes are equal
    }

    bool
    Type::operator!=(const Type& rhs) const {
        return !operator==(rhs);
    }

    std::string
    Type::getName() const {
        switch (m_variant) {
            case Variant::i8: return "i8";
            case Variant::i64: return "i64";
            case Variant::List: {
                assert(m_subtype);
                return "[" + m_subtype->getName() + "]";
            }
            default: throw internal_error("Unsupported type variant: " + (int)m_variant);
        }
    }

    std::string
    Type::getSymbolIdentifier() const {
        switch (m_variant) {
            case Variant::i8: return "i8";
            case Variant::i64: return "i64";
            case Variant::List: {
                assert(m_subtype);
                return "LB" + m_subtype->getSymbolIdentifier() + "LE";
            }
            default: throw internal_error("Unsupported type variant: " + (int)m_variant);
        }
    }

    std::ostream& operator<<(std::ostream& str, const Type& type) {
        return str << type.getName();
    }
} // namespace ingot::ast
