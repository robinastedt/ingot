#pragma once

#include <ingot/ast/Node.hh>

#include <string>
#include <memory>

namespace ingot::ast
{
    class Expression;

    class Type : public Node {
    public:
        enum class Variant {
            Unspecified,
            Integer,
            List,
            Reference
        };
    private:
        Variant m_variant;
        std::unique_ptr<Type> m_subtype;  // Used by List
        size_t m_size;  // Used by Integer
        const Expression* m_reference;  // Used by Reference

        friend std::ostream& operator<<(std::ostream& str, const Type& type);

        Type(Variant variant, std::unique_ptr<Type> subtype, size_t size, Expression* reference);

    public:
        Type();
        Type(const Type& other);
        Type& operator=(const Type& other);
        Type getSubtype() const;
        Variant getVariant() const;
        size_t getSize() const;
        void setSize(size_t size);
        const Expression& getReference() const;

        static Type integer(size_t size);
        static Type list(const Type& subtype);
        static Type reference(Expression* reference);
        
        
        bool operator==(const Type& rhs) const;
        bool operator!=(const Type& rhs) const;
        bool operator<(const Type& rhs) const;
        std::string getName() const;

        // Generates a name which is a valid c symbol
        std::string getNameEncoded() const;
    };

    std::ostream& operator<<(std::ostream& str, const Type& type);
} // namespace ingot::ast
