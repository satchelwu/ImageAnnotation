/*!
 * \file nimagedisplaywidget.h
 * \date 2016/12/07 15:19
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

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QPen>
#include <vector>
/************************************************************************/
/* 整个思路：以图片左上角为基准，每次变换不断更新左上角坐标, 放大倍数是以原图为
基准来判断的

*/
/************************************************************************/
class NImageThumbnailWidget;

class NImageDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	NImageDisplayWidget(QWidget *parent = 0);
	~NImageDisplayWidget();
public:
	virtual int SetImage(const cv::Mat& matSrc);
	virtual int SetImage(const cv::Mat& matSrc, bool bFitView);
	virtual int SetImage(const cv::Mat& matSrc, const std::vector<std::vector<cv::Point> >& vecContours);
	virtual int SetImage(const cv::Mat& matSrc, const std::vector<std::vector<cv::Point> >& vecContours, bool bFitView);
	void SetInfoTextEnabled(bool bEnabled = true);
	void SetMaxMinScale(float fMaxScale, float fMinScale);
	bool OpenFile(QString strFilePath);
	const cv::Mat& GetImage();
	const std::vector<std::vector<cv::Point>>& GetContours();
	void Translate(const QPoint& point);
	void ResizeWindow(int nWidth, int nHeight);
	void Scale(float fScale, const QPoint& pointBase);
	void FitView();
	void OrignalView(QPoint pointBase);
	void SetShowDefects(bool bShowDefects);
	bool ShowDefects() { return m_bShowContours; };
	void SetThumbnailWidgetHidden(bool bHidden);
	void SetClipboardEnabled(bool bEnabled);

Q_SIGNALS:
	void postTranslatePoint(const QPoint& point);
	void postResize(int nWidth, int nHeight);
	void postScale(float fScale, const QPoint& pointBase);
	void postFitView();
	void postOrignalView(const QPoint& pointBase);
	void postShowDefects(bool bShowDefects);
	void postImageOpened(const cv::Mat& matImage);
	void postPixelPointOnImage(const cv::Point& pixelPoint);
	//void postBeginTranlatePoint(const QPoint& point);
	void postOpenedImagePath(const QString& strImagePath);
public Q_SLOTS :
	void actionTranslatePoint(const QPoint& point);
	void actionResize(int nWidth, int nHeight);
	void actionScale(float fScale, const QPoint& pointBase);
	void actionFitView();
	void actionOrignalView(const QPoint& pointBase);
	void actionShowDefects(bool bShowDefects);
	void actionImagePoint(const QPoint& pointInImage);
protected:
	virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
	virtual void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
	virtual void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
	virtual void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
	virtual void moveEvent(QMoveEvent *event) Q_DECL_OVERRIDE;

private:
	void SetupUI();
	// 放大或缩小图片
	// fCurScale 当前比例
	// fNewScale 要放大或缩小到的比例
	// pointBase 以基准点放大
protected:
	void Room(float fCurScale, float fNewScale, QPoint pointBase);
	QPoint WidgetMapToImage(const QPoint& widgetPos);
	QPoint ImageMapToWidget(const QPoint& imagePos);
protected:
	//////////////////////////////////////////////////////////////////////////
	//draw needed
	cv::Mat m_matSrc;
	cv::Mat m_matSrcRGB;
	std::vector<std::vector<cv::Point>> m_vecContours;
	bool m_bShowContours;
	bool m_bClipBoradEnabled;
	QPen m_penRect;
	bool m_bIsMoving;
	QBrush m_backBrush;


	// new Version; 屏幕坐标和图像坐标重合，这样只需要缩放到一定尺寸，然后将坐标中心向屏幕中心移动
	int m_nCurrentX;
	int m_nCurrentY;
	int m_nCurrentWidth;
	int m_nCurrentHeight;
	float m_fCurrentScale;
	QPoint m_pointCurrentWidget; // 当前空间坐标
	int m_nBorderSize;    // 防止移动图片的时候移出窗口
	QRect m_RectMask;  // 在Mask矩形中显示图片
	float m_fPreviousScale;
	float m_fMaxScale;
	float m_fMinScale;
	QString m_strFilePath;
	QVector<QRgb> m_vecColorTable;
	QPoint m_pointStartMoving;

	QPoint m_pointMouseOnPixel;
	// 返回上一次的比例
	QPoint m_pointPreviousBase;

	bool m_bInfoTextEnabled;
private:
	friend NImageThumbnailWidget;
	NImageThumbnailWidget* m_imageThumbnailWidget;
};
 // NIMAGEDISPLAYWIDGET_H
