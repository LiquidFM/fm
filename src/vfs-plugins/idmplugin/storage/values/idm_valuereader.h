#ifndef IDM_VALUEREADER_H_
#define IDM_VALUEREADER_H_

#include "../queries/idm_selectquery.h"
#include "../../containeres/idm_container.h"
#include "idm_entityvalue.h"


IDM_PLUGIN_NS_BEGIN

class ValueReader
{
public:
	ValueReader(const IdmContainer &container, const Select &query);

	bool isValid() const { return m_context.isValid(); }
	Entity *entity() const { return m_context.entity(); }
	const QString &lastError() const { return m_lastError; }

	EntityValue::Holder next() const;
	bool eof() const { return m_afterLast; }
	bool bof() const { return m_beforeFirst; }
	void close() { m_afterLast = true; m_context = QueryContext(); }

	static void addValue(const EntityValue::Holder &value, const EntityValue::Holder &property);
	static void addValue(const EntityValue::Holder &value, const CompositeEntityValue::List &values);
	static void takeValue(const EntityValue::Holder &value, const EntityValue::Holder &property);
	static void updateValue(const EntityValue::Holder &value, const QVariant &newValue);
	static void removeValue(const EntityValue::Holder &value, const EntityValue::Holder &property);
	static void removeValue(const EntityValue::Holder &value, const CompositeEntityValue::List &values);
	static EntityValue::Holder createValue(Entity *entity, EntityValue::id_type id);
	static EntityValue::Holder createValue(Entity *entity, EntityValue::id_type id, const QVariant &value);

private:
	EntityValue::Holder doNext() const;
	EntityValue::Holder value(Entity *entity, Entity::id_type id, int column) const;
	void property(const EntityValue::Holder &value, Entity *property, int &column) const;
	void skip(Entity *property, int &column) const;

private:
	QString m_lastError;
	QueryContext m_context;
	mutable bool m_afterLast;
	mutable bool m_beforeFirst;
};

IDM_PLUGIN_NS_END

#endif /* IDM_VALUEREADER_H_ */
