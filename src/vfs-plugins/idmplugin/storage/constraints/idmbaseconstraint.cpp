#include "idmbaseconstraint.h"


IDM_PLUGIN_NS_BEGIN

BaseConstraint::BaseConstraint(BaseConstraint *parent) :
	QSharedData(),
	m_parent(parent)
{}

BaseConstraint::~BaseConstraint()
{}

IDM_PLUGIN_NS_END