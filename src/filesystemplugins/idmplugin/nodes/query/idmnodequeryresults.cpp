#include "idmnodequeryresults.h"
#include "functors/idmqueryresultsfunctor.h"
#include "control/idmqueryresultscopycontrol.h"
#include "events/idmqueryresultsmodelevents.h"
#include "tasks/scan/idmnodequeryresultsscantask.h"
#include "tasks/perform/idmnodequeryresultsremovefilestask.h"
#include "items/idmqueryresultvalueitem.h"
#include "items/idmqueryresultpropertyitem.h"
#include "items/idmqueryresultpathvalueitem.h"
#include "../folder/idmfoldernode.h"
#include "../../gui/tools/idmentityvaluecreationtools.h"
#include "../../../../tools/containers/union.h"
#include "../../../../tools/widgets/stringdialog/stringdialog.h"
#include "../../../../filesystem/tools/filesystemcommontools.h"
#include "../../../../application.h"
#include <QtGui/QMessageBox>


IDM_PLUGIN_NS_BEGIN

IdmNodeQueryResults::IdmNodeQueryResults(const IdmContainer &container, const Select &query, const Info &info, Node *parent) :
	TasksNode(m_itemsContainer, parent),
	m_items(m_itemsContainer.m_container),
	m_delegate(container),
	m_container(container),
	m_reader(m_container, query),
	m_info(info),
	m_label(tr("Found \"%1\" entities...").arg(m_reader.entity()->name()))
{}

IdmNodeQueryResults::~IdmNodeQueryResults()
{}

bool IdmNodeQueryResults::event(QEvent *e)
{
	switch (static_cast<ModelEvent::Type>(e->type()))
	{
		case ModelEvent::ScanFilesForRemove:
		{
			e->accept();
			scanForRemove(static_cast<BaseTask::Event*>(e));
			return true;
		}
		case ModelEvent::RemoveFiles:
		{
			e->accept();
			performRemove(static_cast<BaseTask::Event*>(e));
			return true;
		}
		default:
			break;
	}

	return TasksNode::event(e);
}

void IdmNodeQueryResults::fetchMore(const QModelIndex &parent)
{
	IdmEntityValue::Holder item;
	ItemsContainer::List list;

	list.reserve(PrefetchLimit);

	for (qint32 actualLimit = 0; actualLimit < PrefetchLimit; ++actualLimit)
		if (item = m_reader.next())
			list.push_back(new QueryResultRootItem(item));
		else
			break;

	if (!list.isEmpty())
	{
		beginInsertRows(parent, m_items.size(), m_items.size() + list.size() - 1);
		m_items.append(list);
		endInsertRows();
	}
}

bool IdmNodeQueryResults::canFetchMore(const QModelIndex &parent) const
{
    return !parent.isValid() && !m_reader.eof();
}

Qt::ItemFlags IdmNodeQueryResults::flags(const QModelIndex &index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant IdmNodeQueryResults::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (section == 0 && orientation == Qt::Horizontal)
		switch (role)
		{
			case Qt::DisplayRole:
				return m_label;

			case Qt::TextAlignmentRole:
				return Qt::AlignCenter;

			default:
				break;
		}

	return QVariant();
}

FileTypeId IdmNodeQueryResults::id() const
{
	return m_info.id();
}

QIcon IdmNodeQueryResults::icon() const
{
	return m_info.icon();
}

QString IdmNodeQueryResults::name() const
{
	return m_info.name();
}

QString IdmNodeQueryResults::description() const
{
	return m_info.description();
}

int IdmNodeQueryResults::columnCount(const QModelIndex &parent) const
{
	return 1;
}

bool IdmNodeQueryResults::isDir() const
{
	return false;
}

bool IdmNodeQueryResults::isFile() const
{
	return false;
}

bool IdmNodeQueryResults::isLink() const
{
	return false;
}

bool IdmNodeQueryResults::exists() const
{
	return true;
}

IdmNodeQueryResults::size_type IdmNodeQueryResults::fileSize() const
{
	return 0;
}

QString IdmNodeQueryResults::fileName() const
{
	return m_info.fileName();
}

QString IdmNodeQueryResults::absolutePath() const
{
	return m_info.absolutePath();
}

QString IdmNodeQueryResults::absoluteFilePath() const
{
	return m_info.absoluteFilePath();
}

QString IdmNodeQueryResults::absoluteFilePath(const QString &fileName) const
{
	return m_info.absoluteFilePath(fileName);
}

QDateTime IdmNodeQueryResults::lastModified() const
{
	return m_info.lastModified();
}

int IdmNodeQueryResults::permissions() const
{
	return 0;
}

void IdmNodeQueryResults::refresh()
{

}

IFileInfo *IdmNodeQueryResults::info(const QModelIndex &idx) const
{
	if (static_cast<QueryResultItem *>(idx.internalPointer())->isValue())
		return &static_cast<QueryResultPathValueItem *>(idx.internalPointer())->info();
	else
		return 0;
}

ICopyControl *IdmNodeQueryResults::createControl(INodeView *view) const
{
	QModelIndex index = view->currentIndex();

	if (static_cast<QueryResultItem *>(index.internalPointer())->isProperty())
	{
		QueryResultPropertyItem *item = static_cast<QueryResultPropertyItem *>(index.internalPointer());

		if (item->property().entity->type() == Database::Path)
			if (item->size() == 0)
				return new IdmQueryResultsCopyControl(
						m_container,
						item->rootValue(),
						item->property(),
						const_cast<IdmNodeQueryResults*>(this),
						index,
						m_info);
			else
			{
				QString destination;
				Tools::DestinationFromPathList tree;

				for (QueryResultPropertyItem::size_type i = 0, size = item->size(); i < size; ++i)
					tree.add(static_cast<QueryResultValueItem*>(item->at(i))->value()->value().toString());

				if (!(destination = tree.choose(tr("Choose a directory"), Application::mainWindow())).isEmpty())
					return new IdmQueryResultsCopyControl(
							m_container,
							item->rootValue(),
							item->property(),
							const_cast<IdmNodeQueryResults*>(this),
							index,
							destination);
			}
	}

	return 0;
}

void IdmNodeQueryResults::contextMenu(const QModelIndexList &list, INodeView *view)
{

}

void IdmNodeQueryResults::createFile(const QModelIndex &index, INodeView *view)
{
	if (static_cast<QueryResultItem *>(index.internalPointer())->isProperty())
	{
		QueryResultPropertyItem *item = static_cast<QueryResultPropertyItem *>(index.internalPointer());

		if (m_container.transaction())
		{
			bool declined = false;
			IdmEntityValue::Holder value(
					CreationTools::chooseOrCreateValue(
					Application::mainWindow(),
					m_container,
					item->property().entity,
					declined));

			if (value)
				if (m_container.addValue(item->rootValue(), value))
					if (m_container.commit())
					{
						beginInsertRows(index, item->size(), item->size());
						item->add(value);
						endInsertRows();
					}
					else
					{
						m_container.rollback();
						QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
					}
				else
				{
					m_container.rollback();
					QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
				}
			else
			{
				m_container.rollback();

				if (!declined)
					QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
			}
		}
		else
			QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
	}
}

void IdmNodeQueryResults::createDirectory(const QModelIndex &index, INodeView *view)
{

}

void IdmNodeQueryResults::rename(const QModelIndex &index, INodeView *view)
{
	view->edit(index);
}

void IdmNodeQueryResults::rename(const QModelIndexList &list, INodeView *view)
{

}

void IdmNodeQueryResults::remove(const QModelIndexList &list, INodeView *view)
{
	if (m_container.transaction())
	{
		QModelIndex index;
		TasksItemList files;
		QueryResultValueItem *item;
		QueryResultPropertyItem *property;
		QueryResultPropertyItem::size_type idx;

		for (QModelIndexList::size_type i = 0, size = list.size(); i < size; ++i)
			if (static_cast<QueryResultItem *>((index = list.at(i)).internalPointer())->isValue())
			{
				item = static_cast<QueryResultValueItem *>(index.internalPointer());

				if (!item->isLocked())
					if (item->value()->entity()->type() == Database::Path)
						files.push_back(item);
					else
					{
						property = static_cast<QueryResultPropertyItem*>(item->parent());

						if (m_container.removeValue(property->rootValue(), item->value()))
						{
							idx = property->indexOf(item);

							beginRemoveRows(FileSystemModel::parent(index), idx, idx);
							property->remove(idx);
							endRemoveRows();
						}
						else
						{
							QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
							m_container.rollback();
							return;
						}
					}
			}

//		if (m_container.commit())
//		{
//			lock(files, tr("Scanning for remove..."));
//			addTask(new ScanFilesTask(this, files), files);
//		}
//		else
		{
			QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
			m_container.rollback();
		}
	}
	else
		QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
}

void IdmNodeQueryResults::cancel(const QModelIndexList &list, INodeView *view)
{

}

void IdmNodeQueryResults::calculateSize(const QModelIndexList &list, INodeView *view)
{

}

void IdmNodeQueryResults::pathToClipboard(const QModelIndexList &list, INodeView *view)
{

}

void IdmNodeQueryResults::copy(const INodeView *source, INodeView *destination)
{

}

void IdmNodeQueryResults::move(const INodeView *source, INodeView *destination)
{

}

QAbstractItemModel *IdmNodeQueryResults::model() const
{
	return const_cast<IdmNodeQueryResults *>(this);
}

QAbstractItemDelegate *IdmNodeQueryResults::delegate() const
{
	return const_cast<IdmQueryResultsDelegate *>(&m_delegate);
}

const INodeView::MenuActionList &IdmNodeQueryResults::actions() const
{
	return m_actions;
}

::History::Entry *IdmNodeQueryResults::menuAction(QAction *action, INodeView *view)
{
	return NULL;
}

QModelIndex IdmNodeQueryResults::rootIndex() const
{
	return QModelIndex();
}

Node *IdmNodeQueryResults::viewChild(const QModelIndex &idx, PluginsManager *plugins, QModelIndex &selected)
{
	QueryResultItem *item = static_cast<QueryResultItem *>(idx.internalPointer());

	if (!item->isLocked() &&
		item->isValue() &&
		static_cast<QueryResultValueItem *>(item)->value()->entity()->type() == Database::Path)

		if (Node *node = static_cast<QueryResultPathValueItem *>(item)->node())
			return node;
		else
		{
			Info info(static_cast<QueryResultValueItem *>(item)->value()->value().toString(), true);

			if (info.exists())
			{
				node = new IdmFolderNode(m_container, info, this);
				static_cast<QueryResultPathValueItem *>(item)->setNode(node);
				return node;
			}
		}

	return NULL;
}

Node *IdmNodeQueryResults::viewChild(const QString &fileName, PluginsManager *plugins, QModelIndex &selected)
{
	return NULL;
}

void IdmNodeQueryResults::updateProgressEvent(const FileSystemItem *item, quint64 progress, quint64 timeElapsed)
{

}

void IdmNodeQueryResults::completedProgressEvent(const FileSystemItem *item, quint64 timeElapsed)
{

}

void IdmNodeQueryResults::performActionEvent(const AsyncFileAction::FilesList &files)
{

}

void IdmNodeQueryResults::add(const QModelIndex &index, const IdmCompositeEntityValue::List &values)
{
	if (static_cast<QueryResultItem *>(index.internalPointer())->isProperty())
	{
		QueryResultPropertyItem *item = static_cast<QueryResultPropertyItem *>(index.internalPointer());

		beginInsertRows(index, item->size(), item->size() + values.size() - 1);
		item->add(values);
		endInsertRows();
	}
}

void IdmNodeQueryResults::remove(const QModelIndex &index, const IdmCompositeEntityValue::List &values)
{

}

void IdmNodeQueryResults::process(const QModelIndexList &list, const Functor &functor)
{
	QueryResultItem *item;

	for (QModelIndexList::size_type i = 0, size = list.size(); i < size; ++i)
		if ((item = static_cast<QueryResultItem *>(list.at(i).internalPointer()))->isValue())
			functor(list.at(i), static_cast<QueryResultValueItem *>(item));
}

void IdmNodeQueryResults::processRemove(const QModelIndexList &list, const Functor &functor)
{
	QueryResultItem *item;

	for (QModelIndexList::size_type i = 0, size = list.size(); i < size; ++i)
		if ((item = static_cast<QueryResultItem *>(list.at(i).internalPointer()))->isValue())
			functor(list.at(i), static_cast<QueryResultValueItem *>(item));
}

void IdmNodeQueryResults::doRemove(INodeView *view, const QModelIndex &index, QueryResultValueItem *value)
{
	if (m_container.transaction())
	{
		QueryResultPropertyItem *property = static_cast<QueryResultPropertyItem*>(value->parent());

		if (m_container.removeValue(property->rootValue(), value->value()))
		{
			beginRemoveRows(FileSystemModel::parent(index), index.row(), index.row());

			if (m_container.commit())
				property->remove(index.row());
			else
			{
				m_container.rollback();
				QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
			}

			endRemoveRows();
		}
		else
		{
			m_container.rollback();
			QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
		}
	}
	else
	{
		m_container.rollback();
		QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
	}
}

void IdmNodeQueryResults::scanForRemove(const BaseTask::Event *e)
{
	typedef const ScanFilesTask::Event *Event;
	Event event = static_cast<Event>(e);
	Snapshot::List list(event->snapshot);

	if (!event->canceled)
	{
		QStringList folders;
		QStringList files;
		InfoItem *entry;

		for (Snapshot::List::size_type i = 0; i < list.size(); ++i)
			if ((entry = list.at(i).second)->isDir())
				folders.push_back(entry->fileName());
			else
				files.push_back(entry->fileName());

		if (QMessageBox::question(
				Application::mainWindow(),
				tr("Remove..."),
				tr("Would you like to remove").
					append(QString::fromLatin1("\n\t")).
					append(tr("directories:")).append(QString::fromLatin1("\n\t\t")).
					append(folders.join(QString::fromLatin1("\n\t\t"))).
					append(QString::fromLatin1("\n\t")).
					append(tr("files:")).append(QString::fromLatin1("\n\t\t")).
					append(files.join(QString::fromLatin1("\n\t\t"))).
					append(QString::fromLatin1("\n")).
					append(tr("it will free ").append(Tools::humanReadableSize(event->snapshot.totalSize()))),
				QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		{
			lock(list, tr("Removing..."));
//			resetTask(new RemoveFilesTask(this, event->files), event->task);
			return;
		}
	}

	unlock(list);
	removeAllTaskLinks(event->task);
}

void IdmNodeQueryResults::performRemove(const BaseTask::Event *e)
{
	typedef const RemoveFilesTask::Event *Event;
	QueryResultPropertyItem::size_type idx;
	Event event = static_cast<Event>(e);
	Snapshot::List list(event->snapshot);
	QueryResultPropertyItem *property;
	QModelIndex modelIdx;

	if (m_container.transaction())
	{
		bool ok = true;

		for (Snapshot::List::size_type i = 0, size = list.size(); i < size; ++i)
			if (list.at(i).second->isRemoved())
			{
				property = static_cast<QueryResultPropertyItem*>(list.at(i).first->parent());

				if (m_container.removeValue(property->rootValue(), static_cast<QueryResultValueItem*>(list.at(i).first)->value()))
				{
					idx = property->indexOf(list.at(i).first);

					beginRemoveRows(index(property), idx, idx);
					property->remove(idx);
					endRemoveRows();
				}
				else
				{
					QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
					m_container.rollback();
					ok = false;
					break;
				}
			}

		if (ok)
			if (!m_container.commit())
			{
				QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());
				m_container.rollback();
			}
	}
	else
		QMessageBox::critical(Application::mainWindow(), tr("Error"), m_container.lastError());

	for (Snapshot::List::size_type i = 0, size = list.size(); i < size; ++i)
		if (!list.at(i).second->isRemoved())
		{
			static_cast<QueryResultValueItem*>(list.at(i).first)->unlock();
			modelIdx = index(list.at(i).first);
			emit dataChanged(modelIdx, modelIdx);
		}

	removeAllTaskLinks(event->task);
}

void IdmNodeQueryResults::lock(const TasksItemList &list, const QString &reason)
{
	typedef QMap<QueryResultListItem*, Union> Map;
	qint32 lastColumn = columnCount(QModelIndex()) - 1;
	QueryResultPropertyItem *property;
	Map map;

	for (TasksItemList::size_type i = 0, size = list.size(); i < size; ++i)
	{
		property = static_cast<QueryResultPropertyItem*>(list.at(i)->parent());
		map[property].add(property->indexOf(list.at(i)));
		static_cast<QueryResultValueItem*>(list.at(i))->lock(reason);
	}

	for (Map::const_iterator i = map.constBegin(), end = map.constEnd(); i != end; ++i)
		for (Union::List::size_type q = 0, size = (*i).size(); q < size; ++q)
			emit dataChanged(createIndex((*i).at(q).top(), 0, i.key()->at((*i).at(q).top())),
							 createIndex((*i).at(q).bottom(), lastColumn, i.key()->at((*i).at(q).bottom())));
}

void IdmNodeQueryResults::lock(const Snapshot::List &list, const QString &reason)
{
	typedef QMap<QueryResultListItem*, Union> Map;
	qint32 lastColumn = columnCount(QModelIndex()) - 1;
	QueryResultPropertyItem *property;
	Map map;

	for (TasksItemList::size_type i = 0, size = list.size(); i < size; ++i)
	{
		property = static_cast<QueryResultPropertyItem*>(list.at(i).first->parent());
		map[property].add(property->indexOf(list.at(i).first));
		static_cast<QueryResultValueItem*>(list.at(i).first)->lock(reason);
	}

	for (Map::const_iterator i = map.constBegin(), end = map.constEnd(); i != end; ++i)
		for (Union::List::size_type q = 0, size = (*i).size(); q < size; ++q)
			emit dataChanged(createIndex((*i).at(q).top(), 0, i.key()->at((*i).at(q).top())),
							 createIndex((*i).at(q).bottom(), lastColumn, i.key()->at((*i).at(q).bottom())));
}

void IdmNodeQueryResults::unlock(const Snapshot::List &list)
{
	typedef QMap<QueryResultListItem*, Union> Map;
	qint32 lastColumn = columnCount(QModelIndex()) - 1;
	QueryResultPropertyItem *property;
	Map map;

	for (TasksItemList::size_type i = 0, size = list.size(); i < size; ++i)
	{
		property = static_cast<QueryResultPropertyItem*>(list.at(i).first->parent());
		map[property].add(property->indexOf(list.at(i).first));
		static_cast<QueryResultValueItem*>(list.at(i).first)->unlock();
	}

	for (Map::const_iterator i = map.constBegin(), end = map.constEnd(); i != end; ++i)
		for (Union::List::size_type q = 0, size = (*i).size(); q < size; ++q)
			emit dataChanged(createIndex((*i).at(q).top(), 0, i.key()->at((*i).at(q).top())),
							 createIndex((*i).at(q).bottom(), lastColumn, i.key()->at((*i).at(q).bottom())));
}

IdmNodeQueryResults::ItemsContainer::ItemsContainer() :
	ModelContainer()
{}

IdmNodeQueryResults::ItemsContainer::~ItemsContainer()
{
	qDeleteAll(m_container);
}

IdmNodeQueryResults::ItemsContainer::size_type IdmNodeQueryResults::ItemsContainer::size() const
{
	return m_container.size();
}

IdmNodeQueryResults::ItemsContainer::Item *IdmNodeQueryResults::ItemsContainer::at(size_type index) const
{
	return m_container.at(index);
}

IdmNodeQueryResults::ItemsContainer::size_type IdmNodeQueryResults::ItemsContainer::indexOf(Item *item) const
{
	return m_container.indexOf(item);
}

IDM_PLUGIN_NS_END
