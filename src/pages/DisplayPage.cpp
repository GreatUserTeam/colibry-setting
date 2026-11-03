#include "DisplayPage.h"

#include "colibry/Keys.h"
#include "colibry/SettingsStore.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QSignalBlocker>
#include <QSlider>
#include <QStyle>

namespace colibry {

namespace {

QWidget *sliderRow(QSlider *slider, QLabel *valueLabel, QWidget *parent)
{
    auto *row = new QWidget(parent);
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    valueLabel->setMinimumWidth(40);
    valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(slider, 1);
    layout->addWidget(valueLabel);
    return row;
}

} // namespace

DisplayPage::DisplayPage(SettingsStore &store, QWidget *parent)
    : SettingsPage(store, parent)
{
    // --- Connected screens --------------------------------------------
    auto *screensForm = addGroup(tr("Screens"));
    m_screens = new QLabel(this);
    m_screens->setTextFormat(Qt::RichText);
    m_screens->setWordWrap(true);
    screensForm->addRow(m_screens);
    connect(qApp, &QGuiApplication::screenAdded, this, [this] { refreshScreens(); });
    connect(qApp, &QGuiApplication::screenRemoved, this, [this] { refreshScreens(); });

    // --- Scale ---------------------------------------------------------
    auto *scaleForm = addGroup(tr("Scale"));
    m_scale = new QComboBox(this);
    for (const int percent : {100, 125, 150, 175, 200})
        m_scale->addItem(QStringLiteral("%1%").arg(percent), percent);
    scaleForm->addRow(tr("Interface scale"), m_scale);
    connect(m_scale, &QComboBox::activated, this, [this] {
        this->store().setValue(Keys::Scale, m_scale->currentData());
    });

    // --- Brightness ----------------------------------------------------
    auto *brightnessForm = addGroup(tr("Brightness"));
    m_brightness = new QSlider(Qt::Horizontal, this);
    m_brightness->setRange(5, 100);
    m_brightnessValue = new QLabel(this);
    brightnessForm->addRow(tr("Screen brightness"), sliderRow(m_brightness, m_brightnessValue, this));
    connect(m_brightness, &QSlider::valueChanged, this, [this](int value) {
        m_brightnessValue->setText(QStringLiteral("%1%").arg(value));
        this->store().setValue(Keys::Brightness, value);
    });

    // --- Night light ---------------------------------------------------
    auto *nightForm = addGroup(tr("Night light"));
    m_nightLight = new QCheckBox(tr("Warm up screen colors"), this);
    nightForm->addRow(m_nightLight);
    connect(m_nightLight, &QCheckBox::toggled, this, [this](bool on) {
        this->store().setValue(Keys::NightLight, on);
        updateNightLightEnabled();
    });
    m_nightStrength = new QSlider(Qt::Horizontal, this);
    m_nightStrength->setRange(0, 100);
    nightForm->addRow(tr("Warmth"), m_nightStrength);
    connect(m_nightStrength, &QSlider::valueChanged, this, [this](int value) {
        this->store().setValue(Keys::NightLightStrength, value);
    });

    pageLayout()->addStretch();
    refresh();
}

QString DisplayPage::title() const
{
    return tr("Display");
}

QIcon DisplayPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("video-display"),
                            style()->standardIcon(QStyle::SP_ComputerIcon));
}

QString DisplayPage::keywords() const
{
    return tr("screen monitor resolution scale dpi brightness night light blue");
}

void DisplayPage::refresh()
{
    {
        const QSignalBlocker blocker(m_scale);
        const int i = m_scale->findData(store().value(Keys::Scale));
        m_scale->setCurrentIndex(i < 0 ? 0 : i);
    }
    {
        const QSignalBlocker blocker(m_brightness);
        m_brightness->setValue(store().value(Keys::Brightness).toInt());
        m_brightnessValue->setText(QStringLiteral("%1%").arg(m_brightness->value()));
    }
    {
        const QSignalBlocker blocker(m_nightLight);
        m_nightLight->setChecked(store().value(Keys::NightLight).toBool());
    }
    {
        const QSignalBlocker blocker(m_nightStrength);
        m_nightStrength->setValue(store().value(Keys::NightLightStrength).toInt());
    }
    updateNightLightEnabled();
    refreshScreens();
}

void DisplayPage::refreshScreens()
{
    const QList<QScreen *> screens = QGuiApplication::screens();
    if (screens.isEmpty()) {
        m_screens->setText(tr("No screens detected."));
        return;
    }

    QStringList lines;
    int number = 0;
    for (const QScreen *screen : screens) {
        ++number;
        const QSize size = screen->size() * screen->devicePixelRatio();
        const bool primary = screen == QGuiApplication::primaryScreen();
        const QString name = screen->name().isEmpty() ? tr("Screen %1").arg(number) : screen->name();
        lines << tr("<b>%1</b>%2<br>%3 × %4 px · %5 Hz · %6 DPI")
                     .arg(name.toHtmlEscaped(),
                          primary ? tr(" (primary)") : QString(),
                          QString::number(size.width()), QString::number(size.height()),
                          QString::number(screen->refreshRate(), 'f', 0),
                          QString::number(screen->physicalDotsPerInch(), 'f', 0));
    }
    m_screens->setText(lines.join(QStringLiteral("<br><br>")));
}

void DisplayPage::updateNightLightEnabled()
{
    m_nightStrength->setEnabled(m_nightLight->isChecked());
}

} // namespace colibry
