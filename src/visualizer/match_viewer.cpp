/* Copyright Olga Vysotska, year 2017.
** In case of any problems with the code please contact me.
** Email: olga.vysotska@uni-bonn.de.
*/

#include "visualizer/match_viewer.h"

#include <QDebug>
#include <QScrollBar>
#include <string>
#include <vector>
#include <database/list_dir.h>

MatchViewer::MatchViewer() {}

bool MatchViewer::init(int width, int height) {
  if (_queryImages.empty() || _refImages.empty()) {
    printf("[ERROR][MatchViewer] Images were not set\n");
    return false;
  }
  this->setDragMode(QGraphicsView::ScrollHandDrag);
  this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  // this->setScene(&_scene);
  // QGraphicsView::fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
  float img_height = height * 0.45;
  float img_width = img_height * 16 / 9;
  _quScene = new QGraphicsScene(0.0, 0.0, img_width, img_height);
  _refScene = new QGraphicsScene(0.0, 0.0, img_width, img_height);

  _quImage.setScene(_quScene);
  _refImage.setScene(_refScene);

  _quImage.setDragMode(QGraphicsView::ScrollHandDrag);

  _refImage.setDragMode(QGraphicsView::ScrollHandDrag);
  _refImage.setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  _textItem = _refImage.scene()->addText("No Match");
  _textItem->setVisible(false);
  _textItem->setDefaultTextColor(QColor(255, 255, 255));
  _textItem->setPos(_refImage.scene()->width() / 2,
                    _refImage.scene()->height() / 2);

  _layout = new QGridLayout;
  _layout->addWidget(&_quImage, 0, 0);
  _layout->addWidget(&_refImage, 1, 0);

  this->setLayout(_layout);
  return true;
}

MatchViewer::~MatchViewer() {
  if (_layout) {
    delete _layout;
  }
  if (_refScene) {
    delete _refScene;
  }
  if (_quScene) {
    delete _quScene;
  }
}

bool MatchViewer::setDatabase(OnlineDatabase::Ptr database) {
  if (!database) {
    printf("[ERROR][PathViewer] The database is not set\n");
    return false;
  }
  _database = database;
  return true;
}

void MatchViewer::wheelEvent(QWheelEvent *event) {
  const QPointF p0scene = mapToScene(event->pos());

  qreal factor = std::pow(1.001, event->delta());
  scale(factor, factor);

  const QPointF p1mouse = mapFromScene(p0scene);
  const QPointF move = p1mouse - event->pos();  // The move
  horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
  verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
}

void MatchViewer::receivedMatch(int quId, int refId, bool hidden) {
  // printf("Received match %d %d \n", quId, refId);
  std::string qu_path, ref_path;
  qu_path = _queryImages[quId];

  QPixmap img_qu;
  img_qu.load(qu_path.c_str());
  // img_qu = img_qu.scaledToHeight(_quImage.scene()->height());
  img_qu = img_qu.scaled(_quImage.scene()->width(), _quImage.scene()->height());
  if (!_qu_imgItem) {
    _qu_imgItem = _quImage.scene()->addPixmap(img_qu);
  } else {
    _qu_imgItem->setPixmap(img_qu);
  }

  if (hidden) {
    // make black pixmap;
    QPixmap empty_img(_quImage.scene()->width(), _quImage.scene()->height());
    empty_img.fill(Qt::black);
    if (!_ref_imgItem) {
      _ref_imgItem = _refImage.scene()->addPixmap(empty_img);
    } else {
      _ref_imgItem->setPixmap(empty_img);
    }
    _textItem->setVisible(true);
    _textItem->setZValue(1);
  } else {
    _textItem->setVisible(false);

    ref_path = _refImages[refId];

    QPixmap img_ref;
    // printf("[MatchViewer] reading image %s\n", ref_path.c_str());
    img_ref.load(ref_path.c_str());
    // img_ref = img_ref.scaledToHeight(_refImage.scene()->height());
    img_ref =
        img_ref.scaled(_refImage.scene()->width(), _refImage.scene()->height());

    if (!_ref_imgItem) {
      _ref_imgItem = _refImage.scene()->addPixmap(img_ref);
    } else {
      _ref_imgItem->setPixmap(img_ref);
    }
  }
}

bool MatchViewer::isReady() const {
  if (!_database) {
    printf("[ERROR][MatchViewer] Database is not set\n");
    return false;
  }
  if (_queryImages.empty()) {
    printf("[ERROR][MatchViewer] Folder for query images is not set\n");
    return false;
  }
  if (_refImages.empty()) {
    printf("[ERROR][MatchViewer] Folder for reference images is not set\n");
    return false;
  }
  // # extension. Maybe also works without it
  // if (_ext.empty()) {
  //   printf("[ERROR][MatchViewer] Folder for reference images is not set\n");
  //   return false;
  // }
  return true;
}

void MatchViewer::setQueryImages(const std::string &folder) {
  _queryImages = listDir(folder);
}
void MatchViewer::setRefImages(const std::string &folder) {
  _refImages = listDir(folder);
}
void MatchViewer::setImageExtension(const std::string &ext) { _imgExt = ext; }
