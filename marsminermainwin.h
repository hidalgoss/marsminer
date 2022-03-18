#ifndef MARSMINERMAINWIN_H
#define MARSMINERMAINWIN_H

#include <QMainWindow>
#include "MarsMiner.h"          // Mi MarsMiner #include.
#include <QFileDialog>
#include <QDebug>
#include <QStandardItemModel>  // para la creacion del tableView con  QStandardItemModel
#include <QTableView>
#include <QtCore/qmath.h>
#include <QProgressBar>

//+-------------------------------------------------------------------------------------
//+-------------------------------------------------------------------------------------
namespace Ui {
class MarsMinerMainWin;
}

//+-------------------------------------------------------------------------------------
//+-------------------------------------------------------------------------------------
class MarsMinerMainWin : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MarsMinerMainWin(QWidget *parent = 0);
    ~MarsMinerMainWin();

    void loadDataSetInTableView(clsMarsMiner *MM, QTableView *tableView);
    void loadDistanceInTableView(clsMarsMiner *MMsu, clsMarsMiner *MMds,
                                 QTableView *tableView);
    double distanceNormal(float fX, float fY, float fMax, float fMin);
    double distance(float fX, float fY);
    void loadDistHeadInTableView(clsMarsMiner *MMsu, clsMarsMiner *MMds,
                                 QTableView *tableView);
    void loadDistHeadInTableViewWithMask(clsMarsMiner *MMsu, clsMarsMiner *MMds,
                                         QTableView *tableView, int *piMask);
    //void OpenFile(int iDataSet);
    int feasibleDistance(clsMarsMiner *MM, clsMarsMiner *MMcls,
                         int iIndividuo, int iClass, float fDistance);
    void insertaDistanceAndClass(clsMarsMiner *MM, clsMarsMiner *MMcls, int iClass,
                                 int iRow, int iStartCol, float fDistance);
    void determineDangerousity(clsMarsMiner *MM, clsMarsMiner *MMcls,
                               int iColResult, int iColProb, int iRow);
    bool verificaClass(clsMarsMiner *MMds, clsMarsMiner *MMsu, int filSu);
    float K_NN_WithMask(int *piFijados, clsMarsMiner *MMdst, clsMarsMiner *MMcls);
    float GreedyIterative(int *piFijados, int iTotalAttributes);
    float GreedyIterativeAuto(int *piFijados, int iTotalAttributes);
    void InicializaArray(int *piArray, int iNumElementos, int iValue);
    void ArrayOr(int *piOrig, int *piDestino, int iNumElementos);
    void CopyArrays(int *piOrigen, int *piDestino, int iTotalAttributes);
    float BckBestProb(int *piFijados);
    float K_NN_WithMaskAuto(int *piFijados, bool bBar);


public slots:
    void OpenFileSuspects(void);
    void OpenFileDataSet(void);
    void DistancesCalculLauncher(void);
    void ExitMarsMiner(void);
    void ClearDatasets(void);
    void loadDistanceInTableView(void);
    void loadDistanceInTableViewTEST(void);

    // Invoca a GreedyIterative() y muestra los resultados.
    //void IterativeDistanceWithAutoAttributeSelection(void);

    // Invoca a GreedyRecursive() y muestra los resultados.
    //void RecursiveDistanceWithAutoAttributeSelection(void);

    //
    //void BackTrackingDistanceWithAutoAttributeSelection(void);

    void NormalMode(void);
    void PrefixedMode(void);
    void GreedyMode(void);
    void BacktrackingMode(void);

private:
    Ui::MarsMinerMainWin *ui;    
    clsMarsMiner *MMsuspects, *MMdataset, *MMdistances, *MMclass;
    bool suspectsLoaded, datasetLoaded;
    int K_VALUE;


};

#endif // MARSMINERMAINWIN_H
