#include "settingstab.h"


SETTINGS_NS_BEGIN

Tab::Tab(const QString &label, const QString &id, Option *parent) :
	Scope(id, parent)
{}

QWidget *Tab::createEditor()
{
	return NULL;
}

void Tab::restoreDefault()
{

}

bool Tab::accept()
{
	return false;
}

void Tab::reject()
{

}

SETTINGS_NS_END
