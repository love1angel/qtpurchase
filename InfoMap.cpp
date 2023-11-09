#include "./InfoMap.hpp"

#include <iostream>

#include <QFile>
#include <QTextStream>
#include <sstream>

// constexpr const char* INFO_PATH = ":/data/product_info.txt";
// constexpr const char* ITEM_PATH = ":/data/product_item.txt";

// qrc 析构函数保存会出现问题
#define INFO_PATH "/Users/helianthusxie/opensource/qtpro/purchase/product_info.txt"
#define ITEM_PATH "/Users/helianthusxie/opensource/qtpro/purchase/product_item.txt"

// #define INFO_PATH ":/data/product_info.txt"
// #define ITEM_PATH ":/data/product_item.txt"

namespace purchase {

InfoMap::InfoMap()
{
    qDebug() << INFO_PATH;
    qDebug() << ITEM_PATH;
    {
        QFile file(INFO_PATH);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);

            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList parts = line.split(' ');
                if (3 == parts.size()) {
                    bool ok { false };
                    int pid = parts[0].toInt(&ok);
                    if (ok) {
                        purchase::ProductInfo info { parts[1].toStdString(), parts[2].toStdString() };
                        if (not m_product_map.contains(pid)) {
                            m_product_map[pid] = std::move(info);
                        }
                    }
                }
            }

            for (const auto& pair : m_product_map) {
                qDebug() << "pid: " << std::get<0>(pair) << ", name: " << std::get<1>(pair).getName().c_str() << ", producer: " << std::get<1>(pair).getProducer().c_str();
            }

            file.close();
        } else {
            qFatal("Unable to open the file.");
        }
    }

    {

        QFile file(ITEM_PATH);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);

            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList parts = line.split(' ');
                if (5 == parts.size()) {
                    bool ok { false };
                    int pid = parts[0].toInt(&ok);
                    if (ok) {
                        bool ok { false };
                        double price = parts[1].toDouble(&ok);
                        if (ok) {
                            bool ok { false };
                            int amount = parts[2].toInt(&ok);
                            if (ok) {
                                purchase::ProductItem item { pid, price, amount, parts[3], parts[4] };
                                m_total.emplace_back(std::move(item));
                            }
                        }
                    }
                }
            }

            for (const auto& p : m_total) {
                qDebug() << "ProductItem: { id: " << p.getPid()
                         << ", price: " << p.getPrice()
                         << ", amount: " << p.getAmount()
                         << ", purchase time: " << p.getPurchaseTime().toString("yyyy-MM-dd")
                         << ", expire time: " << p.getExpireTime().toString("yyyy-MM-dd")
                         << " }.";
            }

            file.close();
        } else {
            qFatal("Unable to open the file.");
        }
    }
}

InfoMap::~InfoMap() noexcept
{
    QFile file2(INFO_PATH);
    if (file2.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file2);

        for (const auto& item : m_product_map) {
            out << std::get<0>(item) << " "
                << std::get<1>(item).getName().data() << " "
                << std::get<1>(item).getProducer().data() << "\n";
        }

        file2.close();
    } else {
        qFatal("Unable to open the INFO_PATH file.");
    }

    QFile file(ITEM_PATH);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        for (const auto& item : m_total) {
            out << item.getPid() << " "
                << item.getPrice() << " "
                << item.getAmount() << " "
                << item.getPurchaseTime().toString("yyyy-MM-dd") << " "
                << item.getExpireTime().toString("yyyy-MM-dd") << "\n";
        }
        file.close();
    } else {
        qDebug() << file.errorString();
        qFatal("Unable to open the ITEM_PATH file.");
    }
}

} // namespace purchase
