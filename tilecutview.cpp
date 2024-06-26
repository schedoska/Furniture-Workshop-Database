#include "tilecutview.h"
#include <QDebug>

TileCutView::TileCutView(QWidget *parent)
    : QWidget{parent}
{
    m_tileOccupation = 0.8;

    m_tileBorderWidth = 1;

    titleLabel = new QLabel("tile", this);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    titleLabel->setGeometry(15, 10, 150, 100);

    settileSize(150, 150);
}

void TileCutView::paintEvent(QPaintEvent *event)
{
    m_scale = (height() - 40) / (double)m_tileHeightReal;
    m_tileLeftTopCorner = QPoint(qMax(width() / 2.0 - m_scale * m_tileWidthReal / 2.0, 200.0), 10);

    QPainter painter(this);
    QBrush brush;
    brush.setColor(QColor(194, 194, 194));
    brush.setStyle(Qt::Dense4Pattern);
    painter.setBrush(brush);
    QPen pen;
    pen.setWidth(m_tileBorderWidth);
    painter.setPen(pen);

    painter.drawRect(m_tileLeftTopCorner.x(), m_tileLeftTopCorner.y(),
                     m_tileWidthReal * m_scale, m_tileHeightReal * m_scale);
    painter.drawText(m_tileLeftTopCorner.x() + m_tileWidthReal * m_scale / 2.0,
                     m_tileLeftTopCorner.y() + m_tileHeightReal * m_scale + 20,
                     QString::number(m_tileWidthReal));
    painter.drawText(m_tileLeftTopCorner.x() - 36,
                     m_tileLeftTopCorner.y() + m_tileHeightReal * m_scale / 2.0,
                     QString::number(m_tileHeightReal));

    for(Rectangle cutAreaRect : m_cutData.elements){
        drawAreaOnTile(QRectF(cutAreaRect.pos_x, cutAreaRect.pos_y, cutAreaRect.width, cutAreaRect.height),
                       AreaType::TO_CUT,
                       &painter);
    }
    for(Rectangle cutAreaRect : m_cutData.leftovers){
        drawAreaOnTile(QRectF(cutAreaRect.pos_x, cutAreaRect.pos_y, cutAreaRect.width, cutAreaRect.height),
                       AreaType::NOT_USED,
                       &painter);
    }
    update();
}

QSize TileCutView::sizeHint() const
{
    return QSize(m_tileWidthReal * m_scale + 100, 150);
}

void TileCutView::settileSize(int width, int height)
{
    m_tileHeightReal = height;
    m_tileWidthReal = width;
}

void TileCutView::setTileData(Component_with_cuts cutData, QString title)
{
    m_cutData = cutData;
    titleLabel->setText(title);
    settileSize(cutData.component_outline.width, cutData.component_outline.height);
}

void TileCutView::drawAreaOnTile(QRectF rect, AreaType areaType, QPainter *painter)
{
    QPen pen;
    pen.setWidth(m_tileBorderWidth);
    QBrush brush;
    if(areaType == AreaType::TO_CUT){
        brush.setColor(QColor(3, 252, 94));
        brush.setStyle(Qt::SolidPattern);
    }
    else if(areaType == AreaType::TO_REMOVE){
        brush.setColor(QColor(227, 45, 9));
        brush.setStyle(Qt::Dense4Pattern);
    }
    else{
        brush.setColor(QColor(194, 194, 194));
        brush.setStyle(Qt::Dense4Pattern);
    }
    painter->setPen(pen);
    painter->setBrush(brush);

    painter->drawRect(m_tileLeftTopCorner.x() + rect.x() * m_scale,
                      m_tileLeftTopCorner.y() + rect.y() * m_scale,
                      rect.width() * m_scale,
                      rect.height() * m_scale);
    if(areaType == AreaType::TO_CUT || AreaType::NOT_USED){
        painter->drawText(m_tileLeftTopCorner.x() + rect.x() * m_scale + rect.width() * m_scale / 2.0,
                          m_tileLeftTopCorner.y() + rect.y() * m_scale + rect.height() * m_scale - 10,
                          QString::number(rect.width()));
        painter->drawText(m_tileLeftTopCorner.x() + rect.x() * m_scale + 10,
                          m_tileLeftTopCorner.y() + rect.y() * m_scale + rect.height() * m_scale / 2.0,
                          QString::number(rect.height()));
    }
}

