#include "List.hh"

#include <ingot/ast/Expression.hh>

namespace ingot::ast
{
    List::List()
    : m_elements() {}

    List::List(std::vector<Expression> elements)
    : m_elements(std::move(elements)) {
    }

    List::List(const std::string& string)
    : m_elements() {
        for (char c : string) {
            m_elements.emplace_back(Integer{c});
        }
    }

    std::vector<Expression>&
    List::getElements() {
        return m_elements;
    }
    const std::vector<Expression>&
    List::getElements() const {
        return m_elements;
    }

    void
    List::setElementType(const Type& type) {
        m_type = Type::list(type);
    }

    const Type&
    List::getType() const {
        return m_type;
    }

    bool
    List::operator==(const List& other) const {
        return m_type == other.m_type && m_elements == other.m_elements;
    }

    std::ostream& operator<<(std::ostream& str, const List& list) {
        str << "[";
        bool first = true;
        for (const Expression& expr : list.getElements()) {
            if (!first) {
                str << ",";
            }
            str << expr;
            first = false;
        }
        str << "]";
        return str;
    }
} // namespace ingot::ast
