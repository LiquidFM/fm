#ifndef IDMPROPERTIESTABLE_H_
#define IDMPROPERTIESTABLE_H_

#include "idmdatabasetypes.h"


IDM_PLUGIN_NS_BEGIN

struct PropertiesTable
{
	enum Fields
	{
		Id = 0,
		EntityId = 1,
		EntityPropertyId = 2,
		Name = 3
	};

	static QString tableName();

	static QByteArray create();
	static QByteArray create(Database::id_type entity, Database::id_type property);
	static QByteArray select();
	static QByteArray select(Database::id_type entity);
	static QByteArray select(Database::id_type entity, Database::id_type property);
	static QByteArray insert();
	static QByteArray insert(Database::id_type id, Database::id_type entity, Database::id_type property, const QString &name);
	static QByteArray insert(Database::id_type entity, Database::id_type property);
	static QByteArray rename(Database::id_type entity, Database::id_type property, const QString &name);
	static QByteArray remove(Database::id_type entity, Database::id_type property);
	static QByteArray addValue(const QString &tableName, Database::id_type id, Database::id_type entity, Database::id_type property);
	static QByteArray removeValue(Database::id_type entity, Database::id_type entityValue, Database::id_type property, Database::id_type propertyValue);
	static QByteArray selectValuesExcept(Database::id_type entity, const Database::IdsList &entityIds, Database::id_type property, const Database::IdsSet &propertyIds);

	struct Incomplete
	{
		static QString selectValues(Database::id_type property);
		static QString selectValues(Database::id_type entity, const Database::IdsList &ids);
		static QString dropProperty(Database::id_type property);
		static QString dropProperty2(Database::id_type property);
		static QByteArray addValue(const QString &tableName, Database::id_type entity);
		static QString removeValues(Database::id_type property, const Database::IdsList &ids);
	};

	struct Parameters
	{
		static QByteArray addValue(Database::id_type entity, Database::id_type property, Database::id_type value);
	};
};

IDM_PLUGIN_NS_END

#endif /* IDMPROPERTIESTABLE_H_ */