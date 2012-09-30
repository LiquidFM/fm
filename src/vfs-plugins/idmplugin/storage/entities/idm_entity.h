#ifndef IDM_ENTITY_H_
#define IDM_ENTITY_H_

#include <QtCore/QRect>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include "../format/idm_shortformat.h"
#include "../structure/idm_databasetypes.h"
#include "../../../../tools/containers/hashedlist.h"


IDM_PLUGIN_NS_BEGIN

class Entity
{
public:
	struct Property
	{
		Property() :
			entity(NULL)
		{}
		Property(Entity *entity, const QString &name) :
			entity(entity),
			name(name)
		{}

		bool operator==(const Property &other) const { return entity == other.entity; }

		Entity *entity;
		QString name;
	};
	typedef Database::EntityType                               Type;
	typedef Database::id_type                                  id_type;
	typedef ::Tools::Containers::HashedList<id_type, Property> Container;
	typedef Container::size_type                               size_type;
	typedef ::Tools::Containers::HashedList<id_type, Entity*>  Parents;
	enum { InvalidIndex = Container::InvalidIndex };
	enum { InvalidId = Database::InvalidId };

public:
	Entity(Type type, id_type id,
			const QString &name,
			const ShortFormat &shortFormat,
			const QByteArray &editorGeometry,
			const QByteArray &listGeometry);
	virtual ~Entity();

	static QByteArray geometryToByteArray(const QRect &geometry);
	static QRect geometryFromByteArray(const QByteArray &buffer);

	const Property &at(size_type index) const { return m_items.at(index); }
	size_type size() const { return m_items.size(); }
	size_type indexOf(id_type id) const { return m_items.indexOf(id); }
	size_type indexOf(Entity *entity) const { return m_items.indexOf(entity->id()); }
	size_type indexOf(const QString &name) const
	{
		for (size_type i = 0, size = m_items.size(); i < size; ++i)
			if (m_items.at(i).name.compare(name, Qt::CaseSensitive) == 0)
				return i;

		return InvalidIndex;
	}

	Type type() const { return m_type; }
	id_type id() const { return m_id; }
	const QString &name() const { return m_name; }
	const Parents &parents() const { return m_parents; }
	const ShortFormat &shortFormat() const { return m_shortFormat; }
	const QRect &editorGeometry() const { return m_editorGeometry; }
	const QRect &listGeometry() const { return m_listGeometry; }

protected:
	friend class Storage;
	friend class StorageUndoAddEntity;
	friend class StorageUndoAddProperty;
	friend class StorageUndoRemoveEntity;
	friend class StorageUndoRemoveProperty;
	friend class StorageUndoRenameProperty;

	void addParent(Entity *parent) { m_parents.add(parent->id(), parent); }
	void removeParent(Entity *parent) { m_parents.remove(parent->id()); }

	void add(Entity *item, const QString &name) { m_items.add(item->id(), Property(item, name)); }
	void rename(Entity *item, const QString &name) { m_items[m_items.indexOf(item->id())].name = name; }
	void remove(Entity *item) { m_items.remove(item->id()); }
	Entity *take(size_type index) { return m_items.take(index).entity; }

	void setEditorGeometry(const QRect &value) { m_editorGeometry = value; }
	void setListGeometry(const QRect &value) { m_listGeometry = value; }

protected:
	Container m_items;

private:
	Type m_type;
	id_type m_id;
	QString m_name;
	Parents m_parents;
	ShortFormat m_shortFormat;
	QRect m_editorGeometry;
	QRect m_listGeometry;
};

IDM_PLUGIN_NS_END

#endif /* IDM_ENTITY_H_ */
