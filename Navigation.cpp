#include "Navigation.hpp"
#include "./ui_navigation.h"

#include <QMessageBox>
#include <QTableWidgetItem>

Navigation::Navigation(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Navigation)
{
    ui->setupUi(this);

    // navigation page jump
    QObject::connect(ui->m_welcome_search_btn, &QPushButton::clicked, [&]() {
        ui->stackedWidget->setCurrentIndex(1);
        emit enterSearchPage();
    });
    QObject::connect(ui->m_welcome_purchase_btn, &QPushButton::clicked, [&]() {
        ui->stackedWidget->setCurrentIndex(2);
        emit enterPurchasePage();
    });
    QObject::connect(ui->m_welcome_shipment_btn, &QPushButton::clicked, [&]() {
        ui->stackedWidget->setCurrentIndex(3);
        emit enterShipmentPage();
    });
    QObject::connect(ui->m_welcome_exit_btn, &QPushButton::clicked, [&]() {
        QApplication::quit();
    });

    // search page configuration
    QObject::connect(this, &Navigation::enterSearchPage, this, &Navigation::handleEnterSearchPage);
    QObject::connect(ui->m_search_return_btn, &QPushButton::clicked, [&]() {
        ui->stackedWidget->setCurrentIndex(0);
    });
    QObject::connect(ui->m_search_search_btn, &QPushButton::clicked, this, &Navigation::handleSearchPageSearchRequest);
    QObject::connect(ui->m_search_state_box, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [&](int index) {
            if (index == 1) {
                this->ui->m_search_warn_cnt->setEnabled(true);
            } else {
                this->ui->m_search_warn_cnt->setEnabled(false);
            }
        });
    QObject::connect(ui->m_search_sort_btn, &QPushButton::clicked, this, &Navigation::handleSearchPageSortRequest);

    // purchase page
    QObject::connect(this, &Navigation::enterPurchasePage, this, &Navigation::handleEnterPurchasePage);
    QObject::connect(this->ui->m_purchase_pid_input, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        qDebug() << "SpinBox value changed to: " << value;
        QString tabel = QString(("货物号：{" + m_map.getInfoMap()[value].getName() + ":" + m_map.getInfoMap()[value].getProducer() + "}").data());
        this->ui->m_purchase_pid_lbl->setText(tabel);
    });
    QObject::connect(ui->m_purchase_order_confirm_btn, &QPushButton::clicked, [&]() {
        int pid = this->ui->m_purchase_pid_input->value();
        double price = this->ui->m_purchase_price_input->value();
        int amount = this->ui->m_purchase_amount_input->value();
        QDate purchase_time = this->ui->m_purchase_purchase_time_input->date();
        QDate expire_time = this->ui->m_purchase_expire_time_input->date();

        qDebug() << "new order"
                 << pid
                 << price
                 << amount
                 << purchase_time
                 << expire_time;

        if (not m_map.getInfoMap().contains(pid)) {
            QMessageBox::critical(nullptr, "错误", "请输入合理的商品号");
            return;
        }
        if (price == 0.0 || amount == 0) {
            QMessageBox::critical(nullptr, "错误", "价格或数量不能为0");
            return;
        }
        if (expire_time < QDate::currentDate()) {
            QMessageBox::critical(nullptr, "错误", "商品已过期");
            return;
        }

        m_map.getTotal().push_back({ pid, price, amount, purchase_time, expire_time });
        QMessageBox::critical(nullptr, "消息", "添加成功");
    });
    QObject::connect(ui->m_purchase_return_btn, &QPushButton::clicked, [&]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    // shipment page
    QObject::connect(this, &Navigation::enterShipmentPage, this, &Navigation::handleEnterShipmentPage);
    QObject::connect(ui->m_shipment_return_btn, &QPushButton::clicked, [&]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    QObject::connect(this->ui->m_shipment_pid_input, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        qDebug() << "SpinBox value changed to: " << value;
        QString tabel = QString(("货物号：{" + m_map.getInfoMap()[value].getName() + ":" + m_map.getInfoMap()[value].getProducer() + "}").data());
        this->ui->m_shipment_pid_lbl->setText(tabel);
    });

    QObject::connect(ui->m_shipment_confirm_btn, &QPushButton::clicked, [&]() {
        int pid = this->ui->m_shipment_pid_input->value();
        int amount = this->ui->m_shipment_amount_input->value();

        qDebug() << "delete order"
                 << pid
                 << amount;

        std::vector<int> ret {};

        for (int i = 0; i < m_map.getTotal().size(); ++i) {
            if (pid == m_map.getTotal()[i].getPid() && not m_map.getTotal()[i].isOutdated()) {
                ret.push_back(i);
            }
        }

        if (0 == ret.size()) {
            QMessageBox::critical(nullptr, "错误", "仓库内没有该类型的商品");
            return;
        }
        int current_amount = 0;
        for (int i = 0; i < ret.size(); ++i) {
            current_amount += m_map.getTotal()[ret[i]].getAmount();
        }
        if (current_amount < amount) {
            QMessageBox::critical(nullptr, "错误", QString("错误, 仓库内没有这么多，目前有：" + QString::number(current_amount)));
            return;
        }

        std::sort(ret.begin(), ret.end(), [this](const int& lhs, const int& rhs) {
            return m_map.getTotal()[lhs].getPurchaseTime() < m_map.getTotal()[rhs].getPurchaseTime()
                || (m_map.getTotal()[lhs].getPurchaseTime() == m_map.getTotal()[rhs].getPurchaseTime() && m_map.getTotal()[lhs].getExpireTime() > m_map.getTotal()[rhs].getExpireTime());
        });

        for (const auto& idx : ret) {
            if (amount <= m_map.getTotal()[idx].getAmount()) {
                m_map.getTotal()[idx].setAmount(m_map.getTotal()[idx].getAmount() - amount);
                break;
            } else {
                amount -= amount - m_map.getTotal()[idx].getAmount();
                m_map.getTotal()[idx].setAmount(0);
            }
        }

        std::remove_if(m_map.getTotal().begin(), m_map.getTotal().end(), [](const purchase::ProductItem& item) {
            return 0 == item.getAmount();
        });

        QMessageBox::critical(nullptr, "消息", "删除成功");
    });
}

Navigation::~Navigation()
{
    delete ui;
}

void Navigation::handleEnterSearchPage()
{
    qDebug() << "enter into search page";
}

void Navigation::handleEnterPurchasePage()
{
    qDebug() << "enter into purchase page";
    QString tabel = QString(("货物号：{" + m_map.getInfoMap()[this->ui->m_purchase_pid_input->value()].getName() + ":" + m_map.getInfoMap()[this->ui->m_purchase_pid_input->value()].getProducer() + "}").data());
    this->ui->m_purchase_pid_lbl->setText(tabel);
    this->ui->m_purchase_pid_input->setMinimum(1);
    this->ui->m_purchase_pid_input->setMaximum(m_map.getInfoMap().size());
    this->ui->m_purchase_purchase_time_input->setDate(QDate::currentDate());
    this->ui->m_purchase_expire_time_input->setDate(QDate::currentDate());
}

void Navigation::handleEnterShipmentPage()
{
    qDebug() << "enter into shipment page";
    QString tabel = QString(("货物号：{" + m_map.getInfoMap()[this->ui->m_shipment_pid_input->value()].getName() + ":" + m_map.getInfoMap()[this->ui->m_shipment_pid_input->value()].getProducer() + "}").data());
    this->ui->m_shipment_pid_lbl->setText(tabel);
    this->ui->m_shipment_pid_input->setMinimum(1);
    this->ui->m_shipment_pid_input->setMaximum(m_map.getInfoMap().size());
}

void Navigation::handleSearchPageSearchRequest()
{
    this->m_current_idxs.clear();

    QString text = ui->m_search_state_box->currentText();

    qDebug() << text;
    if ("全部查询" == text) {
        this->searchPageRefreshMenu(0, 0);
    } else if ("警告查询" == text) {
        this->searchPageRefreshMenu(1, this->ui->m_search_warn_cnt->value());
    } else {
        this->searchPageRefreshMenu(2, 0);
    }
}

void Navigation::handleSearchPageSortRequest()
{
    std::sort(m_current_idxs.begin(), m_current_idxs.end(), [this](const int& lhs, const int& rhs) {
        const auto& total = m_map.getTotal();
        return total[lhs].getPid() < total[rhs].getPid()
            || (total[lhs].getPid() == total[rhs].getPid() && total[lhs].getExpireTime() < total[rhs].getExpireTime());
    });
    searchPageDisplay(this->m_current_idxs);
}

void Navigation::searchPageRefreshMenu(int option, int days)
{
    const auto& total = m_map.getTotal();
    const auto& map = m_map.getInfoMap();

    for (int i = 0; i < total.size(); ++i) {
        if (0 == option) {
            m_current_idxs.push_back(i);
        } else if (1 == option) {
            if (not total[i].isOutdated() && total[i].isWarn(days)) {
                m_current_idxs.push_back(i);
            }
        } else {
            if (total[i].isOutdated()) {
                m_current_idxs.push_back(i);
            }
        }
    }

    searchPageDisplay(this->m_current_idxs);
}

void Navigation::searchPageDisplay(const std::vector<int>& idxs)
{
    ui->m_search_table_wgt->setColumnCount(7);

    const auto& total = m_map.getTotal();
    const auto& map = m_map.getInfoMap();

    ui->m_search_table_wgt->setRowCount(m_current_idxs.size());

    for (int i = 0; i < m_current_idxs.size(); ++i) {
        QTableWidgetItem* pid = new QTableWidgetItem(QString("%0").arg(total[m_current_idxs[i]].getPid()));
        QTableWidgetItem* name = new QTableWidgetItem(map.at(total[m_current_idxs[i]].getPid()).getName().data());
        QTableWidgetItem* producer = new QTableWidgetItem(map.at(total[m_current_idxs[i]].getPid()).getProducer().data());
        QTableWidgetItem* price = new QTableWidgetItem(QString("%0").arg(total[m_current_idxs[i]].getPrice()));
        QTableWidgetItem* amount = new QTableWidgetItem(QString("%0").arg(total[m_current_idxs[i]].getAmount()));
        QTableWidgetItem* purchase_time = new QTableWidgetItem(total[m_current_idxs[i]].getPurchaseTime().toString("yyyy-MM-dd"));
        QTableWidgetItem* expire_time = new QTableWidgetItem(total[m_current_idxs[i]].getExpireTime().toString("yyyy-MM-dd"));
        if (total[m_current_idxs[i]].isOutdated()) {
            expire_time->setData(Qt::BackgroundRole, QColor(255, 0, 0));
        }
        ui->m_search_table_wgt->setItem(i, 0, pid);
        ui->m_search_table_wgt->setItem(i, 1, name);
        ui->m_search_table_wgt->setItem(i, 2, producer);
        ui->m_search_table_wgt->setItem(i, 3, price);
        ui->m_search_table_wgt->setItem(i, 4, amount);
        ui->m_search_table_wgt->setItem(i, 5, purchase_time);
        ui->m_search_table_wgt->setItem(i, 6, expire_time);
    }

    QStringList headerLabels;
    headerLabels << "货物号"
                 << "名称"
                 << "制造商"
                 << "价格"
                 << "数量"
                 << "进货时间"
                 << "到期时间";
    ui->m_search_table_wgt->setHorizontalHeaderLabels(headerLabels);
    ui->m_search_table_wgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
