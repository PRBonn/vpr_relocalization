/* Copyright Olga Vysotska, year 2017.
** In case of any problems with the code please contact me.
** Email: olga.vysotska@uni-bonn.de.
*/

#ifndef SRC_VISUALIZER_MATCH_VIEWER_H_
#define SRC_VISUALIZER_MATCH_VIEWER_H_

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGridLayout>
#include <QtGui>
#include <string>

#include "database/online_database.h"
#include "online_localizer/online_localizer.h"

/**
 * @brief      Class for visualizing the expanded nodes and path.
 */
class MatchViewer : public QGraphicsView {
  Q_OBJECT

 public:
  using Ptr = std::shared_ptr<MatchViewer>;
  using ConstPtr = std::shared_ptr<const MatchViewer>;

  MatchViewer();
  ~MatchViewer();
  
  bool setDatabase(OnlineDatabase::Ptr database);
  void setQuImageDirectory(const std::string &folder);
  void setRefImageDirectory(const std::string &folder);
  void setImageExtension(const std::string &ext);

  bool isReady() const;
  bool init(int width, int height);

  void wheelEvent(QWheelEvent *event);

 public slots:
  void receivedMatch(int quId, int refId, bool hidden);

 private:
  std::string _refImagesFolder = "";
  std::string _quImagesFolder = "";
  std::string _imgExt = "";

  // variable for storing two single images
  QGraphicsPixmapItem *_ref_imgItem = nullptr;
  QGraphicsPixmapItem *_qu_imgItem = nullptr;
  QGraphicsTextItem *_textItem = nullptr;

  QGraphicsView _quImage, _refImage;
  QGraphicsScene *_quScene = nullptr;
  QGraphicsScene *_refScene = nullptr;
  QGridLayout *_layout = nullptr;
  OnlineDatabase::Ptr _database = nullptr;
};

#endif  // SRC_VISUALIZER_MATCH_VIEWER_H_
