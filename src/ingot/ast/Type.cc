#include "Type.hh"

#include <ingot/Error.hh>

#include <cassert>

namespace ingot::ast
{
    Type::Type(Variant variant, std::unique_ptr<Type> subtype, size_t size)
    : Node()
    , m_variant(variant)
    , m_subtype(std::move(subtype))
    , m_size(size) {}

    Type::Type()
    : Node()
    , m_variant(Variant::Unspecified)
    , m_subtype() {}

    Type::Type(const Type& other)
    : Node(other)
    , m_variant(other.m_variant)
    , m_subtype(other.m_subtype ? std::make_unique<Type>(*other.m_subtype) : nullptr)
    , m_size(other.m_size) {}

    Type&
    Type::operator=(const Type& other) {
        Node::operator=(other);
        m_variant = other.m_variant;
        m_subtype = other.m_subtype ? std::make_unique<Type>(*other.m_subtype) : nullptr;
        m_size = other.m_size;
        return *this;
    }

    Type
    Type::getSubtype() const {
        if (!m_subtype) {
            throw internal_error("Type '" + getName() + "' does not have subtype");
        }
        return *m_subtype;
    }

    Type::Variant
    Type::getVariant() const {
        return m_variant;
    }

    size_t
    Type::getSize() const {
        if (!m_size) {
            throw internal_error("Type '" + getName() + "' does not have a size");
        }
        return m_size;
    }

    void
    Type::setSize(size_t size) {
        if (m_variant != Variant::Integer) {
            throw internal_error("Attempted to set size on non-integer type: " + getName());
        }
        m_size = size;
    }

    Type
    Type::integer(size_t size) {
        return Type{Variant::Integer, nullptr, size};;
    }

    Type
    Type::list(const Type& subtype) {
        return Type{Variant::List, std::make_unique<Type>(subtype), 0};
    }


    bool
    Type::operator==(const Type& rhs) const {
        if (m_variant != rhs.m_variant)  {
            return false;
        }
        switch (m_variant) {
            case Variant::Integer: {
                return m_size == rhs.m_size;
            }
            case Variant::List: {
                return *m_subtype == *rhs.m_subtype;
            }
            default: throw internal_error("Unsupported type variant: " + (int)m_variant);
        }
    }

    bool
    Type::operator!=(const Type& rhs) const {
        return !operator==(rhs);
    }

    bool
    Type::operator<(const Type& rhs) const {
        if (m_variant != rhs.m_variant) {
            return m_variant < rhs.m_variant;
        }
        switch (m_variant) {
            case Variant::Integer: {
                return getSize() < rhs.getSize();
            }
            case Variant::List: {
                return getSubtype() < rhs.getSubtype();
            }
            default: throw internal_error("Unsupported type variant: " + (int)m_variant);
        }
    }
    std::string
    Type::getName() const {
        switch (m_variant) {
            case Variant::Integer: {
                return "i" + std::to_string(m_size);
            }
            case Variant::List: {
                assert(m_subtype);
                return "[" + m_subtype->getName() + "]";
            }
            default: throw internal_error("Unsupported type variant: " + (int)m_variant);
        }
    }

    std::string
    Type::getNameEncoded() const {
        switch (m_variant) {
            case Variant::Integer: {
                // L = Integer, E = end
                return "I" + std::to_string(m_size) + "E";
            }
            case Variant::List: {
                assert(m_subtype);
                // L = List, E = end
                return "L" + m_subtype->getNameEncoded() + "E";
            }
            default: throw internal_error("Unsupported type variant: " + (int)m_variant);
        }
    }

    std::ostream& operator<<(std::ostream& str, const Type& type) {
        return str << type.getName();
    }
} // namespace ingot::ast
