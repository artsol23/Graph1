#ifndef SCENEPLOT_H
#define SCENEPLOT_H

#include <QDialog>

#include "qcustomplot.h"
#include <QPointF>
#include <QList>
namespace Ui {
   class ScenePlot;
   }

class ScenePlot : public QDialog
   {
   Q_OBJECT

   double currentTimeMs;
   QList<QColor> colorMap;
   QTimer *timer;

public:
   explicit ScenePlot(QWidget *parent = nullptr);
   ~ScenePlot();
   int n=1;
   int counter=1;
   void sendMass(QList<QPointF> &nPointmas);
   QList<QList<QPointF>*> *graphList;
   QList<QList<QList<QPointF*>*>*> tempGraphList;
    QList<QList<QPointF*>*> tempList;
public slots:
  void saveToFile();
  void logScalex();
  void logScaley();
  void updateData();
  void saveTextToFile();
  void getN();
  void clearAllGraphs();
  void timerControl();
  void sendTimer(QTimer *tempTimer);
  void sendGraphList(QList<QList<QPointF>*> &nGraphlist);
 void buildNGraphs();
 void buildAllGraphs();
 void setColors();
 void appendTempGraphList();
 void buildingGraphs();
private:
   Ui::ScenePlot *ui;
   };

#endif // SCENEPLOT_H
