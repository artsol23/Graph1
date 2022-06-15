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
#include<QTextCursor>

#include <QInputDialog>


ScenePlot::ScenePlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScenePlot)
{
    ui->setupUi(this);


    currentTimeMs = QDateTime(QDate(2022,1,1),QTime(0,0,0)).msecsTo(QDateTime::currentDateTime());

    //   updatePathLinks();
    //   ui->progressBar->setMaximum(ui->SB_ValuesCountTreshold->value()*1000);
    ui->customPlot->xAxis->setVisible(false);
    ui->customPlot->xAxis->setTickLabels(true);
    ui->customPlot->yAxis->setVisible(false);
    ui->customPlot->yAxis->setTickLabels(true);

    connect(ui->clearAllData,&QPushButton::clicked,this,clearAllData);
    ui->customPlot->yAxis->setRange(0, 1000);
    ui->customPlot->xAxis->setRange(-1000 , 1000);
    ui->customPlot->setBackground(QBrush(QColor("#FFFFFF")));
    ui->customPlot->xAxis->setLabel("t (s)");
    ui->customPlot->yAxis->setLabel("counts");
    ui->customPlot->axisRect()->setupFullAxesBox(true);


    ui->stop->setEnabled(false);
    ui->start->setCheckable(true);
    ui->stop->setCheckable(true);
    ui->customPlot->legend->setVisible(true);


    connect(ui->start, &QPushButton::clicked, [=](){
        timer->start();
        ui->stop->setEnabled(true);
        ui->start->setEnabled(false);
    });
    connect(ui->stop, &QPushButton::clicked, [=](){
        timer->stop();
        ui->start->setEnabled(true);
        ui->stop->setEnabled(false);
    });

    show();

    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->customPlot, &QCustomPlot::customContextMenuRequested,
            [=](QPoint position){

        auto *menu = new QMenu(this);
        menu->setAttribute(Qt::WA_DeleteOnClose);

        auto * channels = menu->addMenu("Channels");
        for (int i = 0; i<graphList->count(); i++){
            QCheckBox *checkBox = new QCheckBox(QString("Channel %1").arg(i), channels);
            if(ui->customPlot->graphCount()!=0)
                checkBox->setChecked(ui->customPlot->graph(i)->visible());

            QWidgetAction *checkableAction = new QWidgetAction(channels);
            checkableAction->setDefaultWidget(checkBox);
            channels->addAction(checkableAction);
            if(ui->customPlot->graphCount()!=0)
                connect(checkBox, &QCheckBox::stateChanged, [=](int value){
                    int index = checkBox->text().split(" ")[1].toInt();
                    ui->customPlot->graph(index)->setVisible(value);
                    if (value) ui->customPlot->graph(index)->addToLegend(ui->customPlot->legend);
                    else ui->customPlot->graph(index)->removeFromLegend(ui->customPlot->legend);
                    ui->customPlot->legend->setVisible(ui->customPlot->legend->elementCount() != 0);
                    ui->customPlot->replot();
                });
        }




        auto *logScale = menu->addAction("LogScale",[=](){
            if(ui->customPlot->yAxis->scaleType() != QCPAxis::stLogarithmic){
                ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
                ui->customPlot->replot();
            }
            else{
                ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
                ui->customPlot->replot();
            }
        });
        logScale->setCheckable(true);
        logScale->setChecked(ui->customPlot->yAxis->scaleType() == QCPAxis::stLogarithmic);
        auto *reScale=menu->addAction("rescaling",[=](){
            if (rescaleCounter)
                rescaleCounter=false;
            else
                rescaleCounter=true;


        });
        reScale->setCheckable(true);
        reScale->setChecked(rescaleCounter);
        auto *plottingAll=menu->addAction("plottingAllgraphs",[=](){
            if (formatCounter){
                formatCounter=false;

                counter=1;
                clearTemplist();
                clearTempGraphList();


            }
            else{

                formatCounter=true;

                clearTemplist();
                counter=1;
                n=1;
            }
        });
        plottingAll->setCheckable(true);
        plottingAll->setChecked(!formatCounter);
        auto *getN=menu->addAction("current n = "+QString::number(n),[=](){
            n = QInputDialog::getInt(this,"Input n","label",1,1,20);
            clearTemplist();
            clearTempGraphList();
            ui->customPlot->clearGraphs();
            ui->customPlot->yAxis->setRange(0, 1000);
            ui->customPlot->xAxis->setRange(-1000 , 1000);
            ui->customPlot->replot();
            counter=1;


        });
        getN->setEnabled(formatCounter);


        menu->addAction("Save Data...",[=](){
            saveTextToFile();
        });
        menu->addAction("Save Graph As a Screenshot...",[=](){
            saveToFile();
        });


        menu->popup(ui->customPlot->mapToGlobal(position));
    });

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
            for (int j = 0; j < graphList->count(); ++j) {
                float x=graphList->at(j)->at(i).x();
                float y=graphList->at(j)->at(i).y();
                if (j==0)
                    out<<x<<";";
                out<<y<<";";



            }
            out<<"\n";
        }
    }
}

void ScenePlot::clearAllData()
{
    clearTemplist();
    if(ui->customPlot->graphCount()<graphList->count()){
        counter=1;
        clearTemplist();
        clearTempGraphList();
    }
    for (int j = ui->customPlot->graphCount(); j < graphList->count(); ++j) {
        ui->customPlot->addGraph();

    }

    for (int j = 0; j < graphList->count(); ++j) {
        ui->customPlot->graph(j)->data().data()->clear();
    }
    ui->customPlot->yAxis->setRange(0, 1000);
    ui->customPlot->xAxis->setRange(-1000 , 1000);
    ui->customPlot->replot();
    counter=1;
    clearTempGraphList();
}

void ScenePlot::sendTimer(QTimer *tempTimer){
    timer=tempTimer;
    timer->stop();
}

void ScenePlot::sendGraphList(QList<QList<QPointF> *> &nGraphlist)
{
    graphList=&nGraphlist;


}
void ScenePlot::buildNGraphs()
{
    if(graphList==nullptr)
        return;
    if(ui->customPlot->graphCount()<graphList->count()){
        counter=1;
        clearTemplist();
        clearTempGraphList();
        for (int j = ui->customPlot->graphCount(); j < graphList->count(); ++j) {
            ui->customPlot->addGraph();

        }


    }
    if (counter<=n){
        if (counter==1){
            for (int j = 0; j < graphList->count(); ++j) {
                tempList.append(new QList<QPointF*>);
                for (int i = 0; i < graphList->at(j)->count(); ++i) {
                    float y,x;
                    x=graphList->at(j)->at(i).x();
                    y=graphList->at(j)->at(i).y();
                    tempList.at(j)->append(new QPointF(x,y));
                }

            }
        }
        else{
            for (int j = 0; j < graphList->count(); ++j) {
                for (int i = 0; i < graphList->at(j)->count(); ++i) {
                    float y,x;
                    x=graphList->at(j)->at(i).x();
                    y=graphList->at(j)->at(i).y();
                    *( tempList.at(j)->at(i))+=QPointF(x,y);
                }

            }
        }
        appendTempGraphList();
        counter++;
    }
    else {

        for (int j = 0; j < graphList->count(); ++j) {
            for (int i = 0; i < graphList->at(j)->count(); ++i) {
                *(tempList.at(j)->at(i))+=graphList->at(j)->at(i);
                *(tempList.at(j)->at(i))-= (*(tempGraphList.at(0)->at(j)->at(i)));

            }
        }

        for (int z = 0; z < n-1; ++z) {
            for (int j = 0; j <graphList->count(); ++j) {
                for (int i = 0; i < graphList->at(j)->count(); ++i) {
                    *(tempGraphList.at(z)->at(j)->at(i))=*(tempGraphList.at(z+1)->at(j)->at(i));
                }
            }
        }


        for (int j = 0; j <graphList->count(); ++j) {
            for (int i = 0; i < graphList->at(j)->count(); ++i) {
                *(tempGraphList.at(tempGraphList.count()-1)->at(j)->at(i))=graphList->at(j)->at(i);
            }
        }

        buildingGraphs();
    }

}


void ScenePlot::buildAllGraphs()
{
    if(graphList==nullptr)
        return;
    if(ui->customPlot->graphCount()<graphList->count()){
        counter=1;
        clearTemplist();

        for (int j = ui->customPlot->graphCount(); j < graphList->count(); ++j) {
            ui->customPlot->addGraph();

        }
    }
    if (counter==1){

        for (int j = 0; j < graphList->count(); ++j) {
            tempList.append(new QList<QPointF*>);
            for (int i = 0; i < graphList->at(j)->count(); ++i) {
                float y=0,x;
                y=graphList->at(j)->at(i).y();
                x=graphList->at(j)->at(i).x();
                tempList.at(j)->append(new QPointF());
                *(tempList.at(j)->at(i))=QPointF(x,y);
            }

        }
        counter=2;
    }
    else{
        for (int j = 0; j < graphList->count(); ++j) {
            float y=0,x;
            for (int i = 0; i < graphList->at(j)->count(); ++i) {
                y=graphList->at(j)->at(i).y();
                y+=tempList.at(j)->at(i)->y();
                x=graphList->at(j)->at(i).x();
                *(tempList.at(j)->at(i))=QPointF(x,y);
            }

        }
    }

    buildingGraphs();
}

void ScenePlot::setColors()
{
    const int maxH = 359;
    int k=0;
    int step = maxH /ui->customPlot->graphCount();
    for (int i = 0; i <= maxH-step; i += step) {
        QPen pen;
        pen.setColor(QColor::fromHsl(i, 255, 127));
        ui->customPlot->graph(k)->setPen(pen);
        k++;
    }
}

void ScenePlot::appendTempGraphList()
{
    tempGraphList.append(new QList<QList<QPointF*>*>);
    for (int j = 0; j < graphList->count(); ++j) {
        tempGraphList.at(tempGraphList.count()-1)->append(new QList<QPointF*>);
        for (int i = 0; i < graphList->at(j)->count(); ++i) {
            int x=graphList->at(j)->at(i).x();
            int  y=graphList->at(j)->at(i).y();
            tempGraphList.at(tempGraphList.count()-1)->at(j)->append(new QPointF(x,y));

        }
    }
}

void ScenePlot::buildingGraphs()
{

    QPoint point;

    for (int j = 0; j < tempList.count(); ++j) {
        ui->customPlot->graph(j)->data().data()->clear();
        for (int i = 0; i < tempList.at(j)->count(); ++i) {

            point=tempList.at(j)->at(i)->toPoint();
            ui->customPlot->graph(j)->addData(point.x(),point.y());

        }

    }
    setColors();
    if(rescaleCounter==true){
        ui->customPlot->yAxis->rescale(true);
        ui->customPlot->xAxis->rescale(true);
    }
    ui->customPlot->setInteraction(QCP::iRangeZoom,true);
    ui->customPlot->setInteraction(QCP::iRangeDrag,true);
    ui->customPlot->replot();
    for (int j = 0; j < graphList->count(); ++j) {

        ui->customPlot->graph(j)->data().clear();
    }
}

void ScenePlot::connector()
{
    if(formatCounter==true){

        buildNGraphs();
    }
    else{

        buildAllGraphs();

    }

}

void ScenePlot::clearTemplist()
{
    for (int j = 0; j < tempList.count(); ++j) {

        qDeleteAll(tempList.at(j)->begin(),tempList.at(j)->end());
        tempList[j]->clear();
    }

    qDeleteAll(tempList.begin(),tempList.end());
    tempList.clear();
}

void ScenePlot::clearTempGraphList()
{
    for (int k = 0; k < tempGraphList.count(); ++k) {
        for (int j = 0; j < tempGraphList.at(k)->count(); ++j) {

            qDeleteAll(tempGraphList.at(k)->at(j)->begin(),tempGraphList.at(k)->at(j)->end());
            tempGraphList.at(k)->at(j)->clear();

        }
    }
    for (int k = 0; k < tempGraphList.count(); ++k) {


        qDeleteAll(tempGraphList.at(k)->begin(),tempGraphList.at(k)->end());
        tempGraphList.at(k)->clear();


    }
    qDeleteAll(tempGraphList.begin(),tempGraphList.end());
    tempGraphList.clear();
}





