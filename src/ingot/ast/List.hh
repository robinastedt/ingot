#pragma once


#include <ingot/ast/Node.hh>
#include <ingot/ast/Type.hh>

#include <vector>

namespace ingot::ast
{
    class Expression;

    class List : public Node {
    private:
        std::vector<Expression> m_elements;
        Type m_type;

    public:
        List();
        List(std::vector<Expression> elements);
        // Specialization for strings
        List(const std::string& string);
        
        void setElementType(const Type& type);
        const Type& getType() const;

        std::vector<Expression>& getElements();
        const std::vector<Expression>& getElements() const;
    };

    std::ostream& operator<<(std::ostream& str, const List& list);
} // namespace ingot::ast
