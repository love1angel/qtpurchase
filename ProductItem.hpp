#pragma once

#include "common.hpp"

#include <chrono>
#include <iostream>

#include <QDate>
#include <QString>

namespace purchase {

class ProductItem;

std::istream& operator>>(std::istream&, ProductItem&);
std::ostream& operator<<(std::ostream&, const ProductItem&);

class ProductItem {
    friend std::istream& operator>>(std::istream&, ProductItem&);
    friend std::ostream& operator<<(std::ostream&, const ProductItem&);

public:
    ProductItem() = default;

    ProductItem(pid pid, double price, int amount, const QString& purchase_time, const QString& expire_time)
        : m_pid(pid)
        , m_price(price)
        , m_amount(amount)
        , m_purchase_time(QDate::fromString(purchase_time, "yyyy-MM-dd"))
        , m_expire_time(QDate::fromString(expire_time, "yyyy-MM-dd"))
    {
    }

    ProductItem(pid pid, double price, int amount, const QDate& purchase_time, const QDate& expire_time)
        : m_pid(pid)
        , m_price(price)
        , m_amount(amount)
        , m_purchase_time(purchase_time)
        , m_expire_time(expire_time)
    {
    }

    [[nodiscard]] pid getPid() const { return m_pid; }
    [[nodiscard]] double getPrice() const { return m_price; }
    [[nodiscard]] int getAmount() const { return m_amount; }
    [[nodiscard]] const auto& getPurchaseTime() const { return m_purchase_time; }
    [[nodiscard]] const auto& getExpireTime() const { return m_expire_time; }

    void setAmount(int amount) { this->m_amount = amount; }

    bool isOutdated() const
    {
        auto today = QDate::currentDate();
        return m_expire_time < today;
    }

    bool isWarn(int amount) const
    {
        auto today = QDate::currentDate();

        auto duration = today.daysTo(m_expire_time);
        return duration >= 0 && duration <= amount;
    }

private:
    pid m_pid {};

    double m_price {};
    int m_amount {};

    QDate m_purchase_time;
    QDate m_expire_time;
};

} // namespace purchase
