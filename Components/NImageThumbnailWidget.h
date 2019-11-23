#pragma once

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <vector>
#include <QPen>

class NImageThumbnailWidget : public QWidget
{
	Q_OBJECT

public:
	NImageThumbnailWidget(QWidget *parent);
	~NImageThumbnailWidget();

	 int SetImage(const cv::Mat& matSrc, const std::vector<std::vector<cv::Point> >& vecContours = std::vector<std::vector<cv::Point>>());

 Q_SIGNALS:
	 void postImagePoint(const QPoint& pointInImage);

protected:
	virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;


	virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

	virtual void keyReleaseEvent(QKeyEvent *event) override;

	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
	void FitView();





protected:
	cv::Mat m_matSrc;
	//cv::Mat m_matSrcRGB;
	std::vector<std::vector<cv::Point>> m_vecContours;
	bool m_bShowContours;
	QPen m_penRect;
	QBrush m_backBrush;


	int m_nCurrentX;
	int m_nCurrentY;
	int m_nCurrentWidth;
	int m_nCurrentHeight;
	float m_fCurrentScale;
	QVector<QRgb> m_vecColorTable;

	QRect m_rectVisible;
	QPen m_penRectVisible;

};
