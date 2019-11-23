
#ifndef NDropShadowWidget_H
#define NDropShadowWidget_H

#include <QDialog>
#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>
#include <QColor>
#include "NCommon.h"


class NWindowTitleBar;
class NDropShadowWidget : public QWidget
{
	Q_OBJECT

public:

	explicit NDropShadowWidget(QWidget *parent = 0);
	~NDropShadowWidget();

public:
	void SetShadowWidth(int nWidth = 5);
	void setParent(QWidget* parent);
	void SetWindowTitleBar(NWindowTitleBar* pWindowTitleBar);
	void SetFixedSize(bool bFixedSize = false);
protected:
	bool eventFilter(QObject *watched, QEvent *event);
 	void mousePressEvent(QMouseEvent *event);
 	void mouseMoveEvent(QMouseEvent *event);
 	void mouseReleaseEvent(QMouseEvent *event);
	virtual void paintEvent(QPaintEvent *event);
	
private:
	void UpdateCursorIcon(const QPointF& _mousePosition);
	inline bool isPointNearSide(const int _sideCoordinate, const int _pointCoordinate)
	{
		static const int indent = 7;
		return qAbs(_sideCoordinate - _pointCoordinate) <= 7;
	}

	ECursorPosition CursorPosition(const QRectF& _cropRect, const QPointF& _mousePosition);
	const QRectF CalculateGeometry(const QRectF& _sourceGeometry, const ECursorPosition _cursorPosition, const QPointF& _mouseDelta);
	bool IsPointInRect(const QPoint& pt, const QRect& rect);
	void Resize(const QRect& rectGeometry, const ECursorPosition& cursorPosition, const QPoint& mouseGlobalPos);
private:
	int m_nShadowWidth;
	QPoint m_pointStartResize;
	bool m_bResizing;
	ECursorPosition m_eCursorPostion;
	NWindowTitleBar* m_pWindowWidgetBar;
	bool m_bFixedSize;
};

#endif //NDropShadowWidget_H