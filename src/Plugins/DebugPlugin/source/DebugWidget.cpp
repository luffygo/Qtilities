/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
** Option 1: Open Source
** Under this license Qtilities is free software: you can
** redistribute it and/or modify it under the terms of the GNU General
** Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
**
** Qtilities is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Qtilities. If not, see http://www.gnu.org/licenses/.
**
** Option 2: Commercial
** Alternatively, this library is also released under a commercial license
** that allows the development of closed source proprietary applications
** without restrictions on licensing. For more information on this option,
** please see the project website's licensing page:
** http://www.qtilities.org/licensing.html
**
** If you are unsure which license is appropriate for your use, please
** contact support@qtilities.org.
**
****************************************************************************/

#include "DebugWidget.h"
#include "ui_DebugWidget.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

struct Qtilities::Plugins::Debug::DebugWidgetData {
    DebugWidgetData() : objectPoolWidget(0) {}

    ObserverWidget* objectPoolWidget;
};

Qtilities::Plugins::Debug::DebugWidget::DebugWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugWidget)
{
    ui->setupUi(this);
    d = new DebugWidgetData;
    setObjectName("Qtilities Debug Mode");

    // Version:
    ui->labelVersion->setText(QtilitiesApplication::qtilitiesVersion());

    // Global Object Pool:
    d->objectPoolWidget = new ObserverWidget(Qtilities::TreeView);
    d->objectPoolWidget->setObserverContext(OBJECT_MANAGER->objectPool());
    OBJECT_MANAGER->objectPool()->endProcessingCycle();
    OBJECT_MANAGER->objectPool()->useDisplayHints();
    OBJECT_MANAGER->objectPool()->displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    ObserverHints::ActionHints action_hints = 0;
    action_hints |= ObserverHints::ActionFindItem;
    action_hints |= ObserverHints::ActionRefreshView;
    OBJECT_MANAGER->objectPool()->displayHints()->setActionHints(action_hints);
    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ActionToolBar;
    display_flags |= ObserverHints::ItemView;
    OBJECT_MANAGER->objectPool()->displayHints()->setDisplayFlagsHint(display_flags);
    OBJECT_MANAGER->objectPool()->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    OBJECT_MANAGER->objectPool()->displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    OBJECT_MANAGER->objectPool()->refreshViewsLayout();
    if (ui->widgetObjectPoolHolder->layout())
        delete ui->widgetObjectPoolHolder->layout();

    QHBoxLayout* layout = new QHBoxLayout(ui->widgetObjectPoolHolder);
    layout->setMargin(0);
    layout->addWidget(d->objectPoolWidget);
    d->objectPoolWidget->initialize();
    d->objectPoolWidget->show();
    d->objectPoolWidget->toggleSearchBox();

    // Factories:
    ui->listFactories->addItems(OBJECT_MANAGER->factoryNames());
    connect(ui->listFactories,SIGNAL(currentTextChanged(QString)),SLOT(handle_factoryListSelectionChanged(QString)));
    if (ui->listFactories->count() > 0)
        ui->listFactories->setCurrentRow(0);
}

Qtilities::Plugins::Debug::DebugWidget::~DebugWidget()
{
    delete ui;
}

QWidget* Qtilities::Plugins::Debug::DebugWidget::widget() {
    return this;
}

QIcon Qtilities::Plugins::Debug::DebugWidget::icon() const {
    return QIcon(":/debug_log_mode.png");
}

QString Qtilities::Plugins::Debug::DebugWidget::text() const {
    return tr("Qtilities Debugging");
}

void Qtilities::Plugins::Debug::DebugWidget::finalizeMode() {
    if (d->objectPoolWidget->treeView())
        d->objectPoolWidget->treeView()->expandAll();
    OBJECT_MANAGER->objectPool()->startProcessingCycle();
    if (OBJECT_MANAGER->objectPool()->subjectCount() > 0) {
        QList<QObject*> object_list;
        object_list << OBJECT_MANAGER->objectPool()->subjectAt(0);
        d->objectPoolWidget->selectObjects(object_list);
    }
}

void Qtilities::Plugins::Debug::DebugWidget::handle_factoryListSelectionChanged(const QString& factory_name) {
    ui->listFactoryTags->addItems(OBJECT_MANAGER->tagsForFactory(factory_name));
}

void Qtilities::Plugins::Debug::DebugWidget::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}