#include "LanguageRegionPage.h"

#include "colibry/Keys.h"
#include "colibry/SettingsStore.h"

#include <QComboBox>
#include <QDateTime>
#include <QFormLayout>
#include <QLabel>
#include <QLocale>
#include <QSignalBlocker>
#include <QStyle>
#include <QTimer>

namespace colibry {

LanguageRegionPage::LanguageRegionPage(SettingsStore &store, QWidget *parent)
    : SettingsPage(store, parent)
{
    // --- Language ------------------------------------------------------
    auto *langForm = addGroup(tr("Language"));
    m_language = new QComboBox(this);
    // Names are written in their own language on purpose.
    m_language->addItem(QStringLiteral("English"), QStringLiteral("en"));
    m_language->addItem(QStringLiteral("Українська"), QStringLiteral("uk"));
    m_language->addItem(QStringLiteral("Русский"), QStringLiteral("ru"));
    m_language->addItem(QStringLiteral("Deutsch"), QStringLiteral("de"));
    m_language->addItem(QStringLiteral("Français"), QStringLiteral("fr"));
    m_language->addItem(QStringLiteral("Español"), QStringLiteral("es"));
    m_language->addItem(QStringLiteral("Polski"), QStringLiteral("pl"));
    langForm->addRow(tr("Interface language"), m_language);
    connect(m_language, &QComboBox::activated, this, [this] {
        this->store().setValue(Keys::Language, m_language->currentData());
    });
    auto *langNote = new QLabel(tr("The new language is applied the next time the system starts."), this);
    langNote->setWordWrap(true);
    langNote->setEnabled(false);
    langForm->addRow(langNote);

    // --- Region --------------------------------------------------------
    auto *regionForm = addGroup(tr("Region and formats"));
    m_region = new QComboBox(this);
    m_region->addItem(tr("Same as system"), QString());
    m_region->addItem(QStringLiteral("United States"), QStringLiteral("en_US"));
    m_region->addItem(QStringLiteral("United Kingdom"), QStringLiteral("en_GB"));
    m_region->addItem(QStringLiteral("Україна"), QStringLiteral("uk_UA"));
    m_region->addItem(QStringLiteral("Россия"), QStringLiteral("ru_RU"));
    m_region->addItem(QStringLiteral("Deutschland"), QStringLiteral("de_DE"));
    m_region->addItem(QStringLiteral("France"), QStringLiteral("fr_FR"));
    m_region->addItem(QStringLiteral("España"), QStringLiteral("es_ES"));
    m_region->addItem(QStringLiteral("Polska"), QStringLiteral("pl_PL"));
    regionForm->addRow(tr("Region"), m_region);
    connect(m_region, &QComboBox::activated, this, [this] {
        this->store().setValue(Keys::Region, m_region->currentData());
        updatePreview();
    });

    m_firstDay = new QComboBox(this);
    m_firstDay->addItem(tr("Monday"), static_cast<int>(Qt::Monday));
    m_firstDay->addItem(tr("Saturday"), static_cast<int>(Qt::Saturday));
    m_firstDay->addItem(tr("Sunday"), static_cast<int>(Qt::Sunday));
    regionForm->addRow(tr("First day of the week"), m_firstDay);
    connect(m_firstDay, &QComboBox::activated, this, [this] {
        this->store().setValue(Keys::FirstDayOfWeek, m_firstDay->currentData());
    });

    m_preview = new QLabel(this);
    m_preview->setTextInteractionFlags(Qt::TextSelectableByMouse);
    regionForm->addRow(tr("Preview"), m_preview);

    // Keep the preview in sync with the 12/24-hour setting and the clock.
    connect(&this->store(), &SettingsStore::valueChanged, this, [this](const QString &key) {
        if (key == Keys::Clock24)
            updatePreview();
    });
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &LanguageRegionPage::updatePreview);
    m_timer->start();

    pageLayout()->addStretch();
    refresh();
}

QString LanguageRegionPage::title() const
{
    return tr("Language & Region");
}

QIcon LanguageRegionPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("preferences-desktop-locale"),
                            style()->standardIcon(QStyle::SP_FileDialogDetailedView));
}

QString LanguageRegionPage::keywords() const
{
    return tr("language locale region format date week keyboard translate");
}

void LanguageRegionPage::refresh()
{
    {
        const QSignalBlocker blocker(m_language);
        const int i = m_language->findData(store().value(Keys::Language));
        m_language->setCurrentIndex(i < 0 ? 0 : i);
    }
    {
        const QSignalBlocker blocker(m_region);
        const int i = m_region->findData(store().value(Keys::Region));
        m_region->setCurrentIndex(i < 0 ? 0 : i);
    }
    {
        const QSignalBlocker blocker(m_firstDay);
        const int i = m_firstDay->findData(store().value(Keys::FirstDayOfWeek));
        m_firstDay->setCurrentIndex(i < 0 ? 0 : i);
    }
    updatePreview();
}

void LanguageRegionPage::updatePreview()
{
    const QLocale locale = store().locale();
    const QDateTime now = QDateTime::currentDateTime();
    const bool clock24 = store().value(Keys::Clock24).toBool();
    const QString time = clock24 ? now.toString(QStringLiteral("HH:mm"))
                                 : locale.toString(now.time(), QStringLiteral("h:mm AP"));
    m_preview->setText(QStringLiteral("%1, %2").arg(locale.toString(now.date(), QLocale::LongFormat), time));
}

} // namespace colibry
