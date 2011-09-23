#ifndef CREATEQUERYDIALOG_H_
#define CREATEQUERYDIALOG_H_

#include <QtGui/QDialog>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDialogButtonBox>
#include "model/queryentitiesmodel.h"
#include "../../containeres/idmcontainer.h"
#include "../../../../tools/events/imp/keyboardeventhandler.h"
#include "../../../../tools/events/imp/keyboardeventsource.h"


using namespace FileSystem::Plugins::Idm;

class CreateQueryDialog : public QDialog
{
	Q_OBJECT

public:
	CreateQueryDialog(const IdmContainer &container, IdmEntity *entity, QWidget *parent = 0);

    virtual void accept();

private:
	enum ActionId
	{
		Create,
		Remove,
		AddProperty,
		RemoveProperty
	};

private Q_SLOTS:
	void actionTriggered(QAction *action);

private:
	typedef KeyboardEventSource<
				EventSourceBase<
					QTreeView
				>
			> EntitiesTreeView;
	typedef KeyboardEventHandler<
				EventHandlerBase<
					CreateQueryDialog
				>
			> EntitiesTreeViewHandler;

private:
	void createEntity();

private:
	QModelIndex currentIndex();

private:
	IdmContainer m_container;
	EntitiesTreeViewHandler m_handler;
	QToolBar m_toolBar;
	EntitiesTreeView m_view;
	QueryEntitiesModel m_model;
	QTreeView m_view2;
	QDialogButtonBox m_buttonBox;
	QHBoxLayout m_horizontalLayout;
	QVBoxLayout m_verticatLayout;
};

#endif /* CREATEQUERYDIALOG_H_ */
