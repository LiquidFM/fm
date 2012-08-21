#ifndef DIRECTORYVIEW_H
#define DIRECTORYVIEW_H

#include <QtCore/QList>
#include <QtGui/QWidget>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtXml/QXmlStreamWriter>
#include <QtXml/QXmlStreamReader>

#include <vfs/interfaces/vfs_inode.h>

#include "widgets/pathedit.h"
#include "../../history/navigationhistory.h"
#include "../../tools/events/qt/imp/mouseeventhandler.h"
#include "../../tools/events/qt/imp/keyboardeventhandler.h"
#include "../../tools/events/qt/imp/contextmenueventhandler.h"
#include "../../tools/events/qt/imp/mouseeventsource.h"
#include "../../tools/events/qt/imp/keyboardeventsource.h"
#include "../../tools/events/qt/imp/contextmenueventsource.h"
#include "../../tools/events/qt/imp/focuseventhandler.h"


using namespace Tools::Events;
class FoldersView;

class DirectoryView : public QWidget, public ::VFS::INodeView
{
    Q_OBJECT

public:
    typedef ::VFS::INode::Geometry Geometry;

    struct Tab
    {
    	struct Sort
    	{
    		qint32 column;
    		Qt::SortOrder order;
    	};

    	QString path;
    	Sort sort;
    	Geometry geometry;
    };

public:
    DirectoryView(FoldersView *parent);
    virtual ~DirectoryView();

    /* Should be called immediately after construction and adding to the tabWidget! */
	void setupModel(const QString &absoluteFilePath);
	void setupModel(::VFS::INode *node, const QModelIndex &index, const Geometry &geometry);
	void setupModel(const QString &path, qint32 column, Qt::SortOrder order, const Geometry &geometry);

public:
	/* INodeView */
	virtual ::VFS::INode *node() const;
	virtual QModelIndex currentIndex() const;
	virtual QModelIndexList selectedIndexes() const;
	virtual void edit(const QModelIndex &index);
	virtual void select(const QModelIndex &index);
	virtual void setNode(::VFS::INode *node);

	static QString defaultPath();

    bool hasFocus() const;
	QPoint listPos() const;

	Tab tab() const;

	void setFocus();
	void setCurrentDirectory(const QString &filePath);

public Q_SLOTS:
	void goUp();
    void goBackward();
    void goForward();
    void refresh();
    void activated();
	void pathToClipboard();
	void rename();
	void createFile();
	void createDirectory();
	void remove();
	void calculateSize();
	void copy();
	void move();
	void cancel();
	void search();

private Q_SLOTS:
	void actionTriggered(QAction *action);

private:
	void openInNewTab();
	void closeTab();
    void editPath();
    void contextMenu();

private:
    friend class FileSystemModelBase;
    void refreshOther();
    QList<qint32> geometry() const;

private:
	typedef MouseReleaseEventHandler<
				FocusOutEventHandler<
					KeyboardEventHandler<
						EventHandlerBase<
							DirectoryView
						>
					>
				>
			> PathEventHandler;

private:
	struct Header
	{
		Header(PathEventHandler *eventHandler, QWidget *parent);

		QHBoxLayout layout;
		PathEdit pathEdit;
		bool pathEditFlag;
		QToolButton favorites;
		QToolButton history;
	};
private Q_SLOTS:
	void actHeaderPathAccept();
	void actHeaderPathReject();
	void actHeaderFavorites();
	void actHeaderHistory();

private:
	typedef MouseDoubleClickEventSource<
				ContextMenuEventSource<
					KeyboardEventSource<
						EventSourceBase<
							QTreeView
						>
					>
				>
			> DirectoryListView;
	typedef MouseDoubleClickEventHandler<
				ContextMenuEventHandler<
					KeyboardEventHandler<
						EventHandlerBase<
							DirectoryView
						>
					>
				>
			> DirectoryListViewEventHandler;

private:
	FoldersView *m_parent;
    ::VFS::INode *m_node;
    QVBoxLayout m_layout;
    PathEventHandler m_pathEventHandler;
    Header m_header;
    QToolBar m_toolBar;
    DirectoryListView m_view;
    DirectoryListViewEventHandler m_eventHandler;
    History::Navigation m_navigation;
};

#endif // DIRECTORYVIEW_H
