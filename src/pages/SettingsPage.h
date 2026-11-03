#pragma once

#include <QIcon>
#include <QWidget>

class QFormLayout;
class QVBoxLayout;

namespace colibry {

class SettingsStore;

/// Base class for one section of the Settings window.
class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(SettingsStore &store, QWidget *parent = nullptr);

    /// Name shown in the sidebar and as the page heading.
    virtual QString title() const = 0;

    /// Sidebar icon.
    virtual QIcon icon() const = 0;

    /// Extra words matched by the sidebar search box.
    virtual QString keywords() const { return {}; }

protected:
    /// Reload every widget from the store (must not write back to the store).
    virtual void refresh() = 0;

    SettingsStore &store() const { return m_store; }

    /// Adds a titled group box to the page and returns its form layout.
    QFormLayout *addGroup(const QString &heading);

    QVBoxLayout *pageLayout() const { return m_layout; }

private:
    SettingsStore &m_store;
    QVBoxLayout *m_layout;
};

} // namespace colibry
