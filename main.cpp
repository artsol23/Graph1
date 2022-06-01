#include <QApplication>
#include <QCommandLineParser>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QStyle>
#include <QDesktopWidget>
#include <QDebug>
#include<QPointF>
#include <QTimer>
#include <QRandomGenerator>
#include "ScenePlot.h"

void updateMassive(QList<QPointF> *pointmas1);
int main(int argc, char *argv[])
   {
   QApplication a(argc, argv);

//   QCoreApplication::addLibraryPath("c:/Program Files/Swabian Instruments/TimeTagger2_7_6/driver/x86");
   QList<QPointF> pointmas1;
   //generation
   for (int i = 0; i < 1000; ++i) {
   pointmas1.append(QPointF(i,i));
  }
   QList<QList<QPointF>*> graphList;
   for (int i = 0; i < 4; ++i) {
       graphList.append(new QList<QPointF>());
   }

   QTimer *timer;
   timer = new QTimer();
   timer->setInterval(200);
   timer->start();
   QRandomGenerator generator;
   ScenePlot w = ScenePlot();
   w.sendTimer(timer);
   w.sendGraphList(graphList);
   QObject::connect(timer,&QTimer::timeout,[&graphList,&w,&generator](){
       for (int j = 0; j < graphList.count(); ++j) {
            graphList.at(j)->clear();
       for (int i = 0; i < 1000; ++i) {
           graphList.at(j)->append(QPointF(i,i+(generator.generate()%40)+j*100));
       }
       }

     w.buildNGraphs();
   });
   w.show();
   return a.exec();
   }
