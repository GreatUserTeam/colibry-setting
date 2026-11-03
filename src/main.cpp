#include "MainWindow.h"
#include "core/SettingsApplier.h"
#include "colibry/SettingsStore.h"
#include "core/SystemBackend.h"

#include <QApplication>

#include <memory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("colibry-settings"));
    QApplication::setApplicationDisplayName(QStringLiteral("Settings"));
    QApplication::setApplicationVersion(QStringLiteral(Colibry_SETTINGS_VERSION));
    QApplication::setOrganizationName(QStringLiteral("GreatUserTeam"));
    QApplication::setDesktopFileName(QStringLiteral("colibry-settings"));

    colibry::SettingsStore store;
    const std::unique_ptr<colibry::SystemBackend> backend = colibry::SystemBackend::create();

    colibry::SettingsApplier applier(app, store, *backend);
    applier.applyAll();

    colibry::MainWindow window(store);
    window.show();

    return QApplication::exec();
}
