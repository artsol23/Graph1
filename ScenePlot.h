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
   QList<QPointF> pointmas;
   void createColorMap();

public slots:
   void updateData();
   void saveToFile();
   void logScalex();
   void logScaley();
  void updateDataMas();
private:
   Ui::ScenePlot *ui;
   };

#endif // SCENEPLOT_H
