/*!
 * \file NImageObjectMarkerWidget.h
 * \date 2016/12/07 15:21
 *
 * \author Wu Qiang
 * Contact: thin.wu@163.com
 *
 * \brief 
 *
 * TODO: NULL
 *
 * \note
*/
#pragma once

#include "NImageDisplayWidget.h"
#include <opencv2/opencv.hpp>
#include "NCommon.h"



class NImageObjectMarkerWidget : public NImageDisplayWidget
{
	Q_OBJECT

public:

	enum EAnnotationMode
	{
		Undefined,
		Drawing,
		Move,
		Resize
	};
	NImageObjectMarkerWidget(QWidget *parent = NULL);
	~NImageObjectMarkerWidget();

public:
	void SetCroppingRectBorderColor(const QColor& color);
	void SetCroppingBackgroundColor(const QColor& color);
	void BeginCreateBBox();
	void EndCreateBBox();
	void SetCurrentRectIndex(int nRectIndex, bool bHidden);
	void DeleteRectIndex(int nRectIndex);
	void SetRectList(const QList<QRect>& listRect);
Q_SIGNALS:
	void postCroppingStatusChanged(bool bIsCropping);
	void postCroppedImage(const cv::Mat& matCropped);
	void postAddedRect(const QRect& rect, bool& bConfirm);
	void postRectChanged(int nRectIndex, const QRect& rect);

protected:
	virtual void paintEvent(QPaintEvent *event) override;

	virtual void mousePressEvent(QMouseEvent *event) override;

	virtual void mouseMoveEvent(QMouseEvent *event) override;

	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);

protected:
	void DrawRects(QPainter* painter, const cv::Rect* exceptRect);
	void DrawCroppingRect(QPainter* painter, const QRect& rectCropping, const QColor& colorCroppingRectBorder, const QColor& colorCroppingBackground);
	void DrawCroppingRect(QPainter* painter, const QRect& rectCropping);
	void DrawRect(QPainter* painter, const QRect& rect, const QPen& pen);

	void UpdateCursorIcon(const QPointF& _mousePosition);
	void UpdateCursorIcon(const QPointF& _mousePosition, const QRect& orignalRect);
	inline bool isPointNearSide(const int _sideCoordinate, const int _pointCoordinate)
	{
		static const int indent = 5;
		return qAbs(_sideCoordinate - _pointCoordinate) <= indent;
	}
	QRect RectifiedRect(const QRect& rect);
	QRect MapToOrignalImage(const QRect& rectTransformed);
	void RestrictRect(QRect& rect);
	bool RestrictRect(QRect* pRect);
	void RestrictResizingRect(ECursorPosition cursorPosition, QRect& rect);
	QPoint MapToOrignalImage(const QPoint& point);
	QPoint MatFromOrignalImage(const QPoint& point);
	QRect* RectAt(const QPoint& mousePos);
	int RectIndex(const QRect* pRect);
private:
	void SetupUI();
	void SetupConnect();
	ECursorPosition CursorPosition(const QRectF& _cropRect, const QPointF& _mousePosition);
	const QRectF CalculateGeometry(const QRectF& _sourceGeometry, const ECursorPosition _cursorPosition, const QPointF& _mouseDelta);
	bool IsPointInRect(const QPoint& pt, const QRect& rect);
private:
	
	bool m_bIsCropPressed;
	QPoint m_startCroppingPoint;
	QPoint m_startDrawingPoint;
	ECursorPosition m_eCursorPosition;
	QRect* m_pCurrentCroppingRect; // 矫正过的对应当前缩放和平移的
	QRect* m_pCurrentHoverRect;
	bool m_bIsDrawing;
	bool m_bMove;
	//
	QColor m_colorCroppingBackground;
	QColor m_colorCroppingRectBorder;
	QColor m_colorRectNormal;
	
	QPen m_penCroppingRect;
	QPen m_penHoverRect;
	QPen m_penNormalRect;


	QList<QRect*> m_listRect;
	QList<bool> m_listRectVisible;
	EAnnotationMode m_eAnnotationMode;
};
