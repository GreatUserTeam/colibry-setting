#pragma once

#include "SettingsPage.h"

class QLineEdit;
class QTableWidget;
class QTimer;

namespace colibry {

class NetworkPage : public SettingsPage
{
    Q_OBJECT

public:
    explicit NetworkPage(SettingsStore &store, QWidget *parent = nullptr);

    QString title() const override;
    QIcon icon() const override;
    QString keywords() const override;

protected:
    void refresh() override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    void refreshInterfaces();

    QLineEdit *m_hostname = nullptr;
    QTableWidget *m_table = nullptr;
    QTimer *m_timer = nullptr;
};

} // namespace colibry
