#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

#include "InfoMap.hpp"
#include <QWidget>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class Navigation;
}
QT_END_NAMESPACE

class Navigation : public QWidget {
    Q_OBJECT

public:
    Navigation(QWidget* parent = nullptr);
    ~Navigation();

signals:
    void enterSearchPage();
    void enterPurchasePage();
    void enterShipmentPage();

private slots:
    void handleEnterSearchPage();
    void handleEnterPurchasePage();
    void handleEnterShipmentPage();

    void handleSearchPageSearchRequest();
    void handleSearchPageSortRequest();

    void searchPageRefreshMenu(int option, int days);
    void searchPageDisplay(const std::vector<int>& idxs);

private:
    Ui::Navigation* ui;
    purchase::InfoMap m_map;
    std::vector<int> m_current_idxs;
};
#endif // NAVIGATION_HPP
