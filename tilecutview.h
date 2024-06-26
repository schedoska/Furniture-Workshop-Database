#ifndef TILECUTVIEW_H
#define TILECUTVIEW_H

#include <QWidget>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QStandardItemModel>
#include <QPushButton>
#include <QLabel>

#include "cutter.h"

enum AreaType {TO_CUT, TO_REMOVE, NOT_USED};

class TileCutView : public QWidget
{
    Q_OBJECT
public:
    explicit TileCutView(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

    void settileSize(int height, int width);
    //void drawAreaList(QList)...

    void setTileData(Component_with_cuts cutData, QString title);

private:
    int m_tileWidthReal;
    int m_tileHeightReal;
    int m_tileBorderWidth;

    float m_tileOccupation;
    double m_scale; //jeden cm ile to pixeli
    QPoint m_tileLeftTopCorner;

    void drawAreaOnTile(QRectF rect, AreaType areaType, QPainter *painter);
    Component_with_cuts m_cutData;

    QLabel *titleLabel;

signals:

};

#endif // TILECUTVIEW_H
