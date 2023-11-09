#pragma once

#include "./common.hpp"

#include <QFile>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "./ProductInfo.hpp"
#include "./ProductItem.hpp"

namespace purchase {

class InfoMap {
public:
    InfoMap();

    ~InfoMap() noexcept;

    auto& getTotal() { return m_total; }
    auto& getInfoMap() { return m_product_map; }

private:
    std::vector<ProductItem> m_total {};
    std::unordered_map<pid, ProductInfo> m_product_map {};
    QFile file, file1;
};

} // namespace purchase
