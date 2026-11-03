#pragma once

#include "SettingsPage.h"

class QCheckBox;
class QLabel;
class QSlider;

namespace colibry {

class SoundPage : public SettingsPage
{
    Q_OBJECT

public:
    explicit SoundPage(SettingsStore &store, QWidget *parent = nullptr);

    QString title() const override;
    QIcon icon() const override;
    QString keywords() const override;

protected:
    void refresh() override;

private:
    void updateEnabled();

    QSlider *m_volume = nullptr;
    QLabel *m_volumeValue = nullptr;
    QCheckBox *m_muted = nullptr;
    QCheckBox *m_systemSounds = nullptr;
};

} // namespace colibry
