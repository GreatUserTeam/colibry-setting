#include "SystemBackend.h"

#include <QDebug>

namespace colibry {

void SystemBackend::setVolume(int percent)
{
    qInfo().noquote() << "[backend] volume ->" << percent << "%";
}

void SystemBackend::setMuted(bool muted)
{
    qInfo().noquote() << "[backend] muted ->" << muted;
}

void SystemBackend::setBrightness(int percent)
{
    qInfo().noquote() << "[backend] brightness ->" << percent << "%";
}

void SystemBackend::setNightLight(bool enabled, int strengthPercent)
{
    qInfo().noquote() << "[backend] night light ->" << enabled << strengthPercent << "%";
}

void SystemBackend::setDisplayScale(int percent)
{
    qInfo().noquote() << "[backend] display scale ->" << percent << "%";
}

void SystemBackend::setTimeZone(const QString &ianaId)
{
    qInfo().noquote() << "[backend] time zone ->" << (ianaId.isEmpty() ? QStringLiteral("(system)") : ianaId);
}

void SystemBackend::setAutomaticTime(bool enabled)
{
    qInfo().noquote() << "[backend] automatic time ->" << enabled;
}

void SystemBackend::setHostname(const QString &hostname)
{
    qInfo().noquote() << "[backend] hostname ->" << hostname;
}

std::unique_ptr<SystemBackend> SystemBackend::create()
{
    return std::make_unique<SystemBackend>();
}

} // namespace colibry
