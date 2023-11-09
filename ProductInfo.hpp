#pragma once

#include <iostream>
#include <string>

namespace purchase {

class ProductInfo;

std::istream& operator>>(std::istream&, ProductInfo&);
std::ostream& operator<<(std::ostream&, const ProductInfo&);

class ProductInfo {
    friend std::istream& operator>>(std::istream&, ProductInfo&);
    friend std::ostream& operator<<(std::ostream&, const ProductInfo&);

public:
    ProductInfo() = default;

    ProductInfo(const std::string& name, const std::string& producer)
        : m_name(name)
        , m_producer(producer)
    {
    }

    [[nodiscard]] const std::string& getName() const { return m_name; }
    [[nodiscard]] const std::string& getProducer() const { return m_producer; }

private:
    std::string m_name;
    std::string m_producer;
};

inline std::istream& operator>>(std::istream& in, ProductInfo& p)
{
    return in >> p.m_name >> p.m_producer;
}

inline std::ostream&
operator<<(std::ostream& o, const ProductInfo& p)
{
    return o << "ProductInfo: { name: " << p.m_name
             << ", producer: " << p.m_producer
             << " }." << std::endl;
}

} // namespace purchase
