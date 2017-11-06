/*  Copyright Olga Vysotska, year 2017.
**  In case of any problems with the code please contact me.
**  Email: olga.vysotska@uni-bonn.de
*/
#include "visualizer/visualizer.h"
#include <QPoint>
#include <QVBoxLayout>
#include <QtGui>
#include <iostream>
#include <vector>


Q_DECLARE_METATYPE(PathElement)
Q_DECLARE_METATYPE(Node)
Q_DECLARE_METATYPE(NodeSet)

Visualizer::Visualizer() {
  resize(WINDOW_WIDTH, WINDOW_HEIGHT);
  _mainLayout = new QGridLayout;
  setLayout(_mainLayout);
  show();
}

bool Visualizer::setLocalizationViewer(LocalizationViewer *locViewer) {
  if (!locViewer) {
    printf(
        "[ERROR][Visualizer] Localization Viewer is not set. Can't add to "
        "visualizer.\n");
    return false;
  }

  _locViewer = locViewer;
  _mainLayout->addWidget(_locViewer, 0, 0);
  setLayout(_mainLayout);
  show();

  qRegisterMetaType<std::vector<PathElement> >("Path");
  qRegisterMetaType<std::unordered_set<Node>>("Frontier");
  qRegisterMetaType<NodeSet>("Expansion");

  QObject::connect(
      this, SIGNAL(drawPath_signal(const std::vector<PathElement> &)),
      _locViewer, SLOT(receivedPath(const std::vector<PathElement> &)));

  QObject::connect(this, SIGNAL(drawFrontier_signal(const std::unordered_set<Node> &)),
                   _locViewer,
                   SLOT(receivedFrontier(const std::unordered_set<Node> &)));
  QObject::connect(this, SIGNAL(drawExpansion_signal(NodeSet)),
                   _locViewer, SLOT(receivedExpansion(NodeSet)));
  printf("[INFO][Visualizer] Localization Viewer is set\n");
  return true;
}

bool Visualizer::setMatchViewer(MatchViewer *matchViewer) {
  if (!matchViewer) {
    printf(
        "[ERROR][Visualizer] Match Viewer is not set. Can't add to "
        "visualizer.\n");
    return false;
  }
  _matchViewer = matchViewer;
  if (_matchViewer->init(WINDOW_WIDTH * 0.5, WINDOW_HEIGHT)) {
    _mainLayout->addWidget(_matchViewer, 0, 1);
    _mainLayout->setColumnMinimumWidth(0, WINDOW_WIDTH * 0.5);
    _mainLayout->setColumnMinimumWidth(1, WINDOW_WIDTH * 0.5);

    setLayout(_mainLayout);
    QObject::connect(this, SIGNAL(showPathImage(int, int, bool)),
                     _matchViewer, SLOT(receivedMatch(int, int, bool)));
    return true;
  }
  return false;
}

Visualizer::~Visualizer() {
  delete _mainLayout; 
}

void Visualizer::drawPath(const std::vector<PathElement> &path) {
  emit drawPath_signal(path);
  PathElement lastMatch = path.back();
  emit showPathImage(lastMatch.quId, lastMatch.refId,
                     lastMatch.state == HIDDEN);
}
void Visualizer::drawFrontier(const NodeSet &frontier) {
  emit drawFrontier_signal(frontier);
}

void Visualizer::drawExpansion(NodeSet expansion) {
  emit drawExpansion_signal(expansion);
}

bool Visualizer::isReady() const {
  if (!_locViewer) {
    printf("[WARNING] Visualizer is not ready\n");
    return false;
  }
  return true;
}
