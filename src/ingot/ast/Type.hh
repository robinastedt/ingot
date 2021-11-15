#pragma once

#include <string>
#include <memory>

namespace ingot::ast
{
    class Type {
    public:
        enum class Variant {
            Unspecified,
            i8,
            i64,
            List
        };
    private:
        Variant m_variant;
        std::unique_ptr<Type> m_subtype;

        friend std::ostream& operator<<(std::ostream& str, const Type& type);

        Type(Variant variant, std::unique_ptr<Type> subtype = nullptr);

    public:
        Type();
        Type(const Type& other);
        Type& operator=(const Type& other);
        Type getSubtype() const;
        Variant getVariant() const;

        static Type int8();
        static Type int64();
        static Type list(const Type& subtype);
        
        
        bool operator==(const Type& rhs) const;
        bool operator!=(const Type& rhs) const;
        bool operator<(const Type& rhs) const;
        std::string getName() const;

        // Generates a name which is a valid c symbol
        std::string getNameEncoded() const;
    };

    std::ostream& operator<<(std::ostream& str, const Type& type);
} // namespace ingot::ast
