#include "Type.hh"

#include <ingot/Error.hh>

#include <cassert>

namespace ingot::ast
{
    Type::Type(Variant variant, std::unique_ptr<Type> subtype, size_t size, Expression* reference)
    : Node()
    , m_variant(variant)
    , m_subtype(std::move(subtype))
    , m_size(size)
    , m_reference(reference) {}

    Type::Type()
    : Node()
    , m_variant(Variant::Unspecified)
    , m_subtype()
    , m_size(0)
    , m_reference(nullptr) {}

    Type::Type(const Type& other)
    : Node(other)
    , m_variant(other.m_variant)
    , m_subtype(other.m_subtype ? std::make_unique<Type>(*other.m_subtype) : nullptr)
    , m_size(other.m_size)
    , m_reference(other.m_reference) {}

    Type&
    Type::operator=(const Type& other) {
        Node::operator=(other);
        m_variant = other.m_variant;
        m_subtype = other.m_subtype ? std::make_unique<Type>(*other.m_subtype) : nullptr;
        m_size = other.m_size;
        m_reference = other.m_reference;
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

    const Expression&
    Type::getReference() const {
        if (!m_reference) {
            throw internal_error("Type '" + getName() + "' does not have a reference");
        }
        return *m_reference;
    }

    Type
    Type::integer(size_t size) {
        return Type{Variant::Integer, nullptr, size, nullptr};
    }

    Type
    Type::list(const Type& subtype) {
        return Type{Variant::List, std::make_unique<Type>(subtype), 0, nullptr};
    }

    Type
    Type::reference(Expression* reference) {
        return Type{Variant::Reference, nullptr, 0, reference};
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
            case Variant::Reference: {
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
            case Variant::Reference: {
                return m_reference < rhs.m_reference;
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
            case Variant::Reference: {
                assert(m_reference);
                return "reftype(" + std::to_string((ptrdiff_t)m_reference) + ")";
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
