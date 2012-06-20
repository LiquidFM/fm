#ifndef SETTINGSIVISIBLEPPTION_H_
#define SETTINGSIVISIBLEPPTION_H_

#include "../../settings.h"


class QLayout;


SETTINGS_NS_BEGIN

class IVisibleOption
{
public:
	virtual ~IVisibleOption();

	virtual QLayout *createEditor() = 0;
	virtual bool accept() = 0;
	virtual void reject() = 0;
};

SETTINGS_NS_END

#endif /* SETTINGSIVISIBLEPPTION_H_ */
