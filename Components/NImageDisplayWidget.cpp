#include "nimagedisplaywidget.h"
#include <QPainter>
#include <QImage>
#include <QFileDialog>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QKeyEvent>
#include <QGridLayout>
#include <QSpacerItem>
#include "NImageThumbnailWidget.h"
#include <QClipboard>
#include <QRegExp>
//#include <opencv2/shape.hpp>


NImageDisplayWidget::NImageDisplayWidget(QWidget *parent)
	: QWidget(parent), m_bIsMoving(false), m_fMaxScale(20), m_fMinScale(0.05), m_backBrush(QColor::fromRgb(50, 50, 50))
	, m_bShowContours(true), m_penRect(QPen(QColor(255, 0, 0), 2)), m_nBorderSize(50), m_bClipBoradEnabled(false), m_bInfoTextEnabled(true)
{
	//ui.setupUi(this);
	SetupUI();
}

NImageDisplayWidget::~NImageDisplayWidget()
{

}



int NImageDisplayWidget::SetImage(const cv::Mat& matSrc)
{
	return SetImage(matSrc, std::vector < std::vector<cv::Point> >(), true);
}

int NImageDisplayWidget::SetImage(const cv::Mat& matSrc, bool bFitView)
{
	return SetImage(matSrc, std::vector<std::vector<cv::Point> >(), bFitView);
}

int NImageDisplayWidget::SetImage(const cv::Mat& matSrc, const std::vector<std::vector<cv::Point> >& vecContours)
{
	return SetImage(matSrc, vecContours, true);
}

int NImageDisplayWidget::SetImage(const cv::Mat& matSrc, const std::vector<std::vector<cv::Point> >& vecContours, bool bFitView)
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
	if (nType == CV_16UC1)
	{
		m_matSrc = m_matSrc / 16;
		m_matSrc.convertTo(m_matSrc, CV_8UC1);
	}
	switch (m_matSrc.type())
	{
	case 1:
		//cvtColor(matSrc, m_matSrcRGB, CV_GRAY2RGB);
		break;
	case CV_8UC3:
		cvtColor(matSrc, m_matSrcRGB, CV_BGR2RGB);
		break;
	case CV_8UC4:
		cvtColor(matSrc, m_matSrcRGB, CV_BGRA2RGB);
		break;
	default:
		break;
	}
	// 0, 0, 0, 0 同时坐标原点
	if (bFitView)
	{
		FitView();
	}
	else
	{
		update();
	}
	m_pointCurrentWidget = this->pos();
	if (!m_imageThumbnailWidget->isHidden())
	{
		m_imageThumbnailWidget->SetImage(m_matSrc, vecContours);

	}
	return 0;
}

void NImageDisplayWidget::SetInfoTextEnabled(bool bEnabled /*= true*/)
{
	m_bInfoTextEnabled = bEnabled;
}

void NImageDisplayWidget::SetMaxMinScale(float fMaxScale, float fMinScale)
{
	if (fMaxScale > fMinScale && fMaxScale > 1 && fMinScale > 0)
	{
		m_fMaxScale = fMaxScale;
		m_fMinScale = fMinScale;
	}
}

bool NImageDisplayWidget::OpenFile(QString strFilePath)
{
	cv::Mat matSrc = cv::imread((const char*)strFilePath.toLocal8Bit(), CV_LOAD_IMAGE_UNCHANGED);
	if (SetImage(matSrc) == 0)
	{
		emit postImageOpened(matSrc);
		m_strFilePath = strFilePath;
		setWindowTitle(QStringLiteral("图片浏览器 -- %1").arg(m_strFilePath));
		Q_EMIT postOpenedImagePath(m_strFilePath);
		return true;
	}
	return false;
}

const cv::Mat& NImageDisplayWidget::GetImage()
{
	return m_matSrc;
}

const std::vector<std::vector<cv::Point>>& NImageDisplayWidget::GetContours()
{
	return m_vecContours;
}

void NImageDisplayWidget::Translate(const QPoint& point)
{
	m_nCurrentX = point.x();
	m_nCurrentY = point.y();
	update();
}

void NImageDisplayWidget::ResizeWindow(int nWidth, int nHeight)
{
	resize(nWidth, nHeight);
}

void NImageDisplayWidget::Scale(float fScale, const QPoint& pointBase)
{
	Room(m_fCurrentScale, fScale, pointBase);
}

void NImageDisplayWidget::actionTranslatePoint(const QPoint& point)
{
	Translate(point);
}

void NImageDisplayWidget::actionResize(int nWidth, int nHeight)
{
	ResizeWindow(nWidth, nHeight);
}

void NImageDisplayWidget::actionScale(float fScale, const QPoint& pointBase)
{
	Scale(fScale, pointBase);
}

void NImageDisplayWidget::actionFitView()
{
	FitView();
}


void NImageDisplayWidget::actionOrignalView(const QPoint& pointBase)
{
	OrignalView(pointBase);
}

void NImageDisplayWidget::actionShowDefects(bool bShowDefects)
{
	SetShowDefects(bShowDefects);
}

void NImageDisplayWidget::actionImagePoint(const QPoint& pointInImage)
{
	QPoint p = QPoint(pointInImage.x() * m_fCurrentScale, pointInImage.y() * m_fCurrentScale);
	QPoint vec = QPoint(this->contentsRect().center().x() - p.x(), this->contentsRect().center().y() - p.y());
	Translate(vec);
	emit postTranslatePoint(vec);
}



void NImageDisplayWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.fillRect(contentsRect(), m_backBrush);
	if (!m_matSrc.empty())
	{
		QRect targetRect;
		// 		targetRect.setX(m_nCurX);
		// 		targetRect.setY(m_nCurY);
		// 		targetRect.setWidth(m_nCurWidth);
		// 		targetRect.setHeight(m_nCurHeight);

		targetRect.setX(m_nCurrentX);
		targetRect.setY(m_nCurrentY);
		targetRect.setWidth(m_nCurrentWidth);
		targetRect.setHeight(m_nCurrentHeight);
		//QRect rectIntersected = targetRect.intersected(sourceRect);
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
			QImage image(m_matSrcRGB.data, m_matSrcRGB.cols, m_matSrcRGB.rows, m_matSrcRGB.step, QImage::Format_RGB888);
			painter.drawImage(targetRect, image);
		}
		else if (nChannels == 4)
		{
			//QImage image(m_matSrcRGB.data, m_nSrcWidth, m_nSrcHeight, m_matSrcRGB.step, QImage::Format_RGB888);

			QImage image(m_matSrcRGB.data, m_matSrcRGB.cols, m_matSrcRGB.rows, m_matSrcRGB.step, QImage::Format_RGB888);
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
				painter.drawRect(qRect);
			}
		}
		if (m_bInfoTextEnabled)
		{


			painter.setPen(QColor(134, 135, 244));
			//painter.drawText(0, 20, QStringLiteral("room:%1").arg(m_fScale));
			painter.drawText(0, 20, QStringLiteral("room:%1").arg(m_fCurrentScale));
			painter.drawText(0, 40, m_bShowContours ? QStringLiteral("显示轮廓(%1)").arg(m_vecContours.size()) : QStringLiteral("隐藏轮廓"));
			bool bXRange = m_pointMouseOnPixel.x() >= 0 && m_pointMouseOnPixel.x() < m_matSrc.cols;
			bool bYRange = m_pointMouseOnPixel.y() >= 0 && m_pointMouseOnPixel.y() < m_matSrc.rows;
			if (bXRange && bYRange)
			{
				switch (m_matSrc.type())
				{
				case CV_8UC1:
				{
					painter.drawText(0, 60, QStringLiteral("(%1,%2):%3").arg(m_pointMouseOnPixel.x(), 4).arg(m_pointMouseOnPixel.y(), 4).arg(m_matSrc.at<uchar>(m_pointMouseOnPixel.y(), m_pointMouseOnPixel.x()), 3));
					break;
				}
				case CV_8UC3:
				{
					cv::Vec3b& color = m_matSrc.at<cv::Vec3b>(m_pointMouseOnPixel.y(), m_pointMouseOnPixel.x());
					painter.drawText(0, 60, QStringLiteral("(%1,%2):(%3,%4,%5)").arg(m_pointMouseOnPixel.x(), 4).arg(m_pointMouseOnPixel.y(), 4).arg(color[0], 3).arg(color[1], 3).arg(color[2], 3));
					break;
				}

				case CV_8UC4:
				{
					cv::Vec4b& color = m_matSrc.at<cv::Vec4b>(m_pointMouseOnPixel.y(), m_pointMouseOnPixel.x());
					painter.drawText(0, 60, QStringLiteral("(%1,%2):(%3,%4,%5,%6)").arg(m_pointMouseOnPixel.x(), 4).arg(m_pointMouseOnPixel.y(), 4).arg(color[0], 3).arg(color[1], 3).arg(color[2], 3).arg(color[3], 3));
					break;
				}
				}
			}
		}
		// 		else
		// 		{
		// 			//painter.drawText(0, 60, QStringLiteral("像素值:(%1, %2):%3").arg("?").arg("?").arg("?"));
		// 		}
	}
}

void NImageDisplayWidget::mousePressEvent(QMouseEvent *event)
{
	setFocus(Qt::MouseFocusReason);
	if (event->button() == Qt::LeftButton)
	{
		m_bIsMoving = true;
		m_pointStartMoving = event->pos();
	}
}

void NImageDisplayWidget::mouseMoveEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	if (event->buttons()&Qt::LeftButton)
	{
		if (m_bIsMoving)
		{
			// 			QPoint vec = event->pos() - m_pointStartMoving;
			// 			m_nOffsetX = vec.x();
			// 			m_nOffsetY = vec.y();
			// 			m_nCurX += m_nOffsetX;
			// 			m_nCurY += m_nOffsetY;
			// 			update();

			QPoint vec = pos - m_pointStartMoving;
			m_nCurrentX += vec.x();
			m_nCurrentY += vec.y();
			// 			int nY = m_nCurrentY + vec.y();
			// 			QRect rect(nX, nY, nX + m_nCurrentWidth, nY + m_nCurrentHeight);
			//if ((rect.bottom() > m_nBorderSize && rect.right() > m_nBorderSize)  ||
			//				(rect.bottom() > m_nBorderSize && nX < this->contentsRect().right() - m_nBorderSize) || 
			//				(nX < this->contentsRect().right() - m_nBorderSize && nY < this->contentsRect().bottom() - m_nBorderSize) ||
			//				(nX > m_nBorderSize && nY < this->contentsRect().bottom() - m_nBorderSize))
			//			QRect& rectContent = this->contentsRect();
			//			QRect rectIntersect = rectContent.intersected(rect);
			//if (rectIntersect.width() > m_nBorderSize || rectIntersect.height() > m_nBorderSize|| rectContent.contains(rect))
			QRect& rectContent = this->contentsRect();
			if (m_nCurrentX + m_nCurrentWidth < m_nBorderSize)
			{
				m_nCurrentX = -m_nCurrentWidth + m_nBorderSize;
			}
			if (m_nCurrentY + m_nCurrentHeight < m_nBorderSize)
			{
				m_nCurrentY = -m_nCurrentHeight + m_nBorderSize;
			}
			if (m_nCurrentX > rectContent.width() - m_nBorderSize)
			{
				m_nCurrentX = rectContent.width() - m_nBorderSize;
			}
			if (m_nCurrentY > rectContent.height() - m_nBorderSize)
			{
				m_nCurrentY = rectContent.height() - m_nBorderSize;
			}
			emit postTranslatePoint(QPoint(m_nCurrentX, m_nCurrentY));

			m_pointStartMoving = event->pos();
			m_pointPreviousBase = event->pos();
			m_pointMouseOnPixel = QPoint((pos.x() - m_nCurrentX) * 1.0f / m_fCurrentScale,
				(pos.y() - m_nCurrentY) * 1.0f / m_fCurrentScale);
			update();
			return;
		}

	}
	QRect rect(m_nCurrentX, m_nCurrentY, m_nCurrentWidth, m_nCurrentHeight);
	if (rect.contains(pos))
	{
		m_pointMouseOnPixel = QPoint((pos.x() - m_nCurrentX) * 1.0f / m_fCurrentScale,
			(pos.y() - m_nCurrentY) * 1.0f / m_fCurrentScale);
		update(QRect(0, 0, 200, 200));
	}
	else
	{
		m_pointMouseOnPixel = QPoint(-1, -1);
	}
}

void NImageDisplayWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (m_bIsMoving)
		{
			m_bIsMoving = false;
		}
	}
	else
	{
		if (event->button() == Qt::RightButton)
		{
			emit postPixelPointOnImage(cv::Point(m_pointMouseOnPixel.x(), m_pointMouseOnPixel.y()));
		}
	}
}

void NImageDisplayWidget::wheelEvent(QWheelEvent *event)
{


	float fPreviousScale = m_fCurrentScale;
	if (event->delta() > 0)
	{
		m_fCurrentScale *= 1.2f;
		if (m_fCurrentScale > m_fMaxScale)
		{
			m_fCurrentScale = m_fMaxScale;
		}
	}
	else
	{
		m_fCurrentScale /= 1.2f;
		if (m_fCurrentScale < m_fMinScale)
		{
			m_fCurrentScale = m_fMinScale;
		}
	}
	if (fPreviousScale != m_fCurrentScale)
	{
		emit postScale(m_fCurrentScale, event->pos());
		Room(fPreviousScale, m_fCurrentScale, event->pos());
	}

}

void NImageDisplayWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasText())
	{
		event->accept();
	}
}

void NImageDisplayWidget::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasText())
	{
		QString strFilePath = event->mimeData()->text();
		strFilePath = strFilePath.right(strFilePath.length() - 8);
		// 		cv::Mat matTmp  = cv::imread((const char*)strFilePath.toLocal8Bit(), CV_LOAD_IMAGE_UNCHANGED);
		// 		if(!matTmp.empty())
		// 		{
		// 			SetImage(matTmp);
		// 		}
		OpenFile(strFilePath);
		event->accept();
	}
}

void NImageDisplayWidget::SetupUI()
{
	for (int k = 0; k < 256; ++k)
	{
		m_vecColorTable.push_back(qRgb(k, k, k));
	}
	this->setAcceptDrops(true);
	//setDragEnabled(true);
	setWindowTitle(QStringLiteral("图片浏览器"));
	//在外面控制窗口的显示
	// 	QRect screenRect = QApplication::desktop()->screenGeometry();
	// 	int w = screenRect.width(), h = screenRect.height();
	// 	resize(0.75 * w	, 0.75 * h);
	// 	move(( 0.25 * w) / 2, (0.25 * h) / 2);
	// 	setFixedSize(0.75* w, 0.75 * h);
	QGridLayout* gridLayout = new QGridLayout;
	gridLayout->setMargin(2);
	gridLayout->setSpacing(2);
	QSpacerItem* hSpaceItem = new QSpacerItem(50, 50, QSizePolicy::Expanding, QSizePolicy::Expanding);
	QSpacerItem* vSpaceItem = new QSpacerItem(50, 50, QSizePolicy::Expanding, QSizePolicy::Expanding);
	gridLayout->addItem(vSpaceItem, 0, 0, 2, 1);
	gridLayout->addItem(hSpaceItem, 2, 1, 1, 2);
	m_imageThumbnailWidget = new NImageThumbnailWidget(this);
	connect(m_imageThumbnailWidget, &NImageThumbnailWidget::postImagePoint, this, &NImageDisplayWidget::actionImagePoint);
	m_imageThumbnailWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_imageThumbnailWidget->setFixedSize(200, 200);
	gridLayout->addWidget(m_imageThumbnailWidget, 3, 0, 1, 1);

	this->setLayout(gridLayout);
	//setMouseTracking(true);

}

void NImageDisplayWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (!m_matSrc.empty())
	{
		// 		if (event->button() == Qt::LeftButton)
		// 		{
		// 			m_nOrignalShowX = (width() - m_nSrcWidth) / 2;
		// 			m_nOrignalShowY = (height() - m_nSrcHeight) / 2;
		// 			if (m_nSrcWidth / m_nSrcHeight > width() / height())
		// 			{
		// 				m_nSpecificX = 0;
		// 				m_nSpecificHeight = width() * m_nSrcHeight / m_nSrcWidth;
		// 				m_nSpecificWidth = width();
		// 				m_nSpecificY = (height() - m_nSpecificHeight) / 2;
		// 				// 		m_nCurX = m_nOrignalShowX;
		// 				// 		m_nCurY = m_nOrignalShowY;
		// 			}
		// 			else
		// 			{
		// 				m_nSpecificY = 0;
		// 				m_nSpecificWidth = height() * m_nSrcWidth / m_nSrcHeight;
		// 				m_nSpecificHeight = height();
		// 				m_nSpecificX = (width() - m_nSpecificWidth) / 2;
		// 				// 		m_nCurX = m_nOrignalShowX;
		// 				// 		m_nCurY = m_nOrignalShowY;
		// 			}
		// 			m_nCurX = m_nOrignalShowX;
		// 			m_nCurY = m_nOrignalShowY;
		// 			m_nCurWidth = m_nSrcWidth;
		// 			m_nCurHeight = m_nSrcHeight;
		// 			m_fSpecificScale = m_nSpecificHeight * 1.0 / m_nSrcHeight;
		// 			Room(1, m_fSpecificScale, QPoint(width() / 2, height() / 2));
		// 			update();
		// 		}
		// 		else if (event->button() == Qt::RightButton)
		// 		{
		// 			Room(m_fScale, 1, event->pos());
		// 		}

		if (event->button() == Qt::LeftButton)
		{
			emit postFitView();
			FitView();
		}
		else if (event->button() == Qt::RightButton)
		{
			emit postOrignalView(event->pos());
			Room(m_fCurrentScale, 1, event->pos());
		}
	}

}

void NImageDisplayWidget::resizeEvent(QResizeEvent *event)
{
	emit postResize(event->size().width(), event->size().height());
	if (m_matSrc.empty())
		return;
	QPoint pointNew = this->pos();
	if (pointNew != m_pointCurrentWidget)
	{
		bool b = true;
	}
	//TODO:屏幕中心 相对于(0，0)的向量变了，可以根据这个改变m_nCurrentX等


}

void NImageDisplayWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V)
	{
		if (m_bClipBoradEnabled)
		{
			QClipboard *clipboard = QApplication::clipboard();
			QString originalText = clipboard->text();
			QString strFilePath = originalText.right(originalText.count() - 8);
			// 		QRegExp regExp("^[a-zA-Z]:(((\\(?! )[^/:*?<>\"" | \\] + ) + \\ ? ) | (\\) ? )\s*.(jpg | png | gif | jpeg)$");
			// 		if (regExp.exactMatch(strFilePath))
			// 		{
			// 			OpenFile(strFilePath);
			// 		}
			if (!strFilePath.isEmpty())
			{
				OpenFile(strFilePath);
			}

		}
		else
		{
			return QWidget::keyPressEvent(event);
		}

	}
	else if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_S)
	{
		if (!m_matSrc.empty())
		{
			QString strFilePath;
			cv::Mat matTmp;
			QString strSelectedFilter = "Images(*.jpg);;Image(*.bmp);;Image(*.png)";
			strFilePath = QFileDialog::getSaveFileName(this, QStringLiteral("保存图片"), "", strSelectedFilter);
			if (!strFilePath.isEmpty())
			{
				QFileInfo fileInfo(strFilePath);
				QString strSuffix = fileInfo.suffix();
				if (strSuffix == "png")
				{
					switch (m_matSrc.type())
					{
					case CV_8UC1:
						cv::cvtColor(m_matSrc, matTmp, CV_GRAY2BGRA);
						break;
					case CV_8UC3:
						cv::cvtColor(m_matSrc, matTmp, CV_BGR2BGRA);
						break;
					default:
						matTmp = m_matSrc;
					}
				}
				else
				{
					matTmp = m_matSrc;
				}
				cv::imwrite(qPrintable(strFilePath), matTmp);
			}
		}
	}
	else
	{
		return QWidget::keyPressEvent(event);
	}
}

void NImageDisplayWidget::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Space:
	{
		m_bShowContours = !m_bShowContours;
		emit postShowDefects(m_bShowContours);
		update();
	}
	break;
	case Qt::Key_Escape:
	{
		if (m_fCurrentScale != m_fPreviousScale)
		{
			emit postScale(m_fPreviousScale, m_pointPreviousBase);
			Room(m_fCurrentScale, m_fPreviousScale, m_pointPreviousBase);
		}
	}
	break;
	default:
		return QWidget::keyPressEvent(event);
	}
}

void NImageDisplayWidget::moveEvent(QMoveEvent *event)
{
	m_pointCurrentWidget = event->pos();
}

void NImageDisplayWidget::Room(float fCurScale, float fNewScale, QPoint pointBase)
{
	int newX = cvRound(pointBase.x() - fNewScale * (pointBase.x() - m_nCurrentX) / fCurScale);
	int newY = cvRound(pointBase.y() - fNewScale * (pointBase.y() - m_nCurrentY) / fCurScale);
	m_nCurrentX = newX;
	m_nCurrentY = newY;
	m_nCurrentWidth = cvRound(m_matSrc.cols * fNewScale);
	m_nCurrentHeight = cvRound(m_matSrc.rows * fNewScale);
	m_fCurrentScale = fNewScale;
	m_fPreviousScale = fCurScale;
	m_pointPreviousBase = pointBase;
	update();
}

QPoint NImageDisplayWidget::WidgetMapToImage(const QPoint& widgetPos)
{
	return QPoint((widgetPos.x() - m_nCurrentX) / m_fCurrentScale, (widgetPos.y() - m_nCurrentY) / m_fCurrentScale);
}

QPoint NImageDisplayWidget::ImageMapToWidget(const QPoint& imagePos)
{
	return  QPoint(imagePos.x() * m_fCurrentScale + m_nCurrentX, imagePos.y() * m_fCurrentScale + m_nCurrentY);
}

void NImageDisplayWidget::FitView()
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

void NImageDisplayWidget::OrignalView(QPoint pointBase)
{
	Room(m_fCurrentScale, 1, pointBase);
}

void NImageDisplayWidget::SetShowDefects(bool bShowDefects)
{
	m_bShowContours = bShowDefects;
	update();
}

void NImageDisplayWidget::SetThumbnailWidgetHidden(bool bHidden)
{
	if (bHidden)
	{
		m_imageThumbnailWidget->setHidden(bHidden);
	}
	else
	{
		m_imageThumbnailWidget->SetImage(m_matSrc);
	}
}

void NImageDisplayWidget::SetClipboardEnabled(bool bEnabled)
{
	m_bClipBoradEnabled = bEnabled;
}
