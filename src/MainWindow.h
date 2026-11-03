#pragma once

#include <QMainWindow>

class QLineEdit;
class QListWidget;
class QStackedWidget;

namespace colibry {

class SettingsStore;

/// Sidebar (with search) on the left, the selected settings page on the right.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(SettingsStore &store, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void addPage(class SettingsPage *page);
    void applySidebarStyle();
    void applyFilter(const QString &text);

    SettingsStore &m_store;
    QLineEdit *m_search = nullptr;
    QListWidget *m_sidebar = nullptr;
    QStackedWidget *m_stack = nullptr;
};

} // namespace colibry
