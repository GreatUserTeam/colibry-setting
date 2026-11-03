#pragma once

#include "SettingsPage.h"

class QLabel;

namespace colibry {

class AboutPage : public SettingsPage
{
    Q_OBJECT

public:
    explicit AboutPage(SettingsStore &store, QWidget *parent = nullptr);

    QString title() const override;
    QIcon icon() const override;
    QString keywords() const override;

protected:
    void refresh() override;

private:
    QLabel *m_hostname = nullptr;
};

} // namespace colibry
