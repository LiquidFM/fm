#include "settingsdecorationtext.h"

#include <QtGui/QLabel>


SETTINGS_NS_BEGIN

DecorationText::DecorationText(const QString &text) :
	Decoration(),
	m_text(text)
{}

QWidget *DecorationText::createEditor()
{
	return new QLabel(m_text);
}

SETTINGS_NS_END
