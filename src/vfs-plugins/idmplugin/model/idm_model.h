#ifndef IDM_MODEL_H_
#define IDM_MODEL_H_

#include "items/idm_item.h"
#include "../../../tools/models/tree/treemodel.h"


IDM_PLUGIN_NS_BEGIN

class Model : public ::Tools::Models::Tree::Model
{
public:
	Model(QObject *parent = 0);

protected:
	class Container : public ::Tools::Models::Tree::Model::Container
	{
	public:
		typedef QList<IdmItem *> List;

	public:
		Container();
		virtual ~Container();

		virtual size_type size() const;
		virtual Item *at(size_type index) const;
		virtual size_type indexOf(Item *item) const;

	private:
		friend class Model;
		friend class RootNode;
		friend class QueryResultsNode;
		List m_container;
	};

protected:
	Container m_container;
	Container::List &m_items;
};

IDM_PLUGIN_NS_END

#endif /* IDM_MODEL_H_ */