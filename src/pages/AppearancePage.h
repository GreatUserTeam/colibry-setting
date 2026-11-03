#pragma once

#include "SettingsPage.h"

#include <QColor>
#include <QVector>

class QComboBox;
class QSpinBox;
class QToolButton;

namespace colibry {

class AppearancePage : public SettingsPage
{
    Q_OBJECT

public:
    explicit AppearancePage(SettingsStore &store, QWidget *parent = nullptr);

    QString title() const override;
    QIcon icon() const override;
    QString keywords() const override;

protected:
    void refresh() override;

private:
    void updateSwatches(const QColor &current);

    QComboBox *m_theme = nullptr;
    QSpinBox *m_fontSize = nullptr;
    QToolButton *m_custom = nullptr;

    struct Swatch
    {
        QColor color;
        QToolButton *button;
    };
    QVector<Swatch> m_swatches;
};

} // namespace colibry
