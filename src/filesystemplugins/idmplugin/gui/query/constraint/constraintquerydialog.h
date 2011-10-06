#ifndef CONSTRAINTQUERYDIALOG_H_
#define CONSTRAINTQUERYDIALOG_H_

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDialogButtonBox>
#include "../../../containeres/idmcontainer.h"
#include "../../../storage/constraints/value/idmconstraint.h"


using namespace FileSystem::Plugins::Idm;

class ConstraintQueryDialog : public QDialog
{
	Q_OBJECT

public:
	ConstraintQueryDialog(const IdmContainer &container, const QString &name, IdmEntity *entity, QWidget *parent = 0);

	Constraint *takeConstraint();
    virtual void accept();

private Q_SLOTS:
	void chooseValue();

private:
	IdmContainer m_container;
	IdmEntity *m_entity;
	QLabel m_label;
	QComboBox m_operator;
	QLineEdit m_edit;
	QToolButton m_choose;
	QDialogButtonBox m_buttonBox;
	QVBoxLayout m_verticatLayout;
	QHBoxLayout m_horizontalLayout;
};

#endif /* CONSTRAINTQUERYDIALOG_H_ */
