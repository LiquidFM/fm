#ifndef IDMQUERYRESULTSCOPYCONTROL_H_
#define IDMQUERYRESULTSCOPYCONTROL_H_

#include <QtCore/QCoreApplication>
#include "../interface/idminodequeryresults.h"
#include "../../../control/idmcopycontrolbase.h"
#include "../../../containeres/idmcontainer.h"
#include "../../../storage/values/idmentityvalue.h"


IDM_PLUGIN_NS_BEGIN

class IdmQueryResultsCopyControl : public IdmCopyControlBase
{
	Q_DECLARE_TR_FUNCTIONS(IdmQueryResultsCopyControl)

public:
	IdmQueryResultsCopyControl(ICopyControl::Holder &dest, const IdmContainer &container, const IdmEntityValue::Holder &value, const IdmEntity::Property &property, IQueryResultsUpdater *model, const QModelIndex &index);

	/* ICopyControl */
	virtual bool start(const Snapshot &files, bool move);
	virtual void done(bool error);
	virtual void canceled();

private:
	IdmEntityValue::Holder m_value;
	IdmEntity::Property m_property;
	IQueryResultsUpdater *m_model;
	QModelIndex m_index;
};

IDM_PLUGIN_NS_END

#endif /* IDMQUERYRESULTSCOPYCONTROL_H_ */
