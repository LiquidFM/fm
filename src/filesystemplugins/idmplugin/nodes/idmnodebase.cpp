#include "idmnodebase.h"
#include "idmnode.h"
#include "query/idmnodequeryresults.h"
#include "control/idmcopycontrol.h"
#include "../gui/list/listentitydialog.h"
#include "../gui/create/createentitydialog.h"
#include "../gui/choose/choosefileentitydialog.h"
#include "../gui/query/create/createquerydialog.h"
#include "../../../application.h"
#include <QtGui/QMessageBox>


IDM_PLUGIN_NS_BEGIN

IdmNodeBase::IdmNodeBase(const IdmContainer &container, const Info &info, Node *parent) :
	FolderNode(info, parent),
	m_container(container)
{}

ICopyControl *IdmNodeBase::createControl(INodeView *view) const
{
	if (IdmEntity *entity = ChooseFileEntityDialog::choose(m_container, &Application::instance()->mainWindow()))
		return new IdmCopyControl(m_container, entity, absoluteFilePath());
	else
		return 0;
}

void IdmNodeBase::menuAction(QAction *action, INodeView *view)
{
	switch (static_cast<IdmContainer::MenuId>(action->data().toInt()))
	{
		case IdmContainer::Create:
		{
			CreateEntityDialog dialog(m_container, QString(), &Application::instance()->mainWindow());

			if (dialog.exec() == CreateEntityDialog::Accepted)
				if (m_container.transaction())
					if (IdmEntity *entity = m_container.createEntity(dialog.name(), dialog.type(), dialog.shortFormat()))
						if (entity->type() == Database::Composite)
						{
							bool ok = true;

							for (CreateEntityDialog::size_type i = 0, size = dialog.size(); i < size; ++i)
								if (!m_container.addProperty(entity, dialog.property(i), dialog.propertyName(i)))
								{
									ok = false;
									m_container.rollback();
									QMessageBox::critical(&Application::instance()->mainWindow(), tr("Error"), m_container.lastError());
									break;
								}

							if (ok && !m_container.commit())
							{
								m_container.rollback();
								QMessageBox::critical(&Application::instance()->mainWindow(), tr("Error"), m_container.lastError());
							}
						}
						else
						{
							if (!m_container.commit())
							{
								m_container.rollback();
								QMessageBox::critical(&Application::instance()->mainWindow(), tr("Error"), m_container.lastError());
							}
						}
					else
					{
						m_container.rollback();
						QMessageBox::critical(&Application::instance()->mainWindow(), tr("Error"), m_container.lastError());
					}
				else
					QMessageBox::critical(&Application::instance()->mainWindow(), tr("Error"), m_container.lastError());

			break;
		}
		case IdmContainer::Find:
		{
			if (IdmEntity *entity = ChooseFileEntityDialog::choose(m_container, &Application::instance()->mainWindow()))
			{
				CreateQueryDialog dialog(m_container, entity, &Application::instance()->mainWindow());

				if (dialog.exec() == CreateQueryDialog::Accepted)
					switchTo(new IdmNodeQueryResults(m_container, dialog.query(), absoluteFilePath(), this), view);
			}

			break;
		}
		case IdmContainer::List:
		{
			if (m_container.transaction())
			{
				ListEntityDialog dialog(m_container, &Application::instance()->mainWindow());

				if (dialog.exec() == ListEntityDialog::Accepted)
				{
					if (!m_container.commit())
					{
						m_container.rollback();
						QMessageBox::critical(&Application::instance()->mainWindow(), tr("Error"), m_container.lastError());
					}
				}
				else
					m_container.rollback();
			}
			else
				QMessageBox::critical(&Application::instance()->mainWindow(), tr("Error"), m_container.lastError());

			break;
		}
	}
}

void IdmNodeBase::createFile(const QModelIndex &index, INodeView *view)
{

}

void IdmNodeBase::createDirectory(const QModelIndex &index, INodeView *view)
{

}

void IdmNodeBase::rename(const QModelIndexList &list, INodeView *view)
{

}

void IdmNodeBase::remove(const QModelIndexList &list, INodeView *view)
{
//	if (m_container.transaction())
//		if (!processIndexList(list, IdmFunctors::callTo(this, &IdmNodeBase::processRemoveItem)))
//			m_container.rollback();
//		else
//			if (!m_container.commit())
//			{
//				QMessageBox::critical(&Application::instance()->mainWindow(), tr("Error"), m_container.lastError());
//				m_container.rollback();
//			}
}

void IdmNodeBase::cancel(const QModelIndexList &list, INodeView *view)
{

}

void IdmNodeBase::calculateSize(const QModelIndexList &list, INodeView *view)
{

}

void IdmNodeBase::pathToClipboard(const QModelIndexList &list, INodeView *view)
{

}

void IdmNodeBase::copy(const INodeView *source, INodeView *destination)
{

}

void IdmNodeBase::move(const INodeView *source, INodeView *destination)
{

}

const INodeView::MenuActionList &IdmNodeBase::menuActions() const
{
	return m_container.menuActions();
}

IDM_PLUGIN_NS_END
