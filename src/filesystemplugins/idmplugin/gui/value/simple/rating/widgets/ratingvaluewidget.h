#ifndef RATINGVALUEWIDGET_H_
#define RATINGVALUEWIDGET_H_

#include <QtGui/QRadioButton>
#include "../../../../tools/nestedplaindialog/widgets/nestedwidget.h"
#include "../../../../../storage/values/idmentityvalue.h"


using namespace FileSystem::Plugins::Idm;

class RatingValueWidgetPrivate : public QWidget
{
	Q_OBJECT

public:
	RatingValueWidgetPrivate();

    int value() const { return m_result; }

private Q_SLOTS:
	void button1Click(bool checked);
	void button2Click(bool checked);
	void button3Click(bool checked);
	void button4Click(bool checked);
	void button5Click(bool checked);

private:
	int m_result;
    QRadioButton m_button1;
    QRadioButton m_button2;
    QRadioButton m_button3;
    QRadioButton m_button4;
    QRadioButton m_button5;
	QVBoxLayout m_verticatLayout;
};


class MainRatingValueWidget : public BaseNestedWidget
{
public:
	MainRatingValueWidget(NestedDialog *parent);

	/* BaseNestedWidget */
	virtual QWidget *centralWidget();
	virtual void setReadOnly(bool value);
	virtual void setFocus();

    int value() const { return m_private.value(); }

private:
	RatingValueWidgetPrivate m_private;
};


class RatingValueWidget : public NestedWidget
{
public:
	RatingValueWidget(NestedDialog *parent, const QString &title);

	/* BaseNestedWidget */
	virtual void setFocus();

    int value() const { return m_private.value(); }

private:
	RatingValueWidgetPrivate m_private;
};

#endif /* RATINGVALUEWIDGET_H_ */
