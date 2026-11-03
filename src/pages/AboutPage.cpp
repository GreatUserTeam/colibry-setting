#include "AboutPage.h"

#include "colibry/Keys.h"
#include "colibry/SettingsStore.h"

#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>
#include <QSysInfo>
#include <QVBoxLayout>

namespace colibry {

namespace {

QLabel *selectable(const QString &text, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    return label;
}

} // namespace

AboutPage::AboutPage(SettingsStore &store, QWidget *parent)
    : SettingsPage(store, parent)
{
    auto *product = addGroup(tr("Great User System"));
    auto *tagline = new QLabel(tr("An operating system made for people, not programmers."), this);
    tagline->setWordWrap(true);
    product->addRow(tagline);
    product->addRow(tr("Settings version"), selectable(QStringLiteral(Colibry_SETTINGS_VERSION), this));
    product->addRow(tr("Source code"),
                    [this] {
                        auto *link = new QLabel(
                            QStringLiteral("<a href=\"https://github.com/GreatUserTeam/Colibry\">"
                                           "github.com/GreatUserTeam/Colibry</a>"),
                            this);
                        link->setTextFormat(Qt::RichText);
                        link->setOpenExternalLinks(true);
                        return link;
                    }());

    auto *device = addGroup(tr("This device"));
    m_hostname = selectable(QString(), this);
    device->addRow(tr("Device name"), m_hostname);
    device->addRow(tr("Operating system"), selectable(QSysInfo::prettyProductName(), this));
    device->addRow(tr("Kernel"),
                   selectable(QStringLiteral("%1 %2").arg(QSysInfo::kernelType(), QSysInfo::kernelVersion()), this));
    device->addRow(tr("Architecture"), selectable(QSysInfo::currentCpuArchitecture(), this));
    device->addRow(tr("Qt version"), selectable(QString::fromLatin1(qVersion()), this));

    auto *reset = addGroup(tr("Reset"));
    auto *button = new QPushButton(tr("Reset all settings…"), this);
    reset->addRow(button);
    connect(button, &QPushButton::clicked, this, [this] {
        const auto answer = QMessageBox::question(
            this, tr("Reset all settings"),
            tr("Every setting will go back to its default value. Continue?"),
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
        if (answer == QMessageBox::Yes)
            this->store().resetAll();
    });

    pageLayout()->addStretch();
    refresh();
}

QString AboutPage::title() const
{
    return tr("About");
}

QIcon AboutPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("help-about"),
                            style()->standardIcon(QStyle::SP_MessageBoxInformation));
}

QString AboutPage::keywords() const
{
    return tr("about version system info kernel reset factory");
}

void AboutPage::refresh()
{
    m_hostname->setText(store().value(Keys::Hostname).toString());
}

} // namespace colibry
