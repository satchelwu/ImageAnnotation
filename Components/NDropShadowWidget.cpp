#include "NDropShadowWidget.h"
#include <QLayout>
#include <QtMath>
#include <QApplication>
#include <QDesktopWidget>
#include <QStyleOption>

NDropShadowWidget::NDropShadowWidget(QWidget *parent)
	: QWidget(parent), m_nShadowWidth(5), m_pWindowWidgetBar(NULL), m_bFixedSize(false)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
	//setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_PaintOnScreen);
	if (parent)
	{
		this->setMouseTracking(false);
	}
	else
	{
		this->setMouseTracking(true);
	}
	//SetShadowWidth(m_nShadowWidth);
}

NDropShadowWidget::~NDropShadowWidget()
{

}

void NDropShadowWidget::SetShadowWidth(int nWidth /*= 5*/)
{
	int width = qMax(0, nWidth);
	m_nShadowWidth = nWidth;
	QLayout* layout = this->layout();
	layout->setSpacing(0);
	if (layout)
	{
		layout->setMargin(nWidth);
	}
	update();
	//setContentsMargins(1, 1, 1, 1);
}

void NDropShadowWidget::setParent(QWidget* parent)
{
	QWidget::setParent(parent);
	if (parent)
	{
		this->setMouseTracking(false);
	}
	else
	{
		this->setMouseTracking(true);
	}
}

void NDropShadowWidget::SetWindowTitleBar(NWindowTitleBar* pWindowTitleBar)
{

}

void NDropShadowWidget::SetFixedSize(bool bFixedSize /*= false*/)
{
	m_bFixedSize = bFixedSize;
}

void NDropShadowWidget::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	//QWidget::paintEvent(event);
	if (!((this->windowState() & Qt::WindowMaximized) || (this->windowState() & Qt::WindowFullScreen)) && this->parent() == NULL)
	{
		//style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

		QPainterPath path;
		path.setFillRule(Qt::WindingFill);
		//path.addRect(m_nShadowWidth, m_nShadowWidth, this->width() - m_nShadowWidth * 2, this->height() - m_nShadowWidth * 2);

		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing, true);
		//painter.fillPath(path, QBrush(Qt::white));
		//painter.fillRect(this->frameGeometry(), Qt::black);
		QColor color(13, 32, 45, 200);
		int nDiff = (255 - 50) / m_nShadowWidth;
		for (int i = 0; i < m_nShadowWidth; i++)
		{
			QPainterPath path;
			//path.setFillRule(Qt::WindingFill);
			path.addRect(m_nShadowWidth - i, m_nShadowWidth - i, this->width() - (m_nShadowWidth - i) * 2, this->height() - (m_nShadowWidth - i) * 2);
			//color.setAlpha(200 - qSqrt(i) * 50);
			color.setAlpha(200 - nDiff * i);
			painter.setPen(color);
			painter.drawPath(path);
		}
	}
	else
	{
		/*const QBrush& backGroundBrush = opt.palette.background();
		p.fillRect(this->frameGeometry(), backGroundBrush);*/
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

	}
}

bool NDropShadowWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::Enter)
	{
		setCursor(Qt::ArrowCursor);
	}
	return QWidget::eventFilter(watched, event);
}

void NDropShadowWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (this->windowState() == Qt::WindowNoState)
		{
			if (m_bFixedSize == false)
			{
				ECursorPosition cp = CursorPosition(this->frameGeometry(), event->globalPos());
				if (cp != CursorPositionMiddle && cp != CursorPositionUndefined)
				{
					m_bResizing = true;
					m_pointStartResize = event->pos();
					m_eCursorPostion = cp;
				}
				else
				{
					m_bResizing = false;
				}
			}
			
		}
	}
	return QWidget::mousePressEvent(event);
}

void NDropShadowWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		Qt::WindowStates ws = this->windowState();
		if (ws == Qt::WindowNoState)
		{
			if (!m_bFixedSize && m_bResizing)
			{
				Resize(this->geometry(), m_eCursorPostion, event->globalPos());
			}
		}
	}

	else
	{
		if (this->windowState() == Qt::WindowNoState)
		{
			if (m_bFixedSize == false)
				UpdateCursorIcon(event->globalPos());
		}
		else
		{
			this->unsetCursor();
		}
	}

	return QWidget::mouseMoveEvent(event);
}

void NDropShadowWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_bResizing = false;
	}
	//UpdateCursorIcon(event->globalPos());
	return QWidget::mouseReleaseEvent(event);
}


void NDropShadowWidget::UpdateCursorIcon(const QPointF& _mousePosition)
{
	QCursor cursorIcon;
	QRectF rect = this->frameGeometry();
	switch (CursorPosition(rect, _mousePosition))
	{
	case CursorPositionTopRight:
	case CursorPositionBottomLeft:
		cursorIcon = QCursor(Qt::SizeBDiagCursor);
		break;
	case CursorPositionTopLeft:
	case CursorPositionBottomRight:
		cursorIcon = QCursor(Qt::SizeFDiagCursor);
		break;
	case CursorPositionTop:
	case CursorPositionBottom:
		cursorIcon = QCursor(Qt::SizeVerCursor);
		break;
	case CursorPositionLeft:
	case CursorPositionRight:
		cursorIcon = QCursor(Qt::SizeHorCursor);
		break;
		// 		case CursorPositionMiddle:
		// 			cursorIcon = m_bIsCropPressed ?
		// 				QCursor(Qt::ClosedHandCursor) :
		// 				QCursor(Qt::OpenHandCursor);
		// 			break;
	case CursorPositionUndefined:
	case CursorPositionMiddle:
	default:
		cursorIcon = QCursor(Qt::ArrowCursor);
		break;
	}
	this->setCursor(cursorIcon);
}


ECursorPosition NDropShadowWidget::CursorPosition(const QRectF& _cropRect, const QPointF& _mousePosition)
{
	ECursorPosition cursorPosition = CursorPositionUndefined;
	if (_cropRect.contains(_mousePosition))
	{

		if (isPointNearSide(_cropRect.top(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.left(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionTopLeft;
		}
		else if (isPointNearSide(_cropRect.bottom(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.left(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionBottomLeft;
		}
		else if (isPointNearSide(_cropRect.top(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.right(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionTopRight;
		}
		else if (isPointNearSide(_cropRect.bottom(), _mousePosition.y()) &&
			isPointNearSide(_cropRect.right(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionBottomRight;
		}
		else if (isPointNearSide(_cropRect.left(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionLeft;
		}
		else if (isPointNearSide(_cropRect.right(), _mousePosition.x()))
		{
			cursorPosition = CursorPositionRight;
		}
		else if (isPointNearSide(_cropRect.top(), _mousePosition.y()))
		{
			cursorPosition = CursorPositionTop;
		}
		else if (isPointNearSide(_cropRect.bottom(), _mousePosition.y()))
		{
			cursorPosition = CursorPositionBottom;
		}
		else {
			cursorPosition = CursorPositionMiddle;
		}
	}
	return cursorPosition;
}

const QRectF NDropShadowWidget::CalculateGeometry(const QRectF& _sourceGeometry, const ECursorPosition _cursorPosition, const QPointF& _mouseDelta)
{
	QRectF resultGeometry = _sourceGeometry;
	int nMinLength = 10;
	QDesktopWidget* desktop = QApplication::desktop();
	switch (_cursorPosition)
	{
	case CursorPositionTopLeft:
		resultGeometry.setLeft(qMax(0.f, (float)(_sourceGeometry.left() + _mouseDelta.x())));
		resultGeometry.setTop(qMax(0.f, (float)(_sourceGeometry.top() + _mouseDelta.y())));
		resultGeometry.setLeft(qMin(resultGeometry.left(), resultGeometry.right() - nMinLength));
		resultGeometry.setTop(qMin(resultGeometry.top(), resultGeometry.bottom() - nMinLength));
		break;
	case CursorPositionTopRight:
		resultGeometry.setTop(qMax(0.f, (float)(_sourceGeometry.top() + _mouseDelta.y())));
		resultGeometry.setRight(qMin((float)(desktop->rect().right()), (float)(_sourceGeometry.right() + _mouseDelta.x())));

		resultGeometry.setTop(qMin(resultGeometry.top(), resultGeometry.bottom() - nMinLength));
		resultGeometry.setRight(qMax(resultGeometry.left() + nMinLength, resultGeometry.right()));
		break;
	case CursorPositionBottomLeft:
		resultGeometry.setBottom(qMin((float)(desktop->rect().bottom()), (float)(_sourceGeometry.bottom() + _mouseDelta.y())));
		resultGeometry.setLeft(qMax((float)0, (float)(_sourceGeometry.left() + _mouseDelta.x())));

		resultGeometry.setBottom(qMax(resultGeometry.top() + nMinLength, resultGeometry.bottom()));
		resultGeometry.setLeft(qMin(resultGeometry.left(), resultGeometry.right() - nMinLength));
		break;
	case CursorPositionBottomRight:
		resultGeometry.setBottom(qMin((float)(desktop->rect().bottom()), (float)(_sourceGeometry.bottom() + _mouseDelta.y())));
		resultGeometry.setRight(qMin((float)(desktop->rect().right()), (float)(_sourceGeometry.right() + _mouseDelta.x())));

		resultGeometry.setRight(qMax(resultGeometry.left() + nMinLength, resultGeometry.right()));
		resultGeometry.setBottom(qMax(resultGeometry.top() + nMinLength, resultGeometry.bottom()));
		break;
	case CursorPositionTop:
		resultGeometry.setTop(qMax((float)0.f, (float)(_sourceGeometry.top() + _mouseDelta.y())));
		resultGeometry.setTop(qMin(resultGeometry.top(), resultGeometry.bottom() - nMinLength));

		break;
	case CursorPositionBottom:
		resultGeometry.setBottom(qMax(resultGeometry.top() + nMinLength, resultGeometry.bottom()));
		resultGeometry.setBottom(qMin((float)(desktop->rect().bottom()), (float)(_sourceGeometry.bottom() + _mouseDelta.y())));

		break;
	case CursorPositionLeft:
		resultGeometry.setLeft(qMax((float)0.f, (float)(_sourceGeometry.left() + _mouseDelta.x())));
		resultGeometry.setLeft(qMin(resultGeometry.left(), resultGeometry.right() - nMinLength));

		break;
	case CursorPositionRight:
		resultGeometry.setRight(qMin((float)(desktop->rect().right()), (float)(_sourceGeometry.right() + _mouseDelta.x())));
		resultGeometry.setRight(qMax(resultGeometry.left() + nMinLength, resultGeometry.right()));
		break;
	default:
		resultGeometry = _sourceGeometry;
		break;
	}
	if (!resultGeometry.isEmpty())
	{
		if ((resultGeometry.left() >= resultGeometry.right()) ||
			(resultGeometry.top() >= resultGeometry.bottom()))
		{
			resultGeometry = QRectF(resultGeometry.left(), resultGeometry.top(), 10, 10);
		}
	}
	return resultGeometry;
}

bool NDropShadowWidget::IsPointInRect(const QPoint& pt, const QRect& rect)
{
	return rect.contains(pt);
}

void NDropShadowWidget::Resize(const QRect& rectGeometry, const ECursorPosition& cursorPosition, const QPoint& mouseGlobalPos)
{
	QRect rectDesktop = QApplication::desktop()->contentsRect();
	QRect rectResult = rectGeometry;
	int nWidth = 0, nHeight = 0;
	int nX = 0, nY = 0;
	int nLeft, nTop, nRight, nBottom;
	int nIndient = 50;
	switch (cursorPosition)
	{
	case CursorPositionTop:
		rectResult.setTop(mouseGlobalPos.y());
		break;
	case CursorPositionBottom:
		rectResult.setBottom(mouseGlobalPos.y());
		break;
	case CursorPositionLeft:
		rectResult.setLeft(mouseGlobalPos.x());
		break;
	case CursorPositionRight:
		rectResult.setRight(mouseGlobalPos.x());
		break;
	case CursorPositionTopLeft:
		rectResult.setTop(mouseGlobalPos.y());
		rectResult.setLeft(mouseGlobalPos.x());
		break;
	case CursorPositionTopRight:
		rectResult.setTop(mouseGlobalPos.y());
		rectResult.setRight(mouseGlobalPos.x());
		break;
	case CursorPositionBottomLeft:
		rectResult.setLeft(mouseGlobalPos.x());
		rectResult.setBottom(mouseGlobalPos.y());
		break;
	case CursorPositionBottomRight:
		rectResult.setRight(mouseGlobalPos.x());
		rectResult.setBottom(mouseGlobalPos.y());
		break;
	default:
		rectResult = rectGeometry;
		break;
	}
	this->setGeometry(rectResult);
}
