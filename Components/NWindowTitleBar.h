#pragma once

#include <QWidget>
#include "ui_NWindowTitleBar.h"

class QPushButton;
class NWindowTitleBar : public QWidget
{
	Q_OBJECT

public:
	NWindowTitleBar(QWidget *parent = Q_NULLPTR);
	~NWindowTitleBar();

	void SetDialogMode(bool bDialogMode = false);
	void SetTitle(const QString& strTitle, int nFontSize = 21, Qt::Alignment alighment = Qt::AlignHCenter | Qt::AlignVCenter);
	void SetIcon(const QIcon& icon);
	void SetWindowMaxState(bool bMaxState = true);
	void SetMaximizeStateEnabled(bool bEnabled = true);
	void SetDropDownMenuEnabled(bool bEnabled = true);
	 QPushButton* MinimizeButton();
	 QPushButton* MaximizeButton();
	 QPushButton* CloseButton();
	 QPushButton* DropdownButton();
	 QWidget*  TitleIconWidget();
	 QWidget*  TitleLabelWidget();
public:
Q_SIGNALS :
	void postMove(const QPoint& vec);
		  void postClose();
		  void postMinimize();
		  void postDropDownMenuPos(const QPoint& globalPos);
		  void postMaxmize();
		  void postDoubleClicked();
protected:
	virtual bool event(QEvent* event);
	virtual void paintEvent(QPaintEvent* event);
	virtual bool eventFilter(QObject *watched, QEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
private:
	void SetupUI();
	void SetupConnect();
private:
	Ui::NWindowTitleBar ui;
	QPoint m_pointPressed;
	bool m_bCanMove;
	QIcon m_iconMaximizeState;
	QIcon m_iconNormalState;
	bool m_bMaxmizeStateEnabled;
	bool m_bDropDownMenuEnabled;
};
