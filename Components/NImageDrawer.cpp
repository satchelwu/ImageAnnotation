#include "NImageDrawer.h"
#include <QDir>
#include <QFileInfo>
#include <QPen>
#include <QApplication>


NImageDrawer::NImageDrawer(QWidget *parent)
	: NImageDisplayWidget(parent), m_bCanDraw(false), m_nLineSize(5), m_bIsTriggerOn(false), m_nCanDrawRectCount(65526)
{
	SetupUI();
	SetupConnect();
}

NImageDrawer::~NImageDrawer()
{

}

void NImageDrawer::DrawRects(QPainter* painter)
{
	QPen pen(QBrush(Qt::red), m_nLineSize);
	painter->setPen(pen);
	int nVecSize = m_vecRect.size();
	// 矫正矩形框 ， 从原始的图像(0, 0) 坐标开始矫正成现在放大之后，平移之后的
	// 首先平移到现在的图像的左上角，然后再放大
	int nRectRectifyX = 0, nRectRectifyY = 0, nRectRectifyWidth = 0, nRectRectifyHeight = 0;
	for (QList<cv::Rect>::const_iterator iTer = m_vecRect.begin(); iTer != m_vecRect.end(); ++iTer)
	{
		const cv::Rect& rectRef = *iTer;
// 		nRectRectifyX = rectRef.x * m_fCurrentScale + m_nCurrentX + 0.5f;
// 		nRectRectifyY = rectRef.y * m_fCurrentScale + m_nCurrentY + 0.5f;
// 		nRectRectifyWidth = rectRef.width * m_fCurrentScale + 0.5f;
// 		nRectRectifyHeight = rectRef.height * m_fCurrentScale + 0.5f;
// 		QRect rect(nRectRectifyX, nRectRectifyY, nRectRectifyWidth, nRectRectifyHeight);
		painter->drawRect(RectifiedRect(rectRef));
	}
}


QRectF NImageDrawer::RectifiedRect(const cv::Rect& rect)
{
	float nRectRectifyX = 0, nRectRectifyY = 0, nRectRectifyWidth = 0, nRectRectifyHeight = 0;
	nRectRectifyX = rect.x * m_fCurrentScale + m_nCurrentX /*+ 0.5f*/;
	nRectRectifyY = rect.y * m_fCurrentScale + m_nCurrentY /*+ 0.5f*/;
	nRectRectifyWidth = rect.width * m_fCurrentScale /*+ 0.5f*/;
	nRectRectifyHeight = rect.height * m_fCurrentScale /*+ 0.5f*/;
	return QRectF(nRectRectifyX, nRectRectifyY, nRectRectifyWidth, nRectRectifyHeight);
}

cv::Rect NImageDrawer::MapToOrignalRect(const QRectF& rect)
{
	int rectOrignalX = qMax(0.f, qMin((float)((rect.x() - m_nCurrentX) * 1.0f / m_fCurrentScale + 0.5f), (float)m_matSrc.cols));
	int rectOrignalY = qMax(0.f, qMin((float)((rect.y() - m_nCurrentY) * 1.0f / m_fCurrentScale + 0.5f), (float)m_matSrc.rows));
	int rectOrignalWidth = qMax(0.f, qMin((float)(rect.width() / m_fCurrentScale + 0.5f), (float)m_matSrc.cols));
	int rectOrignalHeight = qMax(0.f, qMin((float)(rect.height() / m_fCurrentScale + 0.5f), (float)m_matSrc.rows));
	return cv::Rect(rectOrignalX, rectOrignalY, rectOrignalWidth, rectOrignalHeight);
}

void NImageDrawer::OpenFile(QString strFilePath)
{
	NImageDisplayWidget::OpenFile(strFilePath);
	actionResetImage();
	m_pActionEnableDrawing->setChecked(false);
	m_bCanDraw = false;
}

void NImageDrawer::SetCanDrawRectCount(int nRectCount /*= 65526*/)
{
	int nCount = qMax(0, nRectCount);
	if (nCount < m_vecRect.count())
	{
		m_vecRect = m_vecRect.mid(0, nCount);
	}
	m_nCanDrawRectCount = nCount;
}

void NImageDrawer::paintEvent(QPaintEvent *event)
{
	//先调用原来的图片
	NImageDisplayWidget::paintEvent(event);
	//TODO:// Rect 要保持放大比例 
	QPainter painter(this);
	DrawRects(&painter);
	if (!m_CurrentDrawingRect.isEmpty())
	{
		QPen pen(QBrush(Qt::red), m_nLineSize);
		painter.setPen(pen);
		painter.drawRect(m_CurrentDrawingRect);
	}
}

void NImageDrawer::mousePressEvent(QMouseEvent *event)
{

	if (!(QApplication::keyboardModifiers() & Qt::ControlModifier))
	{
		m_bCanDraw = false;
		NImageDisplayWidget::mousePressEvent(event);
		if (event->button() == Qt::LeftButton)
		{
			m_pointStartPoint = event->pos();
		}
	}
	else
	{
		if (event->button() == Qt::LeftButton)
		{
			m_bCanDraw = true;
			m_pointStartPoint = event->pos();
			if (m_pointStartPoint.x() < m_nCurrentX)
			{
				m_pointStartPoint.setX(m_nCurrentX);
			}
			if (m_pointStartPoint.y() < m_nCurrentY)
			{
				m_pointStartPoint.setY(m_nCurrentY);
			}
			if (m_pointStartPoint.x() > m_nCurrentX + m_nCurrentWidth)
			{
				m_pointStartPoint.setX(m_nCurrentX + m_nCurrentWidth);
			}
			if (m_pointStartPoint.y() > m_nCurrentY + m_nCurrentHeight)
			{
				m_pointStartPoint.setY(m_nCurrentY + m_nCurrentHeight);
			}
		}
	}
}

void NImageDrawer::mouseMoveEvent(QMouseEvent *event)
{
	if (m_bCanDraw == false)
	{
		NImageDisplayWidget::mouseMoveEvent(event);

#pragma region Obsolete
		// 		int nSize = m_vecRect.size();
		// 		QPoint vecTmp = event->pos() - m_pointStartPoint;
		// 		for (int nIndex = 0; nIndex < m_vecRect.size(); ++nIndex)
		// 		{
		// 			cv::Rect& rectRef = m_vecRect.at(nIndex);
		// 			rectRef.x += rectRef.x + m_nCurrentX;
		// 			rectRef.y += rectRef.y + m_nCurrentY;
		// 		}  
#pragma endregion

		update();
		m_pointStartPoint = event->pos();

	}
	else if (m_vecRect.count() < m_nCanDrawRectCount)
	{
		if (event->buttons() & Qt::LeftButton)
		{
			QPoint pointEnd = event->pos();
			if (pointEnd.x() < m_nCurrentX)
			{
				pointEnd.setX(m_nCurrentX);
			}
			if (pointEnd.y() < m_nCurrentY)
			{
				pointEnd.setY(m_nCurrentY);
			}
			if (pointEnd.x() > m_nCurrentX + m_nCurrentWidth)
			{
				pointEnd.setX(m_nCurrentX + m_nCurrentWidth);
			}
			if (pointEnd.y() > m_nCurrentY + m_nCurrentHeight)
			{
				pointEnd.setY(m_nCurrentY + m_nCurrentHeight);
			}
			QPoint vecTmp = pointEnd - m_pointStartPoint;
			int nWidth = vecTmp.x(), nHeight = vecTmp.y();
			if (nWidth >= 10 && nHeight >= 10)
			{
				m_CurrentDrawingRect.setX(m_pointStartPoint.x());
				m_CurrentDrawingRect.setY(m_pointStartPoint.y());
				m_CurrentDrawingRect.setWidth(vecTmp.x());
				m_CurrentDrawingRect.setHeight(vecTmp.y());
			}
			else
			{
				m_CurrentDrawingRect.setSize(QSize());
			}
			update();
		}
	}
}

void NImageDrawer::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_bCanDraw == false)
	{
		NImageDisplayWidget::mouseReleaseEvent(event);
	}
	else if (m_vecRect.count() < m_nCanDrawRectCount)
	{
		if (event->button() == Qt::LeftButton)
		{
			if (!m_CurrentDrawingRect.isEmpty())
			{
				if (m_CurrentDrawingRect.width() >= 10 && m_CurrentDrawingRect.height() >= 10)
				{
					// 矫正当前矩形框为对应于图片的矩形框
					int rectOrignalX = qMax(0.f, qMin((m_CurrentDrawingRect.x() - m_nCurrentX) * 1.0f / m_fCurrentScale + 0.5f, (float)m_matSrc.cols));
					int rectOrignalY = qMax(0.f, qMin((m_CurrentDrawingRect.y() - m_nCurrentY) * 1.0f / m_fCurrentScale + 0.5f, (float)m_matSrc.rows));
					int rectOrignalWidth = qMax(0.f, qMin(m_CurrentDrawingRect.width() / m_fCurrentScale + 0.5f, (float)m_matSrc.cols));
					int rectOrignalHeight = qMax(0.f, qMin(m_CurrentDrawingRect.height() / m_fCurrentScale + 0.5f, (float)m_matSrc.rows));
					cv::Rect rectTmp(rectOrignalX, rectOrignalY, rectOrignalWidth, rectOrignalHeight);

					m_vecRect.push_back(rectTmp);
					m_CurrentDrawingRect.setSize(QSize());
				}
				else
				{
					m_CurrentDrawingRect.setSize(QSize());
				}
				update();
			}
			
		}
	}
	m_bCanDraw = false;
}

void NImageDrawer::mouseDoubleClickEvent(QMouseEvent *event)
{
	// 	int oldX = m_nCurrentX, oldY = m_nCurrentY, oldWidth = m_nCurrentWidth, oldHeight = m_nCurrentHeight;
	// 	float fOldScale = m_fCurrentScale;
	// 	for (int nIndex = 0; nIndex < m_vecRect.size(); ++nIndex)
	// 	{
	// 		cv::Rect& rectTmp = m_vecRect.at(nIndex);
	// 		rectTmp.x = (rectTmp.x - oldX) * m_nCurrentWidth * 1.0 / oldWidth + m_nCurrentX;
	// 		rectTmp.y = (rectTmp.y - oldY) * m_nCurrentHeight * 1.0 / oldHeight + m_nCurrentY;
	// 		rectTmp.width = rectTmp.width * m_fCurrentScale / fOldScale;
	// 		rectTmp.height = rectTmp.height * m_fCurrentScale / fOldScale;
	// 	}
	//update();
	if (m_bCanDraw == false)
	{
		NImageDisplayWidget::mouseDoubleClickEvent(event);
	}
}

void NImageDrawer::wheelEvent(QWheelEvent *event)
{
	if (m_bCanDraw == false)
	{
		float  fOldScale = m_fCurrentScale;
		NImageDisplayWidget::wheelEvent(event);
		// 		for (int nIndex = 0; nIndex < m_vecRect.size(); ++nIndex)
		// 		{
		// 			RoomRect(m_vecRect.at(nIndex), fOldScale, m_fCurrentScale, event->pos());
		// 		}
		update();
	}
}


// void NImageDrawer::contextMenuEvent(QContextMenuEvent *event)
// {
// 	QPoint pointTmp = mapToGlobal(event->pos());
// 	pointTmp.setX(pointTmp.x() + 5);
// 	pointTmp.setY(pointTmp.y() + 5);
// 	m_pContextMenu->exec(pointTmp);
// }

// void NImageDrawer::keyPressEvent(QKeyEvent *event)
// {
// 	if (event->modifiers() & Qt::ControlModifier)
// 	{
// 		//m_bIsTriggerOn = true;	
// 		//m_bCanDraw = true;
// 		if (event->key() == Qt::Key_Z)
// 		{
// 			if (m_vecRect.size() > 0)
// 			{
// 				m_vecRect.pop_back();
// 				update();
// 			}
// 		}
// 	}
// 	return QWidget::keyPressEvent(event);
// }

// void NImageDrawer::keyReleaseEvent(QKeyEvent* event)
// {
// 	Qt::KeyboardModifiers k = event->nativeModifiers();
// 	int nKey = event->key();
// 	int n = Qt::ControlModifier & k;
// 	if (k == Qt::CTRL)
// 	{
// 		m_bIsTriggerOn = false;
// 		update();
// 	}
// 	return NImageDisplayWidget::keyReleaseEvent(event);
// }

void NImageDrawer::dropEvent(QDropEvent *event)
{
	NImageDisplayWidget::dropEvent(event);
	actionResetImage();
	m_pActionEnableDrawing->setChecked(false);
	m_bCanDraw = false;
}

void NImageDrawer::actionCanDrawCheckChanged(bool bCheck)
{
	if (m_matSrc.empty())
	{
		m_pActionEnableDrawing->setChecked(false);
		return;
	}
	if (bCheck == true)
	{
		m_bCanDraw = true;
	}
	else
	{
		m_bCanDraw = false;
	}
}

void NImageDrawer::actionSaveImage()
{
	if (m_matSrc.empty())
		return;
	QFileInfo fileInfo(m_strFilePath);
	QString strDir = fileInfo.dir().absolutePath();
	QString strFileNameExt = fileInfo.fileName();
	QString strFileName = strFileNameExt.left(strFileNameExt.length() - 4);
	cv::Mat matTmp = m_matSrcRGB.clone();
	cvtColor(m_matSrc, matTmp, CV_RGB2BGR);
	for (QList<cv::Rect>::iterator iTer = m_vecRect.begin(); iTer != m_vecRect.end(); ++iTer)
	{
		//归一化方框  现在不用归一化了
		cv::Rect& rectTmp = *iTer;
		/*rectTmp.x = (rectTmp.x - m_nCurrentX) / m_fCurrentScale;
		rectTmp.y = (rectTmp.y - m_nCurrentY) / m_fCurrentScale;
		if(rectTmp.x < 0)
		{
		rectTmp.x = 0;
		}
		if(rectTmp.x > m_matSrc.cols)
		{
		rectTmp.x = m_matSrc.cols - 1;
		}
		if(rectTmp.y < 0)
		{
		rectTmp.y = 0;
		}
		if(rectTmp.y > m_matSrc.rows - 1)
		{
		rectTmp.y  = m_matSrc.rows - 1;
		}
		rectTmp.width = rectTmp.width / m_fCurrentScale;
		rectTmp.height = rectTmp.height / m_fCurrentScale;
		if(rectTmp.x + rectTmp.width > m_matSrc.cols - 1)
		{
		rectTmp.width = m_matSrc.cols - 1 - rectTmp.x;
		}
		if(rectTmp.y + rectTmp.height > m_matSrc.rows - 1)
		{
		rectTmp.height = m_matSrc.rows - 1 - rectTmp.y;
		}*/
		cv::rectangle(matTmp, rectTmp, cv::Scalar(0, 0, 255), m_nLineSize * 1.25);
	}
	cv::imwrite(QStringLiteral("%1/%2.jpg").arg(strDir).arg(strFileName).toStdString(), matTmp);
	setWindowTitle(QStringLiteral("%1 保存为 %2.jpg").arg(windowTitle()).arg(strFileName));
}

void NImageDrawer::actionResetImage()
{
	m_vecRect.clear();
	update();
}

void NImageDrawer::actionIncreaseLineSize()
{
	m_nLineSize += 2;
	if (m_nLineSize > 50)
		m_nLineSize = 50;
	update();
}

void NImageDrawer::actionDecreaseLineSize()
{
	m_nLineSize -= 2;
	if (m_nLineSize < 5)
	{
		m_nLineSize = 5;
	}
	update();
}

void NImageDrawer::RoomRect(cv::Rect& rect, float fCurrentScale, float fNewScale, QPoint pointBase)
{
	int newX = cvRound(pointBase.x() - fNewScale * (pointBase.x() - rect.x) / fCurrentScale);
	int newY = cvRound(pointBase.y() - fNewScale * (pointBase.y() - rect.y) / fCurrentScale);
	rect.x = newX;
	rect.y = newY;
	rect.width = cvRound(rect.width * fNewScale / fCurrentScale);
	rect.height = cvRound(rect.height * fNewScale / fCurrentScale);
}

void NImageDrawer::SetupUI()
{
	//setMouseTracking(true);
	m_pContextMenu = new QMenu(this);
	setContextMenuPolicy(Qt::DefaultContextMenu);
	m_pActionEnableDrawing = new QAction(this);
	m_pActionEnableDrawing->setCheckable(true);
	m_pActionEnableDrawing->setChecked(false);
	m_pActionEnableDrawing->setText(QStringLiteral("标记"));
	m_pContextMenu->addAction(m_pActionEnableDrawing);
	//m_pActionEnableDrawing->setEnabled(false);

	m_pActionSaveImage = new QAction(this);
	m_pActionSaveImage->setText(QStringLiteral("保存"));
	m_pContextMenu->addAction(m_pActionSaveImage);

	m_pActionResetImage = new QAction(this);
	m_pActionResetImage->setText(QStringLiteral("重置"));
	m_pContextMenu->addAction(m_pActionResetImage);

	m_pActionIncreaseLineSize = new QAction(this);
	m_pActionIncreaseLineSize->setText(QStringLiteral("加粗"));
	m_pContextMenu->addAction(m_pActionIncreaseLineSize);

	m_pActionDecreaseLineSize = new QAction(this);
	m_pActionDecreaseLineSize->setText(QStringLiteral("减细"));
	m_pContextMenu->addAction(m_pActionDecreaseLineSize);
	//SetMaxMinScale(10, 0.15);
}

void NImageDrawer::SetupConnect()
{
	connect(m_pActionEnableDrawing, &QAction::triggered, this, &NImageDrawer::actionCanDrawCheckChanged);
	connect(m_pActionSaveImage, &QAction::triggered, this, &NImageDrawer::actionSaveImage);
	connect(m_pActionResetImage, &QAction::triggered, this, &NImageDrawer::actionResetImage);
	connect(m_pActionIncreaseLineSize, &QAction::triggered, this, &NImageDrawer::actionIncreaseLineSize);
	connect(m_pActionDecreaseLineSize, &QAction::triggered, this, &NImageDrawer::actionDecreaseLineSize);
}
