#include "MainWindow.h"

#include "colibry/Keys.h"
#include "colibry/SettingsStore.h"
#include "pages/AboutPage.h"
#include "pages/AppearancePage.h"
#include "pages/DateTimePage.h"
#include "pages/DisplayPage.h"
#include "pages/LanguageRegionPage.h"
#include "pages/NetworkPage.h"
#include "pages/SettingsPage.h"
#include "pages/SoundPage.h"

#include <QCloseEvent>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace colibry {

namespace {
constexpr int kSearchTextRole = Qt::UserRole + 1;
}

MainWindow::MainWindow(SettingsStore &store, QWidget *parent)
    : QMainWindow(parent)
    , m_store(store)
{
    setWindowTitle(tr("Settings"));
    setWindowIcon(QIcon::fromTheme(QStringLiteral("preferences-system")));
    resize(900, 620);
    setMinimumSize(640, 440);

    auto *central = new QWidget(this);
    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // --- Sidebar -------------------------------------------------------
    auto *side = new QWidget(central);
    side->setFixedWidth(230);
    auto *sideLayout = new QVBoxLayout(side);
    sideLayout->setContentsMargins(10, 10, 6, 10);

    m_search = new QLineEdit(side);
    m_search->setPlaceholderText(tr("Search settings"));
    m_search->setClearButtonEnabled(true);
    sideLayout->addWidget(m_search);

    m_sidebar = new QListWidget(side);
    m_sidebar->setFrameShape(QFrame::NoFrame);
    m_sidebar->setIconSize(QSize(20, 20));
    m_sidebar->setSpacing(2);
    applySidebarStyle();
    sideLayout->addWidget(m_sidebar, 1);

    root->addWidget(side);

    // --- Content -------------------------------------------------------
    m_stack = new QStackedWidget(central);
    root->addWidget(m_stack, 1);
    setCentralWidget(central);

    addPage(new AppearancePage(m_store));
    addPage(new LanguageRegionPage(m_store));
    addPage(new DisplayPage(m_store));
    addPage(new SoundPage(m_store));
    addPage(new NetworkPage(m_store));
    addPage(new DateTimePage(m_store));
    addPage(new AboutPage(m_store));

    connect(m_sidebar, &QListWidget::currentRowChanged, m_stack, &QStackedWidget::setCurrentIndex);
    connect(m_search, &QLineEdit::textChanged, this, &MainWindow::applyFilter);
    m_sidebar->setCurrentRow(0);

    const QByteArray geometry = m_store.value(Keys::WindowGeometry).toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);
}

void MainWindow::addPage(SettingsPage *page)
{
    // Heading + scrollable page body.
    auto *container = new QWidget;
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(14);

    auto *heading = new QLabel(page->title(), container);
    QFont font = heading->font();
    font.setPointSize(font.pointSize() + 8);
    font.setBold(true);
    heading->setFont(font);
    layout->addWidget(heading);
    layout->addWidget(page, 1);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(container);
    m_stack->addWidget(scroll);

    auto *item = new QListWidgetItem(page->icon(), page->title(), m_sidebar);
    item->setData(kSearchTextRole, (page->title() + QLatin1Char(' ') + page->keywords()).toLower());
}

void MainWindow::applyFilter(const QString &text)
{
    const QString needle = text.trimmed().toLower();
    int firstVisible = -1;

    for (int row = 0; row < m_sidebar->count(); ++row) {
        QListWidgetItem *item = m_sidebar->item(row);
        const bool match = needle.isEmpty()
                           || item->data(kSearchTextRole).toString().contains(needle);
        item->setHidden(!match);
        if (match && firstVisible < 0)
            firstVisible = row;
    }

    QListWidgetItem *current = m_sidebar->currentItem();
    if ((!current || current->isHidden()) && firstVisible >= 0)
        m_sidebar->setCurrentRow(firstVisible);
}

void MainWindow::applySidebarStyle()
{
    // A widget stylesheet resolves palette() when it is applied, so it has to be
    // applied again whenever the theme or accent color changes.
    m_sidebar->setStyleSheet(QStringLiteral(
        "QListWidget { background: transparent; color: palette(text); outline: none; }"
        "QListWidget::item { padding: 8px 6px; border-radius: 6px; }"
        "QListWidget::item:hover { background: palette(midlight); }"
        "QListWidget::item:selected, QListWidget::item:selected:!active {"
        "  background: palette(highlight); color: palette(highlighted-text); }"));
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (m_sidebar
        && (event->type() == QEvent::ApplicationPaletteChange || event->type() == QEvent::PaletteChange))
        applySidebarStyle();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_store.setValue(Keys::WindowGeometry, saveGeometry());
    QMainWindow::closeEvent(event);
}

} // namespace colibry
