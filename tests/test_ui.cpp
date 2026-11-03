#include "MainWindow.h"
#include "colibry/Keys.h"
#include "core/SettingsApplier.h"
#include "colibry/SettingsStore.h"
#include "core/SystemBackend.h"
#include "colibry/ThemeManager.h"

#include <QApplication>
#include <QLineEdit>
#include <QListWidget>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

using namespace colibry;

namespace {

/// Records the calls the UI makes towards the OS.
class RecordingBackend : public SystemBackend
{
public:
    void setVolume(int percent) override { volume = percent; }
    void setMuted(bool muted) override { this->muted = muted; }
    void setHostname(const QString &name) override { hostname = name; }

    int volume = -1;
    bool muted = false;
    QString hostname;
};

} // namespace

class UiTest : public QObject
{
    Q_OBJECT

private slots:
    void init()
    {
        QVERIFY(m_dir.isValid());
        m_path = m_dir.filePath(QStringLiteral("ui.ini"));
        QFile::remove(m_path);
    }

    void windowShowsAllSevenPages()
    {
        SettingsStore store(m_path);
        MainWindow window(store);
        const auto *sidebar = window.findChild<QListWidget *>();
        QVERIFY(sidebar);
        QCOMPARE(sidebar->count(), 7);
    }

    void searchFiltersSidebar()
    {
        SettingsStore store(m_path);
        MainWindow window(store);
        const auto *sidebar = window.findChild<QListWidget *>();
        auto *search = window.findChild<QLineEdit *>();
        QVERIFY(sidebar && search);

        search->setText(QStringLiteral("volume"));
        int visible = 0;
        QString name;
        for (int i = 0; i < sidebar->count(); ++i) {
            if (!sidebar->item(i)->isHidden()) {
                ++visible;
                name = sidebar->item(i)->text();
            }
        }
        QCOMPARE(visible, 1);
        QCOMPARE(name, QStringLiteral("Sound"));

        search->clear();
        for (int i = 0; i < sidebar->count(); ++i)
            QVERIFY(!sidebar->item(i)->isHidden());
    }

    void applierForwardsChangesToBackend()
    {
        SettingsStore store(m_path);
        RecordingBackend backend;
        SettingsApplier applier(*qobject_cast<QApplication *>(QApplication::instance()), store, backend);

        store.setValue(Keys::Volume, 33);
        store.setValue(Keys::Muted, true);
        store.setValue(Keys::Hostname, QStringLiteral("my-box"));

        QCOMPARE(backend.volume, 33);
        QCOMPARE(backend.muted, true);
        QCOMPARE(backend.hostname, QStringLiteral("my-box"));
    }

    void applyAllPushesDefaultsAndTheme()
    {
        SettingsStore store(m_path);
        store.setValue(Keys::Theme, QStringLiteral("dark"));
        store.setValue(Keys::Accent, QStringLiteral("#10b981"));
        RecordingBackend backend;
        auto *app = qobject_cast<QApplication *>(QApplication::instance());
        SettingsApplier applier(*app, store, backend);

        applier.applyAll();

        QCOMPARE(backend.volume, 60);
        QVERIFY(ThemeManager::isDark(store));
        QCOMPARE(app->palette().color(QPalette::Highlight), QColor(QStringLiteral("#10b981")));
    }

    void darkAndLightPalettesDiffer()
    {
        const QPalette dark = ThemeManager::makePalette(true);
        const QPalette light = ThemeManager::makePalette(false);
        QVERIFY(dark.color(QPalette::Window).lightness() < light.color(QPalette::Window).lightness());
    }

private:
    QTemporaryDir m_dir;
    QString m_path;
};

QTEST_MAIN(UiTest)
#include "test_ui.moc"
