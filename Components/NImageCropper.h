/*!
 * \file NImageCropper.h
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

class NImageCropper : public NImageDisplayWidget
{
	Q_OBJECT

public:
	NImageCropper(QWidget *parent = NULL);
	~NImageCropper();

public:
	void SetCroppingRectBorderColor(const QColor& color);
	void SetCroppingBackgroundColor(const QColor& color);
Q_SIGNALS:
	void postCroppingStatusChanged(bool bIsCropping);
	void postCroppedImage(const cv::Mat& matCropped);
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

	void UpdateCursorIcon(const QPointF& _mousePosition);
	inline bool isPointNearSide(const int _sideCoordinate, const int _pointCoordinate)
	{
		static const int indent = 7;
		return qAbs(_sideCoordinate - _pointCoordinate) <= 7;
	}
	QRect RectifiedRect(const QRect& rect);
	QRect MapToOrignalImage(const QRect& rectTransformed);
	void RestrictRect(QRect& rect);
private:
	void SetupUI();
	void SetupConnect();
	ECursorPosition CursorPosition(const QRectF& _cropRect, const QPointF& _mousePosition);
	const QRectF CalculateGeometry(const QRectF& _sourceGeometry, const ECursorPosition _cursorPosition, const QPointF& _mouseDelta);
	bool IsPointInRect(const QPoint& pt, const QRect& rect);
private:
	
	bool m_bIsCropPressed;
	QPoint m_startCroppingPoint;
	ECursorPosition m_eCursorPosition;
	QRect m_curCroppingRect; // 矫正过的对应当前缩放和平移的
	bool m_bIsDrawing;
	bool m_bMove;
	//
	QColor m_colorCroppingBackground;
	QColor m_colorCroppingRectBorder;
};
