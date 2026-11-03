#pragma once

#include "SettingsPage.h"

class QComboBox;
class QLabel;
class QTimer;

namespace colibry {

class LanguageRegionPage : public SettingsPage
{
    Q_OBJECT

public:
    explicit LanguageRegionPage(SettingsStore &store, QWidget *parent = nullptr);

    QString title() const override;
    QIcon icon() const override;
    QString keywords() const override;

protected:
    void refresh() override;

private:
    void updatePreview();

    QComboBox *m_language = nullptr;
    QComboBox *m_region = nullptr;
    QComboBox *m_firstDay = nullptr;
    QLabel *m_preview = nullptr;
    QTimer *m_timer = nullptr;
};

} // namespace colibry
