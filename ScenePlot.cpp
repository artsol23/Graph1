#include "ScenePlot.h"
#include "ui_ScenePlot.h"
#include <qcustomplot.h>
#include <QFile>
#include <QPushButton>
#include <QTextStream>
#include<QDebug>
#include <QPoint>
ScenePlot::ScenePlot(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::ScenePlot)
   {
   ui->setupUi(this);


   currentTimeMs = QDateTime(QDate(2022,1,1),QTime(0,0,0)).msecsTo(QDateTime::currentDateTime());

   createColorMap();
//   updatePathLinks();
   //   ui->progressBar->setMaximum(ui->SB_ValuesCountTreshold->value()*1000);
   ui->customPlot->xAxis->setVisible(false);
   ui->customPlot->xAxis->setTickLabels(true);
   ui->customPlot->yAxis->setVisible(false);
   ui->customPlot->yAxis->setTickLabels(true);

   ui->customPlot->yAxis->setRange(0, 1000);
   ui->customPlot->xAxis->setRange(-1000 , 1000);

   ui->customPlot->setBackground(QBrush(QColor("#FFFFFF")));
   ui->customPlot->xAxis->setLabel("t (s)");
   ui->customPlot->yAxis->setLabel("counts");
   ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
   ui->customPlot->axisRect()->setupFullAxesBox(true);
   timer = new QTimer();
   connect(timer,&QTimer::timeout,this,&ScenePlot::updateData);
   connect(ui->save,&QPushButton::clicked,this,&ScenePlot::saveToFile);
   ui->xlog->setCheckable(true);
   ui->ylog->setCheckable(true);
   ui->xlog->setText("xLin");
   ui->ylog->setText("yLin");
   connect(ui->xlog,&QPushButton::clicked,this,&ScenePlot::logScalex);
   connect(ui->ylog,&QPushButton::clicked,this,&ScenePlot::logScaley);
   timer->setInterval(100);
   timer->start();
   show();


   //   connect(ui->PB_SelectFolder,&QPushButton::clicked,this,&CountratePlot::callOpenFolderDialog);
   //   connect(this, &CountratePlot::savePathChanged, this, &CountratePlot::updatePathLinks);
   //   connect(ui->SB_ValuesCountTreshold,qOverload<int>(&QSpinBox::valueChanged), this, &CountratePlot::updateTreshold);
   //   connect(ui->PB_StartStopRec,&QPushButton::toggled,this,&CountratePlot::startStopRecordPressed);
   //   connect(ui->PB_NextFile,&QPushButton::clicked,this,&CountratePlot::closeFile);
   //   connect(storage,&GraphStorage::rowAppended,this,&CountratePlot::createNewFile, Qt::DirectConnection);
   //   connect(storage,&GraphStorage::writedDataCountChanged, ui->progressBar, &QProgressBar::setValue);
   //   connect(ui->customPlot,&QCustomPlot::mouseWheel,this,&CountratePlot::plotWheelEvent);
   //   sBar = new QStatusBar(this);
   //   ui->gridLayout_2->addWidget(sBar);
   }

ScenePlot::~ScenePlot()
   {
   delete ui;
   }

void ScenePlot::createColorMap()
   {
   QColor tempColor = QColor::fromHsl(0,170,170);
   for(int i = 0; i< 81; i++){
      tempColor.setHsl((tempColor.hue()+40)%360,tempColor.hslSaturation(),tempColor.lightness());
      if (tempColor.hslHue() == 0)
         tempColor = tempColor.darker(100);
      colorMap.append(tempColor);
      }
   }

void ScenePlot::updateData()
   {
   ui->customPlot->clearGraphs();
   ui->customPlot->addGraph();
   for(int i = ui->customPlot->xAxis->range().lower; i < ui->customPlot->xAxis->range().upper; i++)
      ui->customPlot->graph(0)->addData(i,(1000./i)*(QRandomGenerator::global()->generate()%1000));
   ui->customPlot->yAxis->rescale(true);
   ui->customPlot->replot();
   }
void ScenePlot::saveToFile(){
    QFile file("graph1.jpg");
     ui->customPlot->saveJpg("graph1.jpg");
}
void ScenePlot::logScalex(){
    if(! ui->xlog->isChecked()){
    ui->customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->xlog->setText("xLog");
     ui->customPlot->replot();
    }
    else{
        ui->customPlot->xAxis->setScaleType(QCPAxis::stLinear);
        ui->xlog->setText("xLin");
        ui->customPlot->replot();
    }

 }
void ScenePlot::logScaley(){
    if(! ui->ylog->isChecked()){
    ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
     ui->ylog->setText("yLog");
     ui->customPlot->replot();

    }
    else{
        ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
         ui->ylog->setText("yLin");
        ui->customPlot->replot();
    }

 }
void ScenePlot::updateDataMas(){
   //генерация массива
    for (int i = 0; i < 1000; ++i) {
    pointmas.append(QPointF(i,(1000./i)*(QRandomGenerator::global()->generate()%1000)));
   }

    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    QPoint point;
    for (int i = 0; i < pointmas.count(); ++i) {
       point=pointmas.at(i).toPoint();
        ui->customPlot->graph(0)->addData(point.x(),point.y());
     ui->customPlot->yAxis->rescale(true);
     ui->customPlot->replot();
    }

}

