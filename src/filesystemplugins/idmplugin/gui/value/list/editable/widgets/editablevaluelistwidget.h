#ifndef EDITABLEVALUELISTWIDGET_H_
#define EDITABLEVALUELISTWIDGET_H_

#include <QtGui/QTreeView>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtCore/QCoreApplication>
#include "../model/editablevaluelistmodel.h"
#include "../../../model/valuelistproxymodel.h"
#include "../../../../../containeres/idmcontainer.h"
#include "../../../../../storage/queries/idmselectquery.h"
#include "../../../../../../../tools/events/imp/keyboardeventhandler.h"
#include "../../../../../../../tools/events/imp/keyboardeventsource.h"
#include "../../../../../../../tools/widgets/nestedplaindialog/widgets/nestedwidget.h"


using namespace ::FileSystem::Plugins::Idm;
using namespace ::Tools::Events;

class EditableValueListWidgetPrivate : public QWidget
{
	Q_OBJECT

public:
	class ICallback
	{
	public:
		virtual ~ICallback();

		virtual void acceptAndClose() = 0;
		virtual NestedDialog *parent() = 0;
		virtual void critical(const QString &text) = 0;
	};

	typedef KeyboardEventSource<
				EventSourceBase<
					QTreeView
				>
			> TreeView;

	typedef KeyboardEventSource<
				EventSourceBase<
					QLineEdit
				>
			> LineEdit;

	typedef KeyboardEventHandler<
				EventHandlerBase<
					EditableValueListWidgetPrivate
				>
			> LineEditHandler;

public:
	EditableValueListWidgetPrivate(ICallback *callback, EventHandler *handler, const IdmContainer &container, const Select &query);

	const IdmContainer &container() const { return m_container; }
	IdmContainer &container() { return m_container; }

	const TreeView &view() const { return m_view; }
	TreeView &view() { return m_view; }

	const EditableValueListModel &model() const { return m_model; }
	EditableValueListModel &model() { return m_model; }

	const ValueListProxyModel &proxy() const { return m_proxy; }
	ValueListProxyModel &proxy() { return m_proxy; }

    QModelIndex currentIndex() const { return m_proxy.mapToSource(m_view.selectionModel()->currentIndex()); }

    void addValue();
	void removeValue();
//    void select(const QModelIndex &index);
	void setFocusToFilter() { m_filter.setFocus(); }

private Q_SLOTS:
	void setFilter();
	void clearFilter();
	void selectValue(const QModelIndex &index);

private:
    void setCurrentIndex(const QModelIndex &index) const;

private:
	ICallback *m_callback;
	IdmContainer m_container;
	IdmEntity *m_entity;

	QVBoxLayout m_vLayout;
	QHBoxLayout m_hLayout;

	LineEditHandler m_handler;
	LineEdit m_filter;
	QPushButton m_search;

	TreeView m_view;
	EditableValueListModel m_model;
	ValueListProxyModel m_proxy;
};


class MainEditableValueListWidget : public BaseNestedWidget, public EditableValueListWidgetPrivate::ICallback
{
public:
	MainEditableValueListWidget(EventHandler *handler, const IdmContainer &container, const Select &query, NestedDialog *parent);

	/* BaseNestedWidget */
	virtual QWidget *centralWidget();
	virtual void setReadOnly(bool value);
	virtual void setFocus();

	/* EditableValueListWidgetPrivate::ICallback */
	virtual void acceptAndClose();
	virtual NestedDialog *parent();
	virtual void critical(const QString &text);

    const IdmContainer &container() const { return m_private.container(); }
	IdmContainer &container() { return m_private.container(); }

    QModelIndex currentIndex() const { return m_private.currentIndex(); }
	IdmEntityValue::Holder takeValue() { return m_private.model().take(currentIndex()); }

	void closeDbContext() { m_private.model().close(); }

    void addValue() { m_private.addValue(); }
	void removeValue() { m_private.removeValue(); }
//    void select(const QModelIndex &index) { m_private.select(index); }
	void setFocusToFilter() { m_private.setFocusToFilter(); }
	void setViewToolTip(const QString &value) { m_private.view().setToolTip(value); }

private:
	EditableValueListWidgetPrivate m_private;
};


class EditableValueListWidget : public NestedWidget, public EditableValueListWidgetPrivate::ICallback
{
public:
	EditableValueListWidget(const IdmContainer &container, const Select &query, NestedDialog *parent);

	/* BaseNestedWidget */
	virtual void setFocus();

	/* EditableValueListWidgetPrivate::ICallback */
	virtual void acceptAndClose();
	virtual NestedDialog *parent();
	virtual void critical(const QString &text);

    const IdmContainer &container() const { return m_private.container(); }
	IdmContainer &container() { return m_private.container(); }

    QModelIndex currentIndex() const { return m_private.currentIndex(); }
	IdmEntityValue::Holder takeValue() { return m_private.model().take(currentIndex()); }

    void addValue();
	void removeValue();
//    void select(const QModelIndex &index) { m_private.select(index); }
	void setFocusToFilter();

private:
	typedef KeyboardEventHandler<
				EventHandlerBase<
					EditableValueListWidget
				>
			> TreeViewHandler;

private:
	TreeViewHandler m_handler;
	EditableValueListWidgetPrivate m_private;
};

#endif /* EDITABLEVALUELISTWIDGET_H_ */
