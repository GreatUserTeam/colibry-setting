#pragma once

#include "SettingsPage.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QSlider;

namespace colibry {

class DisplayPage : public SettingsPage
{
    Q_OBJECT

public:
    explicit DisplayPage(SettingsStore &store, QWidget *parent = nullptr);

    QString title() const override;
    QIcon icon() const override;
    QString keywords() const override;

protected:
    void refresh() override;

private:
    void refreshScreens();
    void updateNightLightEnabled();

    QLabel *m_screens = nullptr;
    QComboBox *m_scale = nullptr;
    QSlider *m_brightness = nullptr;
    QLabel *m_brightnessValue = nullptr;
    QCheckBox *m_nightLight = nullptr;
    QSlider *m_nightStrength = nullptr;
};

} // namespace colibry
