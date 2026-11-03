#pragma once

#include "SettingsPage.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QTimer;

namespace colibry {

class DateTimePage : public SettingsPage
{
    Q_OBJECT

public:
    explicit DateTimePage(SettingsStore &store, QWidget *parent = nullptr);

    QString title() const override;
    QIcon icon() const override;
    QString keywords() const override;

protected:
    void refresh() override;

private:
    void tick();

    QLabel *m_time = nullptr;
    QLabel *m_date = nullptr;
    QCheckBox *m_auto = nullptr;
    QComboBox *m_zone = nullptr;
    QLabel *m_offset = nullptr;
    QCheckBox *m_clock24 = nullptr;
    QTimer *m_timer = nullptr;
};

} // namespace colibry
