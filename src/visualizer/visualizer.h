/*  Copyright Olga Vysotska, year 2017.
**  In case of any problems with the code please contact me.
**  Email: olga.vysotska@uni-bonn.de
*/
#ifndef SRC_VISUALIZER_VISUALIZER_H_
#define SRC_VISUALIZER_VISUALIZER_H_

#include <QGridLayout>
#include <QWidget>
#include <set>
#include <string>
#include <memory>
#include <vector>

#include "database/online_database.h"
#include "online_localizer/ilocvisualizer.h"
#include "visualizer/localization_viewer.h"
#include "visualizer/match_viewer.h"

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

/**
 * @brief      Main window for visualization.
 */
class Visualizer : public QWidget, public iLocVisualizer {
  Q_OBJECT

 public:
  using Ptr = std::shared_ptr<Visualizer>;
  Visualizer();
  ~Visualizer();

  bool setMatchViewer(MatchViewer *matchViewer);
  bool setLocalizationViewer(LocalizationViewer *loc_viewer);

  void drawPath(const std::vector<PathElement> &path) override;
  void drawFrontier(const std::unordered_set<Node> &frontier) override;
  void drawExpansion(NodeSet expansion) override;
  void processFinished() override {}

  bool isReady() const;

 signals:
  void drawPath_signal(const std::vector<PathElement> &path);
  void showPathImage(int quId, int refId, bool hidden);
  void drawFrontier_signal(const std::unordered_set<Node> &frontier);
  void drawExpansion_signal(const NodeSet &expansion);

 private:
  void setSignalSlots();
  LocalizationViewer *_locViewer = nullptr;
  MatchViewer *_matchViewer = nullptr;
  QGridLayout *_mainLayout = nullptr;
};

#endif  // SRC_VISUALIZER_VISUALIZER_H_
