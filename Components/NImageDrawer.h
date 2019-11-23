/*!
 * \file NImageDrawer.h
 * \date 2016/12/07 15:22
 *
 * \author Wu Qiang
 * Contact: thin.wu@163.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/
#pragma once

#include "NImageDisplayWidget.h"
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QPainter>
#include <opencv2/opencv.hpp>

class NImageDrawer : public NImageDisplayWidget
{
	Q_OBJECT

public:
	NImageDrawer(QWidget *parent = NULL);
	~NImageDrawer();
public:
	void OpenFile(QString strFilePath);
	void SetCanDrawRectCount(int nRectCount = 65526);
	//int SetImage(cv::Mat matSrc);
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void wheelEvent(QWheelEvent *event);
	//virtual void contextMenuEvent(QContextMenuEvent *event);
	//virtual void keyPressEvent(QKeyEvent *event);
	//virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void dropEvent(QDropEvent *event);
	private slots:
	void actionCanDrawCheckChanged(bool bCheck);
	void actionSaveImage();
	void actionResetImage();
	void actionIncreaseLineSize();
	void actionDecreaseLineSize();
protected:
	void DrawRects(QPainter* painter);
	QRectF RectifiedRect(const cv::Rect& rect);
	cv::Rect MapToOrignalRect(const QRectF& rect);
private:
	void RoomRect(cv::Rect& rect, float fCurrentScale, float fNewScale, QPoint pointBase);
	void SetupUI();
	void SetupConnect();
protected:
	QAction* m_pActionEnableDrawing;
	QAction* m_pActionPreviewDrawedImage;
	QAction* m_pActionResetImage;
	QAction* m_pActionSaveImage;
	QAction* m_pActionIncreaseLineSize;
	QAction* m_pActionDecreaseLineSize;

	QMenu* m_pContextMenu;
	QPoint m_pointStartPoint;
	QList<cv::Rect> m_vecRect; // 保存的是对应于原图大小尺寸的相对坐标
	int m_nLineSize;
	QRect m_CurrentDrawingRect; // 对应于当前缩放和平移的框

private:
	bool m_bCanDraw;
	bool m_bIsTriggerOn;
	int m_nCanDrawRectCount;
};

// NIMAGEDRAWER_H
