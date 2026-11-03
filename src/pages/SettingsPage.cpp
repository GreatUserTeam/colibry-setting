#include "SettingsPage.h"

#include "colibry/SettingsStore.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

namespace colibry {

SettingsPage::SettingsPage(SettingsStore &store, QWidget *parent)
    : QWidget(parent)
    , m_store(store)
    , m_layout(new QVBoxLayout(this))
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(14);

    connect(&m_store, &SettingsStore::reloaded, this, [this] { refresh(); });
}

QFormLayout *SettingsPage::addGroup(const QString &heading)
{
    auto *box = new QGroupBox(heading, this);
    auto *form = new QFormLayout(box);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    form->setVerticalSpacing(10);
    m_layout->addWidget(box);
    return form;
}

} // namespace colibry
