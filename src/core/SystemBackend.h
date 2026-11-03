#pragma once

#include <QString>

#include <memory>

namespace colibry {

/**
 * Boundary between the Settings UI and the operating system.
 *
 * The UI only stores what the user chose (SettingsStore). Turning that choice
 * into a real effect - changing the volume, the brightness, the hostname - is
 * the job of a SystemBackend.
 *
 * The default implementation only logs the request, which keeps the app fully
 * usable on a development machine. To run on Colibry, subclass SystemBackend,
 * implement the calls with the OS services, and return it from create().
 */
class SystemBackend
{
public:
    virtual ~SystemBackend() = default;

    virtual void setVolume(int percent);
    virtual void setMuted(bool muted);
    virtual void setBrightness(int percent);
    virtual void setNightLight(bool enabled, int strengthPercent);
    virtual void setDisplayScale(int percent);
    virtual void setTimeZone(const QString &ianaId); // empty = system default
    virtual void setAutomaticTime(bool enabled);
    virtual void setHostname(const QString &hostname);

    /// Factory used by main(). Replace to plug in a platform backend.
    static std::unique_ptr<SystemBackend> create();
};

} // namespace colibry
