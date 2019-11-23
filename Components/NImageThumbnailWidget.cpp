#include "NImageThumbnailWidget.h"
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include "NImageDisplayWidget.h"
#include <QWheelEvent>
#include <QMouseEvent>

NImageThumbnailWidget::NImageThumbnailWidget(QWidget *parent)
	: QWidget(parent), m_penRectVisible(QColor(134, 134, 244))
	, m_backBrush(QColor::fromRgb(50, 50, 50))
	, m_bShowContours(true), m_penRect(QPen(QColor(255, 0, 0), 2))
{
	for (int k = 0; k < 256; ++k)
	{
		m_vecColorTable.push_back(qRgb(k, k, k));
	}
}

NImageThumbnailWidget::~NImageThumbnailWidget()
{

}

int NImageThumbnailWidget::SetImage(const cv::Mat& matSrc, const std::vector<std::vector<cv::Point> >& vecContours /*= std::vector<std::vector<cv::Point>>()*/)
{
	m_vecContours = vecContours;
	m_matSrc = matSrc;
	if (matSrc.empty())
	{
		update();
		return 1;
	}
	int nChannels = matSrc.channels();
	int nType = matSrc.type();
// 	if (nType == CV_16UC1)
// 	{
// 		m_matSrc = m_matSrc / 16;
// 		m_matSrc.convertTo(m_matSrc, CV_8UC1);
// 	}
// 	switch (nChannels)
// 	{
// 	case 1:
// 		//cvtColor(matSrc, m_matSrcRGB, CV_GRAY2RGB);
// 		break;
// 	case 3:
// 		cvtColor(matSrc, m_matSrcRGB, CV_BGR2RGB);
// 		break;
// 	case 4:
// 		cvtColor(matSrc, m_matSrcRGB, CV_BGRA2RGB);
// 		break;
// 	default:
// 		break;
// 	}
	// 0, 0, 0, 0 同时坐标原点

	FitView();
	return 0;
}

void NImageThumbnailWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setPen(m_penRectVisible);
	QStyleOption option;
	option.initFrom(this);
	painter.fillRect(this->contentsRect(), QColor(40, 40, 40));
	
	if (!m_matSrc.empty())
	{
		QRect targetRect, sourceRect;
		// 		targetRect.setX(m_nCurX);
		// 		targetRect.setY(m_nCurY);
		// 		targetRect.setWidth(m_nCurWidth);
		// 		targetRect.setHeight(m_nCurHeight);

		targetRect.setX(m_nCurrentX);
		targetRect.setY(m_nCurrentY);
		targetRect.setWidth(m_nCurrentWidth);
		targetRect.setHeight(m_nCurrentHeight);

		NImageDisplayWidget* pImageDisplayWidget = qobject_cast<NImageDisplayWidget*>(this->parentWidget());
		int nChannels = m_matSrc.channels();
		if (nChannels == 1)
		{
			//QImage image(m_matSrc.data, m_nSrcWidth, m_nSrcHeight, m_matSrc.step, QImage::Format_Indexed8);
			QImage image(m_matSrc.data, m_matSrc.cols, m_matSrc.rows, m_matSrc.step, QImage::Format_Indexed8);
			image.setColorTable(m_vecColorTable);
			painter.drawImage(targetRect, image);
		}
		else if (nChannels == 3)
		{
			//QImage image(m_matSrcRGB.data, m_nSrcWidth, m_nSrcHeight, m_matSrcRGB.step, QImage::Format_RGB888);

			QImage image(pImageDisplayWidget->m_matSrcRGB.data, pImageDisplayWidget->m_matSrcRGB.cols, pImageDisplayWidget->m_matSrcRGB.rows, pImageDisplayWidget->m_matSrcRGB.step, QImage::Format_RGB888);
			painter.drawImage(targetRect, image);
		}
		else if (nChannels == 4)
		{
			//QImage image(m_matSrcRGB.data, m_nSrcWidth, m_nSrcHeight, m_matSrcRGB.step, QImage::Format_RGB888);

			QImage image(pImageDisplayWidget->m_matSrcRGB.data, pImageDisplayWidget->m_matSrcRGB.cols, pImageDisplayWidget->m_matSrcRGB.rows, pImageDisplayWidget->m_matSrcRGB.step, QImage::Format_RGB888);
			painter.drawImage(targetRect, image);
		}

		if (m_bShowContours)
		{
			cv::Rect rect;
			QRect qRect;
			painter.setPen(m_penRect);
			for (int nIndex = 0; nIndex < m_vecContours.size(); ++nIndex)
			{
				rect = cv::boundingRect(m_vecContours[nIndex]);
				//qRect = QRect(m_nCurX + rect.x * m_fScale, m_nCurY + rect.y	* m_fScale, rect.width * m_fScale, rect.height * m_fScale);
				qRect = QRect(m_nCurrentX + rect.x * m_fCurrentScale, m_nCurrentY + rect.y	* m_fCurrentScale, rect.width * m_fCurrentScale, rect.height * m_fCurrentScale);
				painter.drawRect(qRect.adjusted(-1, -1, 1, 1));
			}
		}
	}

	//QApplication::style()->drawPrimitive(QStyle::PE_Frame, &option, &painter);
	painter.setPen(m_penRectVisible);
	painter.drawRect(this->contentsRect().adjusted(0, 0, -1, -1));

}

void NImageThumbnailWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void NImageThumbnailWidget::mouseReleaseEvent(QMouseEvent *event)
{

}

void NImageThumbnailWidget::wheelEvent(QWheelEvent *event)
{
	event->accept();
}

void NImageThumbnailWidget::FitView()
{
	QRect& contentRect = this->contentsRect();
	float fWidthScale = m_matSrc.cols * 1.0f / contentRect.width();
	float fHeightScale = m_matSrc.rows * 1.0 / contentRect.height();
	int nRows = m_matSrc.rows;
	int nCols = m_matSrc.cols;
	if (fWidthScale > fHeightScale)
	{
		m_fCurrentScale = 1 / fWidthScale;
		m_nCurrentWidth = m_fCurrentScale * nCols;
		m_nCurrentHeight = m_fCurrentScale * nRows;
		m_nCurrentX = 0;
		m_nCurrentY = (contentRect.height() - m_nCurrentHeight) / 2;
	}
	else
	{
		m_fCurrentScale = 1 / fHeightScale;
		m_nCurrentWidth = m_fCurrentScale * nCols;
		m_nCurrentHeight = m_fCurrentScale * nRows;
		m_nCurrentX = (contentRect.width() - m_nCurrentWidth) / 2;
		m_nCurrentY = 0;
	}
	update();
}

void NImageThumbnailWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (m_matSrc.empty())
	{
		event->accept();
		return;	
	}
	QPoint pos = event->pos();
	if (QRect(m_nCurrentX, m_nCurrentY, m_nCurrentWidth, m_nCurrentHeight).contains(pos))
	{
		emit postImagePoint(QPoint(int((pos.x() - m_nCurrentX) * 1.0 / m_fCurrentScale), int((pos.y() - m_nCurrentY) * 1.0f / m_fCurrentScale)));
	}
	event->accept();
}


void NImageThumbnailWidget::keyReleaseEvent(QKeyEvent *event)
{

}



