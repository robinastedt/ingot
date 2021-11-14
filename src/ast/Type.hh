#pragma once

#include <string>
#include <memory>

namespace ingot::ast
{
    class Type {
        
        enum class Variant {
            Unspecified,
            i8,
            i64,
            List
        };

        Variant m_variant;
        std::unique_ptr<Type> m_subtype;

        friend std::ostream& operator<<(std::ostream& str, const Type& type);

        Type(Variant variant, std::unique_ptr<Type> subtype = nullptr);

    public:
        Type();
        Type(const Type& other);
        Type& operator=(const Type& other);
        Type getSubtype() const;

        static Type int8();
        static Type int64();
        static Type list(const Type& subtype);
        
        
        bool operator==(const Type& rhs) const;
        bool operator!=(const Type& rhs) const;
        std::string getName() const;

        // TODO: Use instead of hardcoded "prefix" in codegen
        std::string getSymbolIdentifier() const;
    };

    std::ostream& operator<<(std::ostream& str, const Type& type);
} // namespace ingot::ast
