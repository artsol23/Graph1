#include "ScenePlot.h"
#include "ui_ScenePlot.h"
#include <qcustomplot.h>
#include <QFile>
#include <QPushButton>
#include <QTextStream>
#include <QDebug>
#include <QPoint>
#include <QFileDialog>
#include <QString>
#include<QDateTime>
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
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    connect(ui->save,&QPushButton::clicked,this,&ScenePlot::saveTextToFile);
    connect(ui->savegraph,&QPushButton::clicked,this,&ScenePlot::saveToFile);
    connect(ui->number_of_graphs,QOverload<int>::of(&QSpinBox::valueChanged),this,&ScenePlot::getN);
    ui->plotting5graphs->setCheckable(true);
    ui->plotting5graphs->setText("1graphPlotting");
    ui->plotAllMassive->setText("1Massive");
    ui->plotAllMassive->setCheckable(true);
    ui->xlog->setCheckable(true);
    ui->ylog->setCheckable(true);
    ui->stopPlotting->setCheckable(true);
    ui->xlog->setText("xLin");
    ui->ylog->setText("yLin");
    connect(ui->xlog,&QPushButton::clicked,this,&ScenePlot::logScalex);
    connect(ui->ylog,&QPushButton::clicked,this,&ScenePlot::logScaley);
    connect(ui->stopPlotting,&QPushButton::clicked,this,&ScenePlot::timerControl);
    show();
    connect(ui->clearData,&QPushButton::clicked,this,&ScenePlot::clearAllGraphs);

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

void ScenePlot::saveToFile(){
    QString name,date,temp2;
    QString format=".jpg";
    QDateTime temp;
    temp.currentDateTime();
    date= QDateTime::currentDateTime().toString(Qt::ISODate);
    date="graph"+date;
    temp2=date.replace(":","_");
    QString selFilter="All files (*.*)";
    name= QFileDialog::getSaveFileName(this,"Save file",temp2,
                                       "JPG files (*.jpg);;All files (*.*)",&selFilter);
    if(!name.isEmpty()){
        if(!name.contains(format)){
            name+=format;
        }
        ui->customPlot->saveJpg(name);
    }

}
void ScenePlot::logScalex(){
    if( ui->xlog->isChecked()){
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
    if( ui->ylog->isChecked()){
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
void ScenePlot::updateData(){
    ui->customPlot->clearGraphs();
    QPoint point;

    if(graphList==nullptr)
        return;
    for (int j = 0; j < graphList->count(); ++j) {
        ui->customPlot->addGraph();
        for (int i = 0; i < graphList->at(j)->count(); ++i) {
            point=graphList->at(j)->at(i).toPoint();
            ui->customPlot->graph(j)->addData(point.x(),point.y());

        }
    }
    const int maxH = 359;
    int k=0;
    int step = maxH /graphList->count();
    for (int i = 0; i <= maxH-step; i += step) {
        QPen pen;
        pen.setColor(QColor::fromHsl(i, 255, 127));
     ui->customPlot->graph(k)->setPen(pen);
             k++;
    }

    ui->customPlot->yAxis->rescale(true);
    ui->customPlot->xAxis->rescale(true);
    ui->customPlot->setInteraction(QCP::iRangeZoom,true);
    ui->customPlot->setInteraction(QCP::iRangeDrag,true);
    ui->customPlot->replot();

}
void ScenePlot::saveTextToFile(){
    QString name,date,temp2;
    QString format=".txt";
    QDateTime temp;
    temp.currentDateTime();
    date= QDateTime::currentDateTime().toString(Qt::ISODate);
    date="data"+date;
    temp2=date.replace(":","_");
    QString selFilter="All files (*.*)";
    name= QFileDialog::getSaveFileName(this,"Save file",temp2,
                                       "CSV files (*.txt);;All files (*.*)",&selFilter);
    if(!name.isEmpty()){
        if(!name.contains(format)){
            name+=format;
        }
        QFile file=QFile(name);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        for (int i = 0; i < graphList->at(0)->count(); ++i) {
            float x=graphList->at(0)->at(i).x();
            float y=graphList->at(0)->at(i).y();
            out<<x<<";"<<y<<";"<<"\n";
        }
    }
}

void ScenePlot::getN(){
    n=ui->number_of_graphs->value();
}

void ScenePlot::clearAllGraphs()
{
tempGraphList.clear();
}

void ScenePlot::timerControl(){
    if( ui->stopPlotting->isChecked()){
        timer->stop();
    }
    else{
        timer->start();
    }
}
void ScenePlot::sendTimer(QTimer *tempTimer){
    timer=tempTimer;
}

void ScenePlot::sendGraphList(QList<QList<QPointF> *> &nGraphlist)
{
    graphList=&nGraphlist;

}
void ScenePlot::buildNGraphs()
{
    if(graphList==nullptr)
        return;
    QList<QList<QPointF>*> tempList;
    tempGraphList.append(new QList<QList<QPointF>*>);
             for (int j = 0; j < graphList->count(); ++j) {

             for (int i = 0; i < graphList->at(j)->count(); ++i) {
               int x=graphList->at(j)->at(i).x();
              int  y=graphList->at(j)->at(i).y();
                tempGraphList.at(tempGraphList.count()-1)->append(new QList<QPointF>);
                tempGraphList.at(tempGraphList.count()-1)->at(j)->append(QPointF(x,y));

             }
         }
         if (tempGraphList.count()<=n){
             updateData();
         }
         else{
        for (int j = 0; j < graphList->count(); ++j) {
             tempList.append(new QList<QPointF>);
            for (int i = 0; i < graphList->at(j)->count(); ++i) {
                QPointF tempPoint=QPointF(0,0);
                for (int k = 0; k < n; ++k) {
                 tempPoint+=tempGraphList.at(tempGraphList.count()-1-k)->at(j)->at(i);
                }
             tempList.at(j)->append(tempPoint);
            }

        }
        tempGraphList.removeFirst();
        qDebug()<<tempGraphList.count();
        ui->customPlot->clearGraphs();
        QPoint point;
        for (int j = 0; j < tempList.count(); ++j) {
            ui->customPlot->addGraph();
            for (int i = 0; i < tempList.at(j)->count(); ++i) {
                point=tempList.at(j)->at(i).toPoint();
                ui->customPlot->graph(j)->addData(point.x(),point.y());

            }
        }
        const int maxH = 359;
        int k=0;
        int step = maxH /graphList->count();
        for (int i = 0; i <= maxH-step; i += step) {
            QPen pen;
            pen.setColor(QColor::fromHsl(i, 255, 127));
         ui->customPlot->graph(k)->setPen(pen);
                 k++;
        }

        ui->customPlot->yAxis->rescale(true);
        ui->customPlot->xAxis->rescale(true);
        ui->customPlot->setInteraction(QCP::iRangeZoom,true);
        ui->customPlot->setInteraction(QCP::iRangeDrag,true);
        ui->customPlot->replot();
         }
}

void ScenePlot::buildAllGraphs()
{
    if(graphList==nullptr)
        return;
    QList<QList<QPointF>*> tempList;
    tempGraphList.append(new QList<QList<QPointF>*>);
             for (int j = 0; j < graphList->count(); ++j) {

             for (int i = 0; i < graphList->at(j)->count(); ++i) {
               int x=graphList->at(j)->at(i).x();
              int  y=graphList->at(j)->at(i).y();
                tempGraphList.at(tempGraphList.count()-1)->append(new QList<QPointF>);
                tempGraphList.at(tempGraphList.count()-1)->at(j)->append(QPointF(x,y));

             }
         }
             for (int j = 0; j < graphList->count(); ++j) {
                  tempList.append(new QList<QPointF>);
                 for (int i = 0; i < graphList->at(j)->count(); ++i) {
                     float y=0,x;
                     for (int k = 0; k < tempGraphList.count(); ++k) {
                      y+=tempGraphList.at(tempGraphList.count()-1-k)->at(j)->at(i).y();
                     }
                     x=tempGraphList.at(tempGraphList.count()-1)->at(j)->at(i).x();
                  tempList.at(j)->append(QPointF(x,y));
                 }

             }
             ui->customPlot->clearGraphs();
             QPoint point;
             for (int j = 0; j < tempList.count(); ++j) {
                 ui->customPlot->addGraph();
                 for (int i = 0; i < tempList.at(j)->count(); ++i) {
                     point=tempList.at(j)->at(i).toPoint();
                     ui->customPlot->graph(j)->addData(point.x(),point.y());

                 }
             }
             const int maxH = 359;
             int k=0;
             int step = maxH /graphList->count();
             for (int i = 0; i <= maxH-step; i += step) {
                 QPen pen;
                 pen.setColor(QColor::fromHsl(i, 255, 127));
              ui->customPlot->graph(k)->setPen(pen);
                      k++;
             }
             ui->customPlot->yAxis->rescale(true);
             ui->customPlot->xAxis->rescale(true);
             ui->customPlot->setInteraction(QCP::iRangeZoom,true);
             ui->customPlot->setInteraction(QCP::iRangeDrag,true);
             ui->customPlot->replot();
}
