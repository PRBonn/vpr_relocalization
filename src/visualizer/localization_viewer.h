/*  Copyright Olga Vysotska, year 2017.
**  In case of any problems with the code please contact me.
**  Email: olga.vysotska@uni-bonn.de
*/

#ifndef SRC_VISUALIZER_LOCALIZATION_VIEWER_H_
#define SRC_VISUALIZER_LOCALIZATION_VIEWER_H_

#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QPixmap>
#include <QtGui>

#include <set>
#include <vector>
#include <unordered_map>

#include "database/idatabase.h"
#include "online_localizer/online_localizer.h"
#include "successor_manager/node.h"

/**
 * @brief      Visualizes the expanded graph as well as current  best matching
 * hypothesis.
 */
class LocalizationViewer : public QGraphicsView {
  Q_OBJECT

 public:
  using Ptr = std::shared_ptr<LocalizationViewer>;
  using ConstPtr = std::shared_ptr<const LocalizationViewer>;

  LocalizationViewer();
  ~LocalizationViewer() {}
  void setSceneSize(qreal width, qreal height);

  /**
   * @brief      The costs for visualization are taken from the database.
   *
   * @param[in]  database  The database
   *
   * @return     { false, if database::Ptr is null}
   */
  bool setDatabase(iDatabase::Ptr database);

  void wheelEvent(QWheelEvent *event);
  void drawNode(float x, float y, const QPen &pen, bool filled);

 public slots:
  void receivedPath(const std::vector<PathElement> &path);
  void receivedFrontier(const std::unordered_set<Node> &frontier);
  void receivedExpansion(NodeSet expansion);
  void receivedLocalizationFinished();

 private:
  // std::vector<QGraphicsPolygonItem *> _pathItems;
  std::unordered_map<int, std::pair<int, QGraphicsItem*> > _path;
  // PriorityQueue _frontier;
  QGraphicsScene _scene;
  iDatabase::Ptr _database = nullptr;
  QGraphicsPixmapItem *_initScreen = nullptr;
  QPixmap _pixmap;
};

#endif  // SRC_VISUALIZER_LOCALIZATION_VIEWER_H_
