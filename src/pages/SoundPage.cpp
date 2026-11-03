#include "SoundPage.h"

#include "colibry/Keys.h"
#include "colibry/SettingsStore.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QSlider>
#include <QStyle>

namespace colibry {

SoundPage::SoundPage(SettingsStore &store, QWidget *parent)
    : SettingsPage(store, parent)
{
    auto *outputForm = addGroup(tr("Output"));

    auto *row = new QWidget(this);
    auto *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    m_volume = new QSlider(Qt::Horizontal, row);
    m_volume->setRange(0, 100);
    m_volumeValue = new QLabel(row);
    m_volumeValue->setMinimumWidth(40);
    m_volumeValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    rowLayout->addWidget(m_volume, 1);
    rowLayout->addWidget(m_volumeValue);
    outputForm->addRow(tr("Volume"), row);

    connect(m_volume, &QSlider::valueChanged, this, [this](int value) {
        m_volumeValue->setText(QStringLiteral("%1%").arg(value));
        this->store().setValue(Keys::Volume, value);
    });

    m_muted = new QCheckBox(tr("Mute"), this);
    outputForm->addRow(m_muted);
    connect(m_muted, &QCheckBox::toggled, this, [this](bool on) {
        this->store().setValue(Keys::Muted, on);
        updateEnabled();
    });

    auto *effectsForm = addGroup(tr("Sound effects"));
    m_systemSounds = new QCheckBox(tr("Play system sounds"), this);
    effectsForm->addRow(m_systemSounds);
    connect(m_systemSounds, &QCheckBox::toggled, this, [this](bool on) {
        this->store().setValue(Keys::SystemSounds, on);
    });

    pageLayout()->addStretch();
    refresh();
}

QString SoundPage::title() const
{
    return tr("Sound");
}

QIcon SoundPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("audio-volume-high"),
                            style()->standardIcon(QStyle::SP_MediaVolume));
}

QString SoundPage::keywords() const
{
    return tr("audio volume mute speaker sound effects");
}

void SoundPage::refresh()
{
    {
        const QSignalBlocker blocker(m_volume);
        m_volume->setValue(store().value(Keys::Volume).toInt());
        m_volumeValue->setText(QStringLiteral("%1%").arg(m_volume->value()));
    }
    {
        const QSignalBlocker blocker(m_muted);
        m_muted->setChecked(store().value(Keys::Muted).toBool());
    }
    {
        const QSignalBlocker blocker(m_systemSounds);
        m_systemSounds->setChecked(store().value(Keys::SystemSounds).toBool());
    }
    updateEnabled();
}

void SoundPage::updateEnabled()
{
    m_volume->setEnabled(!m_muted->isChecked());
}

} // namespace colibry
