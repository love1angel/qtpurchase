#include "ProductItem.hpp"

#include <iomanip>
#include <sstream>

namespace purchase {

std::istream& operator>>(std::istream& in, ProductItem& p)
{
    return in;
}

std::ostream& operator<<(std::ostream& o, const ProductItem& p)
{
    return o << "ProductItem: { id: " << p.m_pid
             << ", price: " << p.m_price
             << ", amount: " << p.m_amount
             << ", purchase time: " << p.m_purchase_time.toString("yyyy-MM-dd").data()
             << ", expire time: " << p.m_expire_time.toString("yyyy-MM-dd").data()
             << " }." << std::endl;
}

} // namespace purchase
