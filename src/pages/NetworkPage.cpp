#include "NetworkPage.h"

#include "colibry/Keys.h"
#include "colibry/SettingsStore.h"

#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSignalBlocker>
#include <QStyle>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

namespace colibry {

NetworkPage::NetworkPage(SettingsStore &store, QWidget *parent)
    : SettingsPage(store, parent)
{
    // --- Device name ---------------------------------------------------
    auto *nameForm = addGroup(tr("Device name"));
    m_hostname = new QLineEdit(this);
    m_hostname->setMaxLength(63);
    m_hostname->setValidator(new QRegularExpressionValidator(
        QRegularExpression(QStringLiteral("[A-Za-z0-9]([A-Za-z0-9-]{0,61}[A-Za-z0-9])?")), m_hostname));
    nameForm->addRow(tr("Hostname"), m_hostname);
    auto *hint = new QLabel(tr("Letters, digits and hyphens only. Press Enter to apply."), this);
    hint->setEnabled(false);
    nameForm->addRow(hint);

    connect(m_hostname, &QLineEdit::editingFinished, this, [this] {
        if (m_hostname->hasAcceptableInput())
            this->store().setValue(Keys::Hostname, m_hostname->text());
        else
            refresh(); // invalid or empty: put the old name back
    });

    // --- Interfaces ----------------------------------------------------
    auto *interfacesForm = addGroup(tr("Network interfaces"));
    m_table = new QTableWidget(0, 4, this);
    m_table->setHorizontalHeaderLabels({tr("Name"), tr("Status"), tr("IPv4 address"), tr("MAC address")});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->hide();
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setMinimumHeight(180);
    interfacesForm->addRow(m_table);

    m_timer = new QTimer(this);
    m_timer->setInterval(3000);
    connect(m_timer, &QTimer::timeout, this, &NetworkPage::refreshInterfaces);

    pageLayout()->addStretch();
    refresh();
}

QString NetworkPage::title() const
{
    return tr("Network");
}

QIcon NetworkPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("network-wired"),
                            style()->standardIcon(QStyle::SP_DriveNetIcon));
}

QString NetworkPage::keywords() const
{
    return tr("network internet wifi ethernet ip hostname device name mac");
}

void NetworkPage::refresh()
{
    {
        const QSignalBlocker blocker(m_hostname);
        m_hostname->setText(store().value(Keys::Hostname).toString());
    }
    refreshInterfaces();
}

void NetworkPage::showEvent(QShowEvent *event)
{
    SettingsPage::showEvent(event);
    refreshInterfaces();
    m_timer->start();
}

void NetworkPage::hideEvent(QHideEvent *event)
{
    SettingsPage::hideEvent(event);
    m_timer->stop();
}

void NetworkPage::refreshInterfaces()
{
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    m_table->setRowCount(static_cast<int>(interfaces.size()));

    int row = 0;
    for (const QNetworkInterface &iface : interfaces) {
        const bool up = iface.flags().testFlag(QNetworkInterface::IsUp)
                        && iface.flags().testFlag(QNetworkInterface::IsRunning);

        QStringList ipv4;
        const QList<QNetworkAddressEntry> entries = iface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                ipv4 << entry.ip().toString();
        }

        const QString status = iface.flags().testFlag(QNetworkInterface::IsLoopBack)
                                   ? tr("Loopback")
                                   : (up ? tr("Connected") : tr("Disconnected"));

        m_table->setItem(row, 0, new QTableWidgetItem(iface.humanReadableName()));
        m_table->setItem(row, 1, new QTableWidgetItem(status));
        m_table->setItem(row, 2, new QTableWidgetItem(ipv4.join(QStringLiteral(", "))));
        m_table->setItem(row, 3, new QTableWidgetItem(iface.hardwareAddress()));
        ++row;
    }
}

} // namespace colibry
