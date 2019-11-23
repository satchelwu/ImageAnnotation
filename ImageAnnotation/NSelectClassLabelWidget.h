#pragma once

#include "NDropShadowWidget.h"
#include <QStringList>
class QLineEdit;
class NWindowTitleBar;
class QPushButton;
class QListWidget;
class QCompleter;
class QStringListModel;
class QEventLoop;
class QToolButton;
class QListView;
class QCheckBox;

class NSelectClassLabelWidget : public NDropShadowWidget
{
	Q_OBJECT

public:
	NSelectClassLabelWidget(QWidget *parent = NULL);
	~NSelectClassLabelWidget();
	void SetLabels(const QStringList& strLabels);
	QDialog::DialogCode Exec();
	QString CurrentClassLabel();
	bool CurrentIsDifficult();
public:

protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);
	virtual void closeEvent(QCloseEvent *event);
	virtual void hideEvent(QHideEvent *event);
	virtual void showEvent(QShowEvent *event);
	private Q_SLOTS:
	void actionConfirm();
	void actionCancel();
	void actionCompleterActivated(const QModelIndex& index);
	void actionLabelDoubleClicked(const QModelIndex& index);
private:
	void SetupUI();
	void SetupConnect();
private:
	QLineEdit* m_pLineEditClassLabel;
	NWindowTitleBar* m_pWindowTitleBar;
	QCheckBox* m_pCheckBoxIsDifficult;
	QToolButton* m_pToolButtonConfirm;
	QToolButton* m_pToolButtonCancel;
	QListWidget* m_pListWidgetLabels;
	QListView* m_pListViewLables;
	//QStringList m_stringListLabels;
	QCompleter* m_pCompleterLabels;
	QToolButton* m_pToolButtonClear;
	QStringListModel* m_modelLabels;
	QDialog::DialogCode m_dialogCode;
	QEventLoop* m_pEventLoop;
	QString m_strClassLabel;
};
