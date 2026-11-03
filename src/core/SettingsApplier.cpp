#include "SettingsApplier.h"

#include "colibry/Keys.h"
#include "colibry/SettingsStore.h"
#include "SystemBackend.h"
#include "colibry/ThemeManager.h"

#include <QApplication>
#include <QGuiApplication>
#include <QStyleHints>

namespace colibry {

SettingsApplier::SettingsApplier(QApplication &app, SettingsStore &store, SystemBackend &backend,
                                 QObject *parent)
    : QObject(parent)
    , m_app(app)
    , m_store(store)
    , m_backend(backend)
{
    connect(&m_store, &SettingsStore::valueChanged, this,
            [this](const QString &key) { onValueChanged(key); });
    connect(&m_store, &SettingsStore::reloaded, this, [this] { applyAll(); });

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // Follow the OS light/dark switch while "Follow system" is selected.
    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this] {
        if (m_store.value(Keys::Theme).toString() == QStringLiteral("system"))
            applyTheme();
    });
#endif
}

void SettingsApplier::applyAll()
{
    applyTheme();
    m_backend.setVolume(m_store.value(Keys::Volume).toInt());
    m_backend.setMuted(m_store.value(Keys::Muted).toBool());
    m_backend.setBrightness(m_store.value(Keys::Brightness).toInt());
    applyNightLight();
    m_backend.setDisplayScale(m_store.value(Keys::Scale).toInt());
    m_backend.setTimeZone(m_store.value(Keys::TimeZone).toString());
    m_backend.setAutomaticTime(m_store.value(Keys::AutoTime).toBool());
    m_backend.setHostname(m_store.value(Keys::Hostname).toString());
}

void SettingsApplier::onValueChanged(const QString &key)
{
    if (key == Keys::Theme || key == Keys::Accent || key == Keys::FontSize)
        applyTheme();
    else if (key == Keys::Volume)
        m_backend.setVolume(m_store.value(key).toInt());
    else if (key == Keys::Muted)
        m_backend.setMuted(m_store.value(key).toBool());
    else if (key == Keys::Brightness)
        m_backend.setBrightness(m_store.value(key).toInt());
    else if (key == Keys::NightLight || key == Keys::NightLightStrength)
        applyNightLight();
    else if (key == Keys::Scale)
        m_backend.setDisplayScale(m_store.value(key).toInt());
    else if (key == Keys::TimeZone)
        m_backend.setTimeZone(m_store.value(key).toString());
    else if (key == Keys::AutoTime)
        m_backend.setAutomaticTime(m_store.value(key).toBool());
    else if (key == Keys::Hostname)
        m_backend.setHostname(m_store.value(key).toString());
}

void SettingsApplier::applyTheme()
{
    ThemeManager::apply(m_app, m_store);
}

void SettingsApplier::applyNightLight()
{
    m_backend.setNightLight(m_store.value(Keys::NightLight).toBool(),
                            m_store.value(Keys::NightLightStrength).toInt());
}

} // namespace colibry
