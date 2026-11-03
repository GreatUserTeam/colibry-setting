#include "AppearancePage.h"

#include "colibry/Keys.h"
#include "colibry/SettingsStore.h"

#include <QColorDialog>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QStyle>
#include <QToolButton>

namespace colibry {

AppearancePage::AppearancePage(SettingsStore &store, QWidget *parent)
    : SettingsPage(store, parent)
{
    // --- Theme ---------------------------------------------------------
    auto *themeForm = addGroup(tr("Theme"));
    m_theme = new QComboBox(this);
    m_theme->addItem(tr("Follow system"), QStringLiteral("system"));
    m_theme->addItem(tr("Light"), QStringLiteral("light"));
    m_theme->addItem(tr("Dark"), QStringLiteral("dark"));
    themeForm->addRow(tr("Color scheme"), m_theme);

    connect(m_theme, &QComboBox::activated, this, [this] {
        this->store().setValue(Keys::Theme, m_theme->currentData());
    });

    // --- Accent color --------------------------------------------------
    auto *accentForm = addGroup(tr("Accent color"));
    auto *row = new QWidget(this);
    auto *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(8);

    const QStringList presets = {
        QStringLiteral("#3b82f6"), QStringLiteral("#8b5cf6"), QStringLiteral("#ec4899"),
        QStringLiteral("#ef4444"), QStringLiteral("#f59e0b"), QStringLiteral("#10b981"),
        QStringLiteral("#14b8a6"), QStringLiteral("#64748b"),
    };
    for (const QString &hex : presets) {
        auto *button = new QToolButton(row);
        button->setFixedSize(28, 28);
        button->setCursor(Qt::PointingHandCursor);
        button->setToolTip(hex);
        const QColor color(hex);
        connect(button, &QToolButton::clicked, this, [this, hex] {
            this->store().setValue(Keys::Accent, hex);
            updateSwatches(QColor(hex));
        });
        rowLayout->addWidget(button);
        m_swatches.push_back({color, button});
    }

    m_custom = new QToolButton(row);
    m_custom->setText(tr("Custom…"));
    m_custom->setCursor(Qt::PointingHandCursor);
    connect(m_custom, &QToolButton::clicked, this, [this] {
        const QColor current(this->store().value(Keys::Accent).toString());
        const QColor chosen = QColorDialog::getColor(current, this, tr("Choose accent color"));
        if (chosen.isValid()) {
            this->store().setValue(Keys::Accent, chosen.name());
            updateSwatches(chosen);
        }
    });
    rowLayout->addWidget(m_custom);
    rowLayout->addStretch();
    accentForm->addRow(tr("Highlight color"), row);

    // --- Text ----------------------------------------------------------
    auto *textForm = addGroup(tr("Text"));
    m_fontSize = new QSpinBox(this);
    m_fontSize->setRange(8, 24);
    m_fontSize->setSuffix(tr(" pt"));
    textForm->addRow(tr("Interface font size"), m_fontSize);
    connect(m_fontSize, &QSpinBox::valueChanged, this, [this](int value) {
        this->store().setValue(Keys::FontSize, value);
    });

    pageLayout()->addStretch();
    refresh();
}

QString AppearancePage::title() const
{
    return tr("Appearance");
}

QIcon AppearancePage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("preferences-desktop-theme"),
                            style()->standardIcon(QStyle::SP_DesktopIcon));
}

QString AppearancePage::keywords() const
{
    return tr("theme dark light mode color accent font size text");
}

void AppearancePage::refresh()
{
    {
        const QSignalBlocker blocker(m_theme);
        const int index = m_theme->findData(store().value(Keys::Theme));
        m_theme->setCurrentIndex(index < 0 ? 0 : index);
    }
    {
        const QSignalBlocker blocker(m_fontSize);
        m_fontSize->setValue(store().value(Keys::FontSize).toInt());
    }
    updateSwatches(QColor(store().value(Keys::Accent).toString()));
}

void AppearancePage::updateSwatches(const QColor &current)
{
    for (const Swatch &swatch : std::as_const(m_swatches)) {
        const bool selected = swatch.color == current;
        swatch.button->setStyleSheet(
            QStringLiteral("QToolButton { background: %1; border-radius: 14px; border: %2; }")
                .arg(swatch.color.name(),
                     selected ? QStringLiteral("3px solid palette(text)")
                              : QStringLiteral("1px solid palette(mid)")));
    }
}

} // namespace colibry
