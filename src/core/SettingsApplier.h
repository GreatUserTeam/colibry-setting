#pragma once

#include <QObject>
#include <QVariant>

class QApplication;

namespace colibry {

class SettingsStore;
class SystemBackend;

/**
 * Listens to the SettingsStore and turns changes into effects:
 * theme changes go to ThemeManager, everything else to the SystemBackend.
 */
class SettingsApplier : public QObject
{
    Q_OBJECT

public:
    SettingsApplier(QApplication &app, SettingsStore &store, SystemBackend &backend,
                    QObject *parent = nullptr);

    /// Pushes every stored value to the app and backend (call once at startup).
    void applyAll();

private:
    void onValueChanged(const QString &key);
    void applyTheme();
    void applyNightLight();

    QApplication &m_app;
    SettingsStore &m_store;
    SystemBackend &m_backend;
};

} // namespace colibry
