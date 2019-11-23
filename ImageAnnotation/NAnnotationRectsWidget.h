#pragma once

#include <QWidget>
#include "NCommon.h"
#include <QIcon>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QMenu;
class QAction;
class QCheckBox;
class QLineEdit;
class NAnnotationRectsWidget : public QWidget
{
	Q_OBJECT

public:
	NAnnotationRectsWidget(QWidget *parent);
	~NAnnotationRectsWidget();
	void AddAnnotationRect(const SAnnotationRect& annotationRect);
	void SetAnnotationRects(QList<SAnnotationRect>* pListAnnotationRect);
	void SetTitle(const QString& strTitle);
	void SetCurrentRectIndex(int nRowInex, const QRect& rect);
	bool UseDefaultClassLabel();
	QString DefaultClassLabel();

public:
Q_SIGNALS :
	void postCurrentIndex(int nRectIndex, bool bHidden);
		  void postDeleteRectIndex(int nRectIndex);
protected:
	//virtual bool eventFilter(QObject *watched, QEvent *event);
	private Q_SLOTS :
		void actionAnnotationRectItemClicked(QListWidgetItem* item);
	void actionContextMenuRequesed(const QPoint& pos);
	void actionDeleteRect();
private:
	void SetupUI();
	void SetupConnect();

	void UpdateRow(int nRowIndex, const QRect& rect);
private:
	QLabel* m_pLabelTitle;
	QCheckBox* m_pCheckBoxUseDefaultClassLabel;
	QLineEdit* m_pLineEditClassLabel;
	QListWidget* m_pCheckboxRectListWidget;
	QIcon m_iconChecked;
	QIcon m_iconUnchecked;
	QMenu* m_pContextMenu;
	QAction* m_pActionDelete;
	QList<SAnnotationRect>* m_pListAnnotationRect;

};
