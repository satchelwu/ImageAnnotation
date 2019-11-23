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
/* ����˼·����ͼƬ���Ͻ�Ϊ��׼��ÿ�α任���ϸ������Ͻ�����, �Ŵ�������ԭͼΪ
��׼���жϵ�

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
	// �Ŵ����СͼƬ
	// fCurScale ��ǰ����
	// fNewScale Ҫ�Ŵ����С���ı���
	// pointBase �Ի�׼��Ŵ�
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


	// new Version; ��Ļ�����ͼ�������غϣ�����ֻ��Ҫ���ŵ�һ���ߴ磬Ȼ��������������Ļ�����ƶ�
	int m_nCurrentX;
	int m_nCurrentY;
	int m_nCurrentWidth;
	int m_nCurrentHeight;
	float m_fCurrentScale;
	QPoint m_pointCurrentWidget; // ��ǰ�ռ�����
	int m_nBorderSize;    // ��ֹ�ƶ�ͼƬ��ʱ���Ƴ�����
	QRect m_RectMask;  // ��Mask��������ʾͼƬ
	float m_fPreviousScale;
	float m_fMaxScale;
	float m_fMinScale;
	QString m_strFilePath;
	QVector<QRgb> m_vecColorTable;
	QPoint m_pointStartMoving;

	QPoint m_pointMouseOnPixel;
	// ������һ�εı���
	QPoint m_pointPreviousBase;

	bool m_bInfoTextEnabled;
private:
	friend NImageThumbnailWidget;
	NImageThumbnailWidget* m_imageThumbnailWidget;
};
 // NIMAGEDISPLAYWIDGET_H
