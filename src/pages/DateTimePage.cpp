#include "DateTimePage.h"

#include "colibry/Keys.h"
#include "colibry/SettingsStore.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDateTime>
#include <QFont>
#include <QFormLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QStyle>
#include <QTimeZone>
#include <QTimer>
#include <QVBoxLayout>

#include <algorithm>

namespace colibry {

namespace {

QTimeZone zoneFor(const QString &ianaId)
{
    QTimeZone zone;
    if (!ianaId.isEmpty())
        zone = QTimeZone(ianaId.toUtf8());
    if (!zone.isValid())
        zone = QTimeZone::systemTimeZone();
    // Minimal systems may ship without tz data: never end up with an invalid zone.
    return zone.isValid() ? zone : QTimeZone::utc();
}

QString formatOffset(int seconds)
{
    const QChar sign = seconds < 0 ? QLatin1Char('-') : QLatin1Char('+');
    const int abs = qAbs(seconds);
    return QStringLiteral("UTC%1%2:%3")
        .arg(sign)
        .arg(abs / 3600, 2, 10, QLatin1Char('0'))
        .arg((abs % 3600) / 60, 2, 10, QLatin1Char('0'));
}

} // namespace

DateTimePage::DateTimePage(SettingsStore &store, QWidget *parent)
    : SettingsPage(store, parent)
{
    // --- Current time --------------------------------------------------
    auto *nowForm = addGroup(tr("Current time"));
    m_time = new QLabel(this);
    QFont big = m_time->font();
    big.setPointSize(big.pointSize() + 14);
    big.setBold(true);
    m_time->setFont(big);
    m_date = new QLabel(this);
    nowForm->addRow(m_time);
    nowForm->addRow(m_date);

    // --- Settings ------------------------------------------------------
    auto *form = addGroup(tr("Date and time"));

    m_auto = new QCheckBox(tr("Set time automatically"), this);
    form->addRow(m_auto);
    connect(m_auto, &QCheckBox::toggled, this, [this](bool on) {
        this->store().setValue(Keys::AutoTime, on);
    });

    m_zone = new QComboBox(this);
    m_zone->setEditable(true);
    m_zone->setInsertPolicy(QComboBox::NoInsert);
    m_zone->addItem(tr("Same as system"), QString());
    QList<QByteArray> ids = QTimeZone::availableTimeZoneIds();
    std::sort(ids.begin(), ids.end());
    for (const QByteArray &id : std::as_const(ids)) {
        const QString text = QString::fromUtf8(id);
        m_zone->addItem(text, text);
    }
    m_zone->completer()->setCompletionMode(QCompleter::PopupCompletion);
    m_zone->completer()->setFilterMode(Qt::MatchContains);
    m_zone->completer()->setCaseSensitivity(Qt::CaseInsensitive);
    form->addRow(tr("Time zone"), m_zone);

    connect(m_zone, &QComboBox::activated, this, [this] {
        const int index = m_zone->currentIndex();
        if (index < 0)
            return;
        this->store().setValue(Keys::TimeZone, m_zone->itemData(index));
        tick();
    });

    m_offset = new QLabel(this);
    m_offset->setEnabled(false);
    form->addRow(m_offset);

    m_clock24 = new QCheckBox(tr("Use 24-hour clock"), this);
    form->addRow(m_clock24);
    connect(m_clock24, &QCheckBox::toggled, this, [this](bool on) {
        this->store().setValue(Keys::Clock24, on);
        tick();
    });

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &DateTimePage::tick);
    m_timer->start();

    pageLayout()->addStretch();
    refresh();
}

QString DateTimePage::title() const
{
    return tr("Date & Time");
}

QIcon DateTimePage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("preferences-system-time"),
                            style()->standardIcon(QStyle::SP_BrowserReload));
}

QString DateTimePage::keywords() const
{
    return tr("clock time date zone timezone 24 hour ntp automatic");
}

void DateTimePage::refresh()
{
    {
        const QSignalBlocker blocker(m_auto);
        m_auto->setChecked(store().value(Keys::AutoTime).toBool());
    }
    {
        const QSignalBlocker blocker(m_zone);
        const int i = m_zone->findData(store().value(Keys::TimeZone));
        m_zone->setCurrentIndex(i < 0 ? 0 : i);
    }
    {
        const QSignalBlocker blocker(m_clock24);
        m_clock24->setChecked(store().value(Keys::Clock24).toBool());
    }
    tick();
}

void DateTimePage::tick()
{
    const QTimeZone zone = zoneFor(store().value(Keys::TimeZone).toString());
    const QDateTime now = QDateTime::currentDateTimeUtc().toTimeZone(zone);
    const QLocale locale = store().locale();

    const bool clock24 = store().value(Keys::Clock24).toBool();
    m_time->setText(clock24 ? now.toString(QStringLiteral("HH:mm:ss"))
                            : locale.toString(now.time(), QStringLiteral("h:mm:ss AP")));
    m_date->setText(locale.toString(now.date(), QLocale::LongFormat));
    m_offset->setText(tr("%1 · %2").arg(QString::fromUtf8(zone.id()), formatOffset(zone.offsetFromUtc(now))));
}

} // namespace colibry
