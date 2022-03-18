#include "marsminermainwin.h"
#include "ui_marsminermainwin.h"

#define SUSPECTS    1
#define DATASET     2
//#define K_VALUE     7       // Valor que le damos a K.
//#define CLASS_COLUMN    6 //9   // Columna del attributo binario. OJO pq lo ponemos a mano.
#define K_EXTRA_COLS    2   // cols extra aparte de las K cols. 2 para clase y probabilidad.
//#define RECALCUL_CENTROID_VALUE 0.001
//#define ELLIPSE_SIZE    20
//#define DISTANCE_SCALE  250     // Valor por el que se multiplica las distancias
#define DISPERSION_FACTOR   0  // Valor de seperación inicio clusters (eje x)

//+-------------------------------------------------------------------------------------
// Contructor
//+-------------------------------------------------------------------------------------
MarsMinerMainWin::MarsMinerMainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MarsMinerMainWin)
{
    ui->setupUi(this);

    MMsuspects  = new clsMarsMiner;
    MMdataset   = new clsMarsMiner;
    MMdistances = new clsMarsMiner; // Defino K_VALUE columnas mas una.
    MMclass     = new clsMarsMiner; // para guardar la clase de cada distancia de los K.
    MMCluster   = new clsMarsMiner; // NUEVO para Clustering
    Soluciones  = NULL;             // NUEVO para Clustering
    scene=NULL;

    K_VALUE = 0;
    CLASS_COLUMN = 0;               // La cargo una vez cargado alguno de los DS.
    ELLIPSE_SIZE = 20;              // xdefecto
    DISTANCE_SCALE = 200;
    // Recojo como valor inicial, el que aparece en los Widget Dial y Spin.
    RECALCUL_CENTROID_VALUE = ui->doubleSpinBox_distance->value();

    suspectsLoaded  = false;        // se pone a true si se han cargado bien.
    datasetLoaded   = false;
    clusteringLoaded = false;       // NUEVO para Clustering.

    connect(ui->btn_OpenSuspects, SIGNAL(clicked()), this,
            SLOT(OpenFileSuspects()));
    connect(ui->btn_OpenDataset, SIGNAL(clicked()), this,
            SLOT(OpenFileDataSet()));
    connect(ui->btn_CalculateDistance, SIGNAL(clicked()), this,
            SLOT(DistancesCalculLauncher()));
    connect(ui->action_Distance, SIGNAL(triggered()), this,
            SLOT(DistancesCalculLauncher()));
    connect(ui->actionOpen_Suspects, SIGNAL(triggered()), this,
            SLOT(OpenFileSuspects()));
    connect(ui->actionOpen_Training, SIGNAL(triggered()), this,
            SLOT(OpenFileDataSet()));
    connect(ui->actionExit_MarsMiner_Project_1, SIGNAL(triggered()), this,
            SLOT(ExitMarsMiner()));

    connect(ui->actionDistance_in_Normal_Mode, SIGNAL(triggered()), this,
            SLOT(NormalMode()));
    connect(ui->actionDistance_in_Pre_Fixed_Mode, SIGNAL(triggered()), this,
            SLOT(PrefixedMode()));
    connect(ui->actionDistance_by_Greedy_Auto_Attribute_Selection, SIGNAL(triggered()),
            this, SLOT(GreedyMode()));
    connect(ui->actionBackTracking_Auto_Attribute_Selection, SIGNAL(triggered()),
            this, SLOT(BacktrackingMode()));
    connect(ui->actionClear_Datasets, SIGNAL(triggered()), this,
            SLOT(ClearDatasets()));

    // Nuevo connect para Clustering
    connect(ui->btn_CalculateClustering, SIGNAL(clicked()), this,
            SLOT(ClusteringCalculLauncher()));
    connect(ui->btn_OpenCluster, SIGNAL(clicked()), this, SLOT(OpenFileCluster()));

    connect(ui->actionScale_x100, SIGNAL(triggered()), this, SLOT(Scale100()));
    connect(ui->actionScale_x200, SIGNAL(triggered()), this, SLOT(Scale200()));
    connect(ui->actionScale_x300, SIGNAL(triggered()), this, SLOT(Scale300()));
    connect(ui->actionScale_x400, SIGNAL(triggered()), this, SLOT(Scale400()));
    connect(ui->actionScale_x500, SIGNAL(triggered()), this, SLOT(Scale500()));
    connect(ui->actionSize_20, SIGNAL(triggered()), this, SLOT(Ellipse20()));
    connect(ui->actionSize_40, SIGNAL(triggered()), this, SLOT(Ellipse40()));
    connect(ui->actionSize_60, SIGNAL(triggered()), this, SLOT(Ellipse60()));
    connect(ui->actionSize_80, SIGNAL(triggered()), this, SLOT(Ellipse80()));
    connect(ui->actionSize_100, SIGNAL(triggered()), this, SLOT(Ellipse100()));

    // Disal & Double SpinBox
    // Changing the value of spinbox will change the position of dial
    connect(ui->doubleSpinBox_distance, SIGNAL(valueChanged(double)),
            this, SLOT(spinChange(double)));
    //Changing the position of dial will change the value of the spinbox
    connect(ui->dial_distance,SIGNAL(valueChanged(int)),
            this, SLOT(dialChange(int)));

    ui->label_CalculateDistances->setText(
                "Enter a valid K_VALUE and a Pre Process option.");
    ui->lineEdit_Kvalue->setFocus();    
}



//+-------------------------------------------------------------------------------------
// Destructor. Falta implementarlo bien.
//+-------------------------------------------------------------------------------------
MarsMinerMainWin::~MarsMinerMainWin()
{
    delete ui;
}



//+-------------------------------------------------------------------------------------
// Funcion que ejecuta la salida del programa.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::ExitMarsMiner(void){
    exit(1);
}



//+-------------------------------------------------------------------------------------
// Open Suspects File SLOT
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::OpenFileSuspects(void){
    // Recojo el K_VALUE, en caso de que lo hayan introducido.
    int iKtest =  ui->lineEdit_Kvalue->text().toInt();
    if(iKtest > 0)
        K_VALUE = iKtest;
    else
        K_VALUE = 0;

    if(K_VALUE == 0){
        ui->tabWidget->setCurrentIndex(2);
        ui->label_CalculateDistances->setText(
                    "First, you must select a valid K_VALUE and Pre Process option.");
    }else{
        // Abro dialogo de seleccion de archivo.
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open arff File"), "C:/", tr("arff or txt Files (*.arff *.txt)"));

        // Me aseguro de tener creado los objetos, antes de cargarlos.
        if(MMsuspects == NULL)
            MMsuspects  = new clsMarsMiner;
        if(MMdistances == NULL)
            MMdistances = new clsMarsMiner;
        if(MMclass == NULL)
            MMclass     = new clsMarsMiner;

        // establezco la propiedad de pre-proceso, si lo ha escogido en combobox.
        if(ui->comboBox_Preprocess->currentIndex() == 0)
            MMsuspects->SetPreProceso(false);
        else
            MMsuspects->SetPreProceso(true);

        ui->tabWidget->setCurrentIndex(0);
        if(MMsuspects->BuildDataset((char *)qPrintable(fileName)) == false){
            ui->lineEdit_OpenSuspects->setText(
                        "Error loading Suspects DataSet from selected file !!");
            suspectsLoaded = false;
        }else{            
            ui->lineEdit_OpenSuspects->setText(fileName);
            loadDataSetInTableView(MMsuspects, ui->tableView_Suspects);
            suspectsLoaded = true;
            // Nuevo. Auto Cargo la CLASS_COLUMN que antes hacia a mano.
            if(CLASS_COLUMN == 0)
                // Todavía no ha sido establecida. La estbablezco al ultimo att.
                CLASS_COLUMN = MMsuspects->GetAttributes()->GetAttributesTotal()-1;

        }
    }
}




//+-------------------------------------------------------------------------------------
// Open DataSet File SLOT
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::OpenFileDataSet(void){
    // Recojo el K_VALUE, en caso de que lo hayan introducido.
    int iKtest =  ui->lineEdit_Kvalue->text().toInt();
    if(iKtest > 0)
        K_VALUE = iKtest;
    else
        K_VALUE = 0;

    if(K_VALUE == 0){
        ui->tabWidget->setCurrentIndex(2);
        ui->lineEdit_Kvalue->setFocus();
        ui->label_CalculateDistances->setText(
                    "First, you must select a valid K_VALUE and Pre Process option.");
    }else{
        // Abro dialogo de seleccion de archivo.
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open arff File"), "C:/", tr("arff or txt Files (*.arff *.txt)"));

        // Me aseguro de tener creado los objetos, antes de cargarlos.
        if(MMdataset == NULL)
            MMdataset   = new clsMarsMiner;
        if(MMdistances == NULL)
            MMdistances = new clsMarsMiner;
        if(MMclass == NULL)
            MMclass     = new clsMarsMiner;

        // establezco la propiedad de pre-proceso, si lo ha escogido en combobox.
        if(ui->comboBox_Preprocess->currentIndex() == 0)
            MMdataset->SetPreProceso(false);
        else
            MMdataset->SetPreProceso(true);

        ui->tabWidget->setCurrentIndex(1);
        if(MMdataset->BuildDataset((char *)qPrintable(fileName)) == false){
            ui->lineEdit_OpenDataset->setText(
                        "Error loading Training DataSet from selected file !!");
            datasetLoaded = false;
        }else{
            //qDebug() << "hola" << fileName << "adios";
            ui->lineEdit_OpenDataset->setText(fileName);
            loadDataSetInTableView(MMdataset, ui->tableView_Dataset);
            datasetLoaded = true;
            // Nuevo. Auto Cargo la CLASS_COLUMN que antes hacia a mano.
            if(CLASS_COLUMN == 0)
                // Todavía no ha sido establecida. La estbablezco al ultimo att.
                CLASS_COLUMN = MMsuspects->GetAttributes()->GetAttributesTotal()-1;

        }
    }
}



//+-------------------------------------------------------------------------------------
// Carga el DataSet especificado en el tableView especificado.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::loadDataSetInTableView(clsMarsMiner *MM,
                                              QTableView *tableView){
    int fil, col, iTotalAttributes, iTotalRegisters;
    iTotalAttributes    = MM->GetAttributes()->GetAttributesTotal();
    iTotalRegisters     = MM->GetDataSetTotalRegisters();
    // creo el model que cargo en el QTableView.
    QAbstractItemModel *model = new QStandardItemModel(iTotalRegisters,
                                                       iTotalAttributes, this);

    for(col=0; col < iTotalAttributes; col++){
        model->setHeaderData(col, Qt::Horizontal,
                             tr(MM->GetAttributes()->GetAttribute(col)->acName));
    }
    for(fil=0; fil< iTotalRegisters; fil++){
        for(col=0; col< iTotalAttributes; col++){
            QModelIndex index = model->index(fil, col, QModelIndex());
            QString cad;
            if(!strcmp(MM->GetAttributes()->GetAttribute(col)->acType,
                       (char *)qPrintable("real"))){
                cad = QString::number( MM->DataSetGetRow(fil)[col], 'f', 8 );
            }else if(!strcmp(MM->GetAttributes()->GetAttribute(col)->acType,
                            (char *)qPrintable("bin"))){
                //cad = QString::number( MM->DataSetGetRow(fil)[col]);
                if(ui->tabWidget->isTabEnabled(4) == false){
                    //float a =  MM->DataSetGetRow(fil)[col];
                    cad = QString::number( MM->DataSetGetRow(fil)[col]);
                }else{
                    cad = QString(MM->GetClass(MM->DataSetGetRow(fil)[col]));
                }
            }else if(!strcmp(MM->GetAttributes()->GetAttribute(col)->acType,
                            (char *)qPrintable("distance"))){
                cad = QString::number( MM->DataSetGetRow(fil)[col], 'f', 8 );
            }else if(!strcmp(MM->GetAttributes()->GetAttribute(col)->acType,
                            (char *)qPrintable("class"))){
                cad = QString::number( MM->DataSetGetRow(fil)[col]);
            }
            model->setData(index,  cad);
        }
    }
    tableView->setModel(model);
}



//+-------------------------------------------------------------------------------------
/*void MarsMinerMainWin::loadDistanceInTableView(clsMarsMiner *MMsu,
                                               clsMarsMiner *MMds,
                                               QTableView *tableView){*/
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::loadDistanceInTableView(void){    
    int filSu, filDs, col, iTotalAttributes, iAciertos, iProgress,
            iTotalRegistersSuspects, iTotalRegistersDataset;//, iIndividuo;
    double dDistancia = 0;
    float fMax=0, fMin=0, fProbabilidad=0.0;

    ui->tabWidget->setCurrentIndex(2);
    ui->lineEdit_Kvalue->setFocus();
    if(suspectsLoaded == false || datasetLoaded == false){
        // Mesnsaje conforme no se puede realizar el calculo hasta tener los datasets.
        ui->label_CalculateDistances->setText(
                    "First, you must load Suspects and Training datasets.");
    }else{
        iTotalAttributes        = MMsuspects->GetAttributes()->GetAttributesTotal();
        iTotalRegistersSuspects = MMsuspects->GetDataSetTotalRegisters();
        iTotalRegistersDataset  = MMdataset->GetDataSetTotalRegisters();

        // Recojo el K_VALUE, en caso de que lo hayan introducido.
        int iKtest =  ui->lineEdit_Kvalue->text().toInt();
        if(iKtest > 0 && iKtest < iTotalRegistersSuspects)
            K_VALUE = iKtest;
        else
            K_VALUE = 0;

        if(K_VALUE > 0){
            // Preparo los Header del DataSet de Distancias para los K individuos.
            QString qsIndividuo;
            for(col=0; col < K_VALUE; col++){
                qsIndividuo = QString("Individuo: %1").arg(col);
                MMdistances->GetAttributes()->SetAttribute((char *)qPrintable(qsIndividuo),
                                                           "distance", 0, 0, col);
                qsIndividuo = QString("Class: %1").arg(col);
                MMclass->GetAttributes()->SetAttribute((char *)qPrintable(qsIndividuo),
                                                       "class", 0, 0, col);
            }
            // Agrego los atributos extra al DS de distancias.
            MMdistances->GetAttributes()->SetAttribute("Danger 1-0 Good","bin",0,0,col);
            MMdistances->GetAttributes()->SetAttribute("Probability", "real", 0, 0, col + 1);

            // Establezco el numero de columnas de los dataset de distancia y clase.
            MMdistances->GetAttributes()->SetAttributesTotal(K_VALUE + 2);
            MMclass->GetAttributes()->SetAttributesTotal(K_VALUE);

            // establezco lo que será el 100% de la barra de progreso.
            ui->progressBar_distances->setMaximum(iTotalRegistersSuspects);
            for(filSu=0, iAciertos=0, iProgress = 1; filSu< iTotalRegistersSuspects;
                filSu++, iProgress++){

                // actualizo barra de progreso.
                ui->progressBar_distances->setValue(iProgress);

                // Agrego una nueva Row para el sospechoso # filSu.
                float *fMyRow = MMdistances->DataSetGetNewRow();
                MMdistances->DataSetInicRow(fMyRow, -1);
                MMdistances->DataSetAddRow(&fMyRow);

                // Agrego una nueva row para la clase del sospechoso #filSu.
                float *fMyRowClass = MMclass->DataSetGetNewRow();
                MMclass->DataSetInicRow(fMyRowClass, -1);
                MMclass->DataSetAddRow(&fMyRowClass);

                // Compruebo la distancia de cada suspect con cada individuo el Training DS.
                for(filDs=0/*, iIndividuo=0*/; filDs< iTotalRegistersDataset; filDs++){
                    // Empiezo desde la 2 ya que en 0 y 1 estan ID y Date. Ultimo Class tampoco.
                    for(col=2, dDistancia=0; col < (iTotalAttributes - 1); col++){
                        // Cogemos el máximo Max de los dos datasets.
                        if(MMsuspects->GetAttributes()->GetAttributeMaxValue(col) >
                                MMdataset->GetAttributes()->GetAttributeMaxValue(col))
                            fMax = MMsuspects->GetAttributes()->GetAttributeMaxValue(col);
                        else
                            fMax = MMdataset->GetAttributes()->GetAttributeMaxValue(col);
                        // Cogemos el mínimo Min de los dos datasets
                        if(MMsuspects->GetAttributes()->GetAttributeMinValue(col) <
                                MMdataset->GetAttributes()->GetAttributeMinValue(col))
                            fMin = MMsuspects->GetAttributes()->GetAttributeMinValue(col);
                        else
                            fMin = MMdataset->GetAttributes()->GetAttributeMinValue(col);
                        // Calculo de la distancia entre individuos, para el attribute #col.
                        if(!strcmp(MMsuspects->GetAttributes()->GetAttribute(col)->acType,
                                   (char *)qPrintable("real"))){
                            dDistancia += distanceNormal(MMsuspects->DataSetGetRow(filSu)[col],
                                                         MMdataset->DataSetGetRow(filDs)[col],
                                                         fMax, fMin);
                        }
                    }// Fin cols. Fin individuo.

                    // Miro si distancia de individuo hay que añadirla entre los K individuos.
                    dDistancia = qSqrt(dDistancia);
                    // Si este individuo del training se parece, guardo su distancia y clase.
                    feasibleDistance(MMdistances, MMclass, filSu,
                                     MMdataset->DataSetGetRow(filDs)[CLASS_COLUMN],
                                     dDistancia);
                }
                // Le paso DSdist, DSclass, col de resultados, col de probabilidad y #suspect.
                determineDangerousity(MMdistances, MMclass, K_VALUE, K_VALUE + 1, filSu);
                if(verificaClass(MMdistances, MMsuspects, filSu))
                    iAciertos++;
            }
            fProbabilidad = (float)iAciertos/MMsuspects->GetDataSetTotalRegisters()*100;
            ui->lineEdit_Probabilidad->setText(QString::number( fProbabilidad, 'f', 8 ));
            loadDataSetInTableView(MMdistances, ui->tableView_Distances);
            loadDataSetInTableView(MMclass, ui->tableView_Class);
            loadDistHeadInTableView(MMsuspects, MMdataset, ui->tableView_DistancesRanges);
            ui->label_CalculateDistances->setText("Distances Calculated");
        }else{
            ui->label_CalculateDistances->setText(
                        "You must enter a valid K_VALUE and Pre Process option.");
        }
    }
}



//+-------------------------------------------------------------------------------------
// Post: Indica si la distancia proporcionada es lo suficientemente pequeña como para
// agregarla al dataset de distancias proporcionado, para el individuo indicado.
// Devuelve 1 si la distancia ha sido agregada o 0 en caso contrario.
//
// Pre: El dataset proporcionado debe estar formateado para acceder a K attributes
// y como mínimo, al individuo indicado.
//
// Recibe el dataset de distancias, el de clases, el #suspect, las columnas del dataset
// de distancias que indican el número de peligrosos, de no peligrosos, la clase del
// individuo conocido y la distancia a evaluar.
//+-------------------------------------------------------------------------------------
int MarsMinerMainWin::feasibleDistance(clsMarsMiner *MM, clsMarsMiner *MMcls,
                                       int iIndividuo, int iClass, float fDistance){
    // Ultimo attribute del dataset de distances, es para indicar si es o no peligroso.
    for(int col=0; col < (MM->GetAttributes()->GetAttributesTotal() - K_EXTRA_COLS); col++){
        //qDebug() << " -- " << MM->DataSetGetRow(iIndividuo)[col] << Qt::endl;;
        if((fDistance < MM->DataSetGetRow(iIndividuo)[col] ||
                MM->DataSetGetRow(iIndividuo)[col] == -1) && fDistance != 0){
            insertaDistanceAndClass(MM, MMcls, iClass, iIndividuo, col, fDistance);
            return 1;
        }
    }
    return 0;
}




//+-------------------------------------------------------------------------------------
// Inserta la distancia proporcionada en la columna indicada desplazando los valores
// de su derecha. Se pierde el ultimo de la derecha, en caso de superar los K valores.
// El de la derecha es el de mayor distancia y el primero, el de menor.
// Conjuntamente, se realiza la misma operación en el dataset de clases ya que se guarda
// la clase del individuo que ha dado la distancia insertada.
//
// Si hay 4 attributos extra 'peligrosos, no peligrosos, probabilidad, etc..'
// debo empezar en -6 (9-6 = col 3) ya que la col 4 es el que hay que descartar.
// Si aumenta el num de attributos, hay que tener esto en cuenta.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::insertaDistanceAndClass(clsMarsMiner *MM, clsMarsMiner *MMcls,
                                               int iClass, int iRow, int iStartCol,
                                               float fDistance){
    //float fAux=0;
    // desplazo distancias a la derecha para hacer nuevo hueco en iStartCol.
    for(int col = (MM->GetAttributes()->GetAttributesTotal() - (K_EXTRA_COLS+2));
                   col >= iStartCol; col--){
        MM->DataSetGetRow(iRow)[col + 1]    = MM->DataSetGetRow(iRow)[col];
        MMcls->DataSetGetRow(iRow)[col + 1] = MMcls->DataSetGetRow(iRow)[col];
    }
    // Inserto distancia en la columna iStartCol de la fila iRow.
    MM->DataSetSetRowValue(iRow, iStartCol, fDistance);
    // Inserto la clase del individuo que ha dado la distancia insertada.
    MMcls->DataSetSetRowValue(iRow, iStartCol, iClass);
}





//+-------------------------------------------------------------------------------------
// Le paso el dataset de distancias, de clases y la columna en la que hay que poner los
// resultados de la clase y probabilidad.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::determineDangerousity(clsMarsMiner *MM, clsMarsMiner *MMcls,
                                             int iColResult, int iColProb, int iRow){
    int col;
    float fDangerCnt, fGoodCnt, fResult=0.0;
    // cuento cuantas dist eran de individuos peligrosos y cuantas no.
    // Para hacerlo miro la row del individuo en el DS de clases.
    for(fDangerCnt=0, fGoodCnt=0, col=0; col < K_VALUE; col++){
        if(MMcls->DataSetGetRow(iRow)[col] == 1)
            // peligroso
            fDangerCnt++;
        else
            // no peligroso
            fGoodCnt++;
    }
    if(fDangerCnt >= fGoodCnt){
        fResult = (fDangerCnt/K_VALUE)*100;
        MM->DataSetSetRowValue(iRow, iColResult, 1);
        MM->DataSetSetRowValue(iRow, iColProb, fResult);
    }else{
        fResult = (fGoodCnt/K_VALUE)*100;
        MM->DataSetSetRowValue(iRow, iColResult, 0);
        MM->DataSetSetRowValue(iRow, iColProb, fResult);
    }
}




//+-------------------------------------------------------------------------------------
// Calcula la diferencia entre fX fY y la eleva al cuadrado.
//+-------------------------------------------------------------------------------------
double MarsMinerMainWin::distance(float fX, float fY){
    return qPow((fX - fY), 2);
}




//+-------------------------------------------------------------------------------------
// Calcula la diferencia entre fX fY, la eleva al cuadrado y la normaliza.
//+-------------------------------------------------------------------------------------
double MarsMinerMainWin::distanceNormal(float fX, float fY, float fMax, float fMin){
    float fDenominador = fMax-fMin;
    if(fDenominador != 0)
        return qPow((fX - fY)/fDenominador, 2);
    else
        return 0; //return qPow((fX - fY), 2);
}





//+-------------------------------------------------------------------------------------
// Carga las distancias de la clase clsMarsMiner con los header Max-Min
// en el TableView especificado.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::loadDistHeadInTableView(clsMarsMiner *MMsu, clsMarsMiner *MMds,
                                               QTableView *tableView){
    int col, iTotalAttributes;
    float fMax = 0, fMin = 0;    
    QString cad;
    iTotalAttributes    = MMsu->GetAttributes()->GetAttributesTotal();
    QAbstractItemModel *model = new QStandardItemModel(2, iTotalAttributes, this);

    for(col=0; col < iTotalAttributes; col++){
        model->setHeaderData(col, Qt::Horizontal,
                             tr(MMsu->GetAttributes()->GetAttribute(col)->acName));
    }
    //for(fil=0; fil< 2; fil++){
        for(col=0; col < iTotalAttributes; col++){
            if(MMsu->GetAttributes()->GetAttributeMaxValue(col) >
                    MMds->GetAttributes()->GetAttributeMaxValue(col))
                fMax = MMsu->GetAttributes()->GetAttributeMaxValue(col);
            else
                fMax = MMds->GetAttributes()->GetAttributeMaxValue(col);
            // Cogemos el mínimo Min de los dos datasets
            if(MMsu->GetAttributes()->GetAttributeMinValue(col) <
                    MMds->GetAttributes()->GetAttributeMinValue(col))
                fMin = MMsu->GetAttributes()->GetAttributeMinValue(col);
            else
                fMin = MMds->GetAttributes()->GetAttributeMinValue(col);

            QModelIndex indexMax = model->index(0, col, QModelIndex());
            QModelIndex indexMin = model->index(1, col, QModelIndex());
            cad = QString::number( fMax, 'f', 8 );
            model->setData(indexMax, cad);
            cad = QString::number( fMin, 'f', 8 );
            model->setData(indexMin, cad);
        }
    //}
    tableView->setModel(model);
}




//+-------------------------------------------------------------------------------------
// Carga las distancias de la clase clsMarsMiner con los header Max-Min
// en el TableView especificado, pero solo para aquellos campos cuya posicion
// coincida con los habilitados por el array de mascara.
// Se considera habilitado un attribute si la posición del array de máscara asociada
// a ese attribute está a 1.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::loadDistHeadInTableViewWithMask(clsMarsMiner *MMsu,
                                                       clsMarsMiner *MMds,
                                                       QTableView *tableView,
                                                       int *piMask){
    int col, colAux, iTotalAttributes;
    float fMax = 0, fMin = 0;
    QString cad;
    iTotalAttributes    = MMsu->GetAttributes()->GetAttributesTotal();
    QAbstractItemModel *model = new QStandardItemModel(2, iTotalAttributes, this);

    for(col=0, colAux=0; col < iTotalAttributes; col++){
        if(piMask[col] == 1){
            // Si mask indica que este attribute se ha de visualizar.
            model->setHeaderData(colAux++, Qt::Horizontal,
                                 tr(MMsu->GetAttributes()->GetAttribute(col)->acName));
        }
    }
    for(col=0, colAux=0; col < iTotalAttributes; col++){
        if(piMask[col] == 1){
            // Si la mask indica que este attribute se ha de visualizar.
            if(MMsu->GetAttributes()->GetAttributeMaxValue(col) >
                    MMds->GetAttributes()->GetAttributeMaxValue(col))
                fMax = MMsu->GetAttributes()->GetAttributeMaxValue(col);
            else
                fMax = MMds->GetAttributes()->GetAttributeMaxValue(col);
            // Cogemos el mínimo Min de los dos datasets
            if(MMsu->GetAttributes()->GetAttributeMinValue(col) <
                    MMds->GetAttributes()->GetAttributeMinValue(col))
                fMin = MMsu->GetAttributes()->GetAttributeMinValue(col);
            else
                fMin = MMds->GetAttributes()->GetAttributeMinValue(col);

            QModelIndex indexMax = model->index(0, colAux, QModelIndex());
            QModelIndex indexMin = model->index(1, colAux++, QModelIndex());
            cad = QString::number( fMax, 'f', 8 );
            model->setData(indexMax, cad);
            cad = QString::number( fMin, 'f', 8 );
            model->setData(indexMin, cad);
        }
    }

    tableView->setModel(model);
}



//+-------------------------------------------------------------------------------------
// Devuelve true si ha habido acierto o false en caso contrario.
//+-------------------------------------------------------------------------------------
bool MarsMinerMainWin::verificaClass(clsMarsMiner *MMds, clsMarsMiner *MMsu, int filSu){
    if(MMds->DataSetGetRow(filSu)[K_VALUE] == MMsu->DataSetGetRow(filSu)[CLASS_COLUMN])
        return true;
    else
        return false;
}



//+-------------------------------------------------------------------------------------
// Limpia datasets y tableviews para un nuevo calculo.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::ClearDatasets(void){
    // Vacio los model de cada QTableView.

    if(ui->tableView_Suspects->model())
        ui->tableView_Suspects->model()->removeRows(0,
            ui->tableView_Suspects->model()->rowCount());
    if(ui->tableView_Dataset->model())
        ui->tableView_Dataset->model()->removeRows(0,
            ui->tableView_Dataset->model()->rowCount());
    if(ui->tableView_Distances->model())
        ui->tableView_Distances->model()->removeRows(0,
            ui->tableView_Distances->model()->rowCount());
    if(ui->tableView_DistancesRanges->model())
        ui->tableView_DistancesRanges->model()->removeRows(0,
            ui->tableView_DistancesRanges->model()->rowCount());
    if(ui->tableView_Class->model())
        ui->tableView_Class->model()->removeRows(0,
            ui->tableView_Class->model()->rowCount());
    // NUEVO Clustering
    if(ui->tableView_Cluster->model())
        ui->tableView_Cluster->model()->removeRows(0,
            ui->tableView_Cluster->model()->rowCount());

    // Visualizo ls QTableView ya vacios.
    ui->tableView_Suspects->repaint();
    ui->tableView_Dataset->repaint();
    ui->tableView_Distances->repaint();
    ui->tableView_DistancesRanges->repaint();
    ui->tableView_Class->repaint();
    // NEUVO Clustering
    ui->tableView_Cluster->repaint();

    // Elimino los objetos utilizados.
    if(MMsuspects != NULL)
        delete MMsuspects;
    if(MMdataset != NULL)
        delete MMdataset;
    if(MMdistances != NULL)
        delete MMdistances;
    if(MMclass != NULL)
        delete MMclass;
    // NEUVO Clustering
    if(MMCluster != NULL)
        delete MMCluster;

    // Los inicializo a NULL para pedir, si es el caso, de nuevo el objeto.
    MMsuspects  = NULL;
    MMdataset   = NULL;
    MMdistances = NULL;
    MMclass     = NULL;
    // NEUVO Clustering
    MMCluster   = NULL;

    // Le indico a loadDistanceInTableView() que los DS no estan cargados.
    suspectsLoaded  = false;
    datasetLoaded   = false;
    // NEUVO Clustering
    clusteringLoaded    = false;



    // inicializamos resto de elementos.
    ui->lineEdit_Probabilidad->clear();
    ui->lineEdit_OpenSuspects->clear();
    ui->lineEdit_OpenDataset->clear();
    ui->label_CalculateDistances->setText("Datasets successfully cleared.");
    ui->progressBar_distances->setValue(0);
    ui->lineEdit_Kvalue->clear();
    //ui->tabWidget->setCurrentIndex(2);
    //ui->lineEdit_Kvalue->setFocus();
    // NEUVO Clustering
    ui->progressBar_clustering->setValue(0);
    ui->lineEdit_KvalueCluster->clear();
    ui->lineEdit_OpenCluster->clear();
    ui->label_CalculateClustering->setText("Datasets successfully cleared.");
    ui->tabWidget->setCurrentIndex(4);
    ui->lineEdit_KvalueCluster->setFocus();
    ui->tabWidget_Clustering->setCurrentIndex(0);

    // Limpio los Graphic View. Lo hago eliminando sus items.
    if(ui->graphicsView_Cluster1->scene() != NULL)
        qDeleteAll(ui->graphicsView_Cluster1->scene()->items());
    if(ui->graphicsView_Cluster2->scene() != NULL)
        qDeleteAll(ui->graphicsView_Cluster2->scene()->items());
    if(ui->graphicsView_Cluster3->scene() != NULL)
        qDeleteAll(ui->graphicsView_Cluster3->scene()->items());
    if(ui->graphicsView_Cluster4->scene() != NULL)
        qDeleteAll(ui->graphicsView_Cluster4->scene()->items());
    if(ui->graphicsView_Cluster5->scene() != NULL)
        qDeleteAll(ui->graphicsView_Cluster5->scene()->items());
    if(ui->graphicsView_Cluster6->scene() != NULL)
        qDeleteAll(ui->graphicsView_Cluster6->scene()->items());
    if(ui->graphicsView_Cluster7->scene() != NULL)
        qDeleteAll(ui->graphicsView_Cluster7->scene()->items());

    // Elimino array de clusters con las soluciones
    deleteSoluciones();

    // Inicializo K a 0
    K_VALUE = 0;
}



void MarsMinerMainWin::deleteSoluciones(void) {
    Cluster **clusters;

    if(Soluciones){
    for(int i=0; i<K_VALUE; i++){
        clusters = Soluciones[i];
        if(clusters != NULL){
            for(int j=0; j<=i; j++){
                if(clusters[j] != NULL){
                    delete clusters[j];
                }
            }
        }
    }
    Soluciones = NULL;
    }
}

















//+-------------------------------------------------------------------------------------
// Metodos de Seleccion Automatica de Attributes.
//+-------------------------------------------------------------------------------------




//+-------------------------------------------------------------------------------------
// Acciones directas mediante combinacion de teclas.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::NormalMode(void){
    ui->comboBox_AttributeSelection->setCurrentIndex(0);
    DistancesCalculLauncher();
}

void MarsMinerMainWin::PrefixedMode(void){
    ui->comboBox_AttributeSelection->setCurrentIndex(1);
    DistancesCalculLauncher();
}

void MarsMinerMainWin::GreedyMode(void){
    ui->comboBox_AttributeSelection->setCurrentIndex(2);
    DistancesCalculLauncher();
}

void MarsMinerMainWin::BacktrackingMode(void){
    ui->comboBox_AttributeSelection->setCurrentIndex(3);
    DistancesCalculLauncher();
}




//+-------------------------------------------------------------------------------------
// Gestiona la funcion de distancia invocada por el usuario.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::DistancesCalculLauncher(void){
    int col, iTotalAttributes, iTotalRegistersSuspects;//, iTotalRegistersDataset;
    float fProbabilidad=0.0;

    // selecciono el tab de distances.
    ui->tabWidget->setCurrentIndex(2);
    ui->lineEdit_Kvalue->setFocus();

    if(suspectsLoaded == false || datasetLoaded == false){
        // Msg: NO se puede realizar el calculo hasta tener los datasets.
        ui->label_CalculateDistances->setText(
                    "First, you must load Suspects and Training datasets.");

    }else{
        iTotalAttributes        = MMsuspects->GetAttributes()->GetAttributesTotal();
        iTotalRegistersSuspects = MMsuspects->GetDataSetTotalRegisters();
        //iTotalRegistersDataset  = MMdataset->GetDataSetTotalRegisters();

        // Recojo el K_VALUE, en caso de que lo hayan introducido.
        int iKtest =  ui->lineEdit_Kvalue->text().toInt();
        if(iKtest > 0 && iKtest < iTotalRegistersSuspects)
            K_VALUE = iKtest;
        else
            K_VALUE = 0;

        if(K_VALUE > 0){
            ui->label_CalculateDistances->setText("Calculating distances...");

            // CREO ARRAY A 0 DE FIJADOS y DECODIFICO TIPO DE OPERACION.
            int *piFijados = new int[iTotalAttributes];

            switch(ui->comboBox_AttributeSelection->currentIndex()){
            case 0:
                // Normal.
                ui->progressBar_distances->setMaximum(iTotalRegistersSuspects);
                // Inicializo array de fijados a 1. Considero todos los atts.
                InicializaArray(piFijados,iTotalAttributes, 1);
                fProbabilidad = K_NN_WithMaskAuto(piFijados, true);
                ui->progressBar_distances->setValue(iTotalRegistersSuspects);
                break;
            case 1:
                // Pre-Fixed.
                ui->progressBar_distances->setMaximum(iTotalRegistersSuspects);
                InicializaArray(piFijados,iTotalAttributes, 1);
                // Considero del 2 al 8 escala [0..9].
                piFijados[0] = piFijados[1] = piFijados[9] = 0;
                fProbabilidad = K_NN_WithMaskAuto(piFijados, true);
                ui->progressBar_distances->setValue(iTotalRegistersSuspects);
                break;
            case 2:
                // Merodo Greedy Auto Attribute Selection.
                InicializaArray(piFijados,iTotalAttributes, 0);
                fProbabilidad = GreedyIterativeAuto(piFijados, iTotalAttributes);
                break;
            case 3:
                // inicializo progress bar.
                int max = 2000000000;
                ui->progressBar_distances->setMaximum(max);
                ui->progressBar_distances->setValue(0);
                // Metodo de fuerza bruta sobre todo el espacio. Backtracking.
                InicializaArray(piFijados,iTotalAttributes, 0);
                fProbabilidad = BckBestProb(piFijados);
                ui->progressBar_distances->setValue(max);
                break;
            }
            // Cargo combinacion piFijados en distances y class datasets.
            K_NN_WithMask(piFijados, MMdistances, MMclass);


            // MONTO LAS CABECERAS DE DISTANCES Y CLASS
            QString qsIndividuo;
            for(col=0; col < K_VALUE; col++){
                qsIndividuo = QString("Individuo: %1").arg(col);
                MMdistances->GetAttributes()->SetAttribute((char *)qPrintable(qsIndividuo),
                                                           "distance", 0, 0, col);
                qsIndividuo = QString("Class: %1").arg(col);
                MMclass->GetAttributes()->SetAttribute((char *)qPrintable(qsIndividuo),
                                                       "class", 0, 0, col);
            }
            // Agrego los atributos extra al DS de distancias.
            MMdistances->GetAttributes()->SetAttribute("Danger 1-0 Good","bin",0,0,col);
            MMdistances->GetAttributes()->SetAttribute("Probability", "real", 0, 0, col + 1);

            // Establezco el numero de columnas de los dataset de distancia y clase.
            MMdistances->GetAttributes()->SetAttributesTotal(K_VALUE + 2);
            MMclass->GetAttributes()->SetAttributesTotal(K_VALUE);


            // MUESTRO LOS RESULTADOS
            ui->lineEdit_Probabilidad->setText(QString::number( fProbabilidad, 'f', 8 ));
            loadDataSetInTableView(MMdistances, ui->tableView_Distances);
            loadDataSetInTableView(MMclass, ui->tableView_Class);
            loadDistHeadInTableViewWithMask(MMsuspects, MMdataset,
                                            ui->tableView_DistancesRanges, piFijados);
            ui->label_CalculateDistances->setText("Distances Calculated");

            delete [] piFijados;
        }else{
            ui->label_CalculateDistances->setText(
                        "You must enter a valid K_VALUE and Pre Process option.");
        }
    }

}




//+-------------------------------------------------------------------------------------
// Deja cargados los dataset de distances y class con los resultados de la mejor
// Probabilidad encontrada.
//
// PRE: piFijados es un array de tantos attributes como tenga el dataset de Suspects y
//      que se indica mediante el parametro iTotalAttributes.
//
//+-------------------------------------------------------------------------------------
float MarsMinerMainWin::GreedyIterative(int *piFijados, int iTotalAttributes){
    int iNumFijados, iAtt_Mask, iAtt_Mask_Mejor;
    //int aiMask[iTotalAttributes];
    int *piMask = new int[iTotalAttributes];
    float fProbabilidadAciertos = 0.0, fProbabilidadAciertosMejor = 0.0;
    bool mejorP_encontrada = true;
    int iProgress=0;

    // establezco lo que será, como max, el 100% de la barra de progreso.
    ui->progressBar_distances->setMaximum(iTotalAttributes * (iTotalAttributes/2));

    for(iNumFijados=0; (iNumFijados < iTotalAttributes &&
        mejorP_encontrada == true); ){

        mejorP_encontrada = false;
        for(iAtt_Mask=0; iAtt_Mask < iTotalAttributes; iAtt_Mask++){
            if(piFijados[iAtt_Mask] == 0){
                // Establezco estado barra de progeso.
                ui->progressBar_distances->setValue(iProgress++);

                // Attribute no fijado, miro la Prob para esta comb de attributes.
                // Indico en aiMask los attributes sobre los que calcular la dist.
                InicializaArray(piMask, iTotalAttributes, 0);
                piMask[iAtt_Mask] = 1;
                ArrayOr(piFijados, piMask, iTotalAttributes);

                // TEST
                qDebug() << "piMask: ";
                for(int i=0; i<iTotalAttributes; i++)
                    qDebug() << piMask[i];
                // FIN_TEST


                // aiMask tiene a 1 las posic de los attributes para la f(x) de
                // distancia. Calculo la Prob Total de Aciertos para esta comb de att.
                clsMarsMiner MMdst;
                clsMarsMiner MMcls;
                fProbabilidadAciertos = K_NN_WithMask(piMask, &MMdst, &MMcls);
                if(fProbabilidadAciertos > fProbabilidadAciertosMejor){
                    // Mejor P encontrada. Me guardo el Attribute que me la ha dado.
                    fProbabilidadAciertosMejor = fProbabilidadAciertos;
                    iAtt_Mask_Mejor = iAtt_Mask;
                    mejorP_encontrada = true;
                    // El constructor de copia pide memoria para cada row. Si distances
                    // y class ya estaban creados, antes de solicitar hay que liberar.
                    if(MMdistances != NULL)
                        delete MMdistances;
                    MMdistances = new clsMarsMiner(MMdst);

                    if(MMclass != NULL)
                        delete MMclass;
                    MMclass = new clsMarsMiner(MMcls);
                }
            }
        }
        if(mejorP_encontrada == true){
            // Fijo el nuevo attribute que aumenta la Probabilidad de aciertos.
            piFijados[iAtt_Mask_Mejor] = 1;
            // Incremento el número de attributes fijados.
            iNumFijados++;
        }
    }
    // Pongo la progress bar al 100%.
    ui->progressBar_distances->setValue(iTotalAttributes * (iTotalAttributes/2));
    delete [] piMask;
    return fProbabilidadAciertosMejor;
}




//+-------------------------------------------------------------------------------------
// No uso datasets para guardar la info. Cuando tenga el array piFijados con la mejor
// solucion, solo debere pasarselo a K_NN_WithMask() para que cargue los dataset de
// distancia y clase. Una vez cargados, los podre mostrar en el tableview.
//+-------------------------------------------------------------------------------------
float MarsMinerMainWin::GreedyIterativeAuto(int *piFijados, int iTotalAttributes){
    int iNumFijados, iAtt_Mask, iAtt_Mask_Mejor;
    int *piMask = new int[iTotalAttributes];
    float fProbabilidadAciertos = 0.0, fProbabilidadAciertosMejor = 0.0;
    bool mejorP_encontrada = true;
    int iProgress=0;

    // establezco lo que será, como max, el 100% de la barra de progreso.
    ui->progressBar_distances->setMaximum(iTotalAttributes * (iTotalAttributes/2));

    for(iNumFijados=0; (iNumFijados < iTotalAttributes &&
        mejorP_encontrada == true); ){

        mejorP_encontrada = false;
        for(iAtt_Mask=0; iAtt_Mask < iTotalAttributes; iAtt_Mask++){
            if(piFijados[iAtt_Mask] == 0){
                // Establezco estado barra de progeso.
                ui->progressBar_distances->setValue(iProgress++);

                // Attribute no fijado, miro la Prob para esta comb de attributes.
                InicializaArray(piMask, iTotalAttributes, 0);
                // Indico en aiMask los attributes sobre los que calcular la dist.
                piMask[iAtt_Mask] = 1;
                ArrayOr(piFijados, piMask, iTotalAttributes);                

                // aiMask tiene a 1 las posic de los attributes para la f(x) de
                // distancia. Calculo la Prob Total de Aciertos para esta comb de att.
                fProbabilidadAciertos = K_NN_WithMaskAuto(piMask, false);
                if(fProbabilidadAciertos > fProbabilidadAciertosMejor){
                    // Mejor P encontrada. Me guardo el Attribute que me la ha dado.
                    fProbabilidadAciertosMejor = fProbabilidadAciertos;
                    iAtt_Mask_Mejor = iAtt_Mask;
                    mejorP_encontrada = true;
                }
            }
        }
        if(mejorP_encontrada == true){
            // Fijo el nuevo attribute que aumenta la Probabilidad de aciertos.
            piFijados[iAtt_Mask_Mejor] = 1;
            // Incremento el número de attributes fijados.
            iNumFijados++;
        }
    }
    // Pongo la progress bar al 100%.
    ui->progressBar_distances->setValue(iTotalAttributes * (iTotalAttributes/2));
    delete [] piMask;
    return fProbabilidadAciertosMejor;
}




//+-------------------------------------------------------------------------------------
// - Calcula la distancia del Suspect dataset devolviendo la probabilidad de aciertos
// totales en la resolucion de la clase de los suspects.
// - Calcula la distancia considerando solo los atributos marcados a 1 en su
// correspondiente posicion del array de piFijados.
// - Deja las distancias y las clases resultantes en los dataset MMdistances y MMclass.
// - No crea los header de los dataset MMdistances ni MMclass, pero si guarda su numero
// de attributos en funcion del valor K_VALUE seleccionado.
//
// Pre: Los dataset MMdistances y MMclass deben tener memoria solicitada pero vacios
//      y sin cargar. Se debe haber hecho el new, pero no haberse cargado previamente.
//+-------------------------------------------------------------------------------------
float MarsMinerMainWin::K_NN_WithMask(int *piFijados, clsMarsMiner *MMdst,
                                      clsMarsMiner *MMcls){

    int filSu, filDs, col, iTotalAttributes, iAciertos,// iProgress,
            iTotalRegistersSuspects, iTotalRegistersDataset;//, iIndividuo;
    double dDistancia = 0;
    float fMax=0, fMin=0, fProbabilidad=0.0;

    // Establezco el numero de columnas de los dataset de distancia y clase.
    MMdst->GetAttributes()->SetAttributesTotal(K_VALUE + 2);
    MMcls->GetAttributes()->SetAttributesTotal(K_VALUE);

    // Guardo tamaños dataset en vars.
    iTotalAttributes        = MMsuspects->GetAttributes()->GetAttributesTotal();
    iTotalRegistersSuspects = MMsuspects->GetDataSetTotalRegisters();
    iTotalRegistersDataset  = MMdataset->GetDataSetTotalRegisters();

    for(filSu=0, iAciertos=0/*, iProgress = 1*/; filSu< iTotalRegistersSuspects;
        filSu++){ //coemento pq progreso no se implementa aki.// , iProgress++){

        // Agrego una nueva Row para el sospechoso # filSu.
        float *fMyRow = MMdst->DataSetGetNewRow();
        MMdst->DataSetInicRow(fMyRow, -1);
        MMdst->DataSetAddRow(&fMyRow);

        // Agrego una nueva row para la clase del sospechoso #filSu.
        float *fMyRowClass = MMcls->DataSetGetNewRow();
        MMcls->DataSetInicRow(fMyRowClass, -1);
        MMcls->DataSetAddRow(&fMyRowClass);

        // Compruebo la distancia de cada suspect con cada individuo del Training DS.
        for(filDs=0/*, iIndividuo=0*/; filDs< iTotalRegistersDataset; filDs++){
            // Empiezo desde la 2 ya que en 0 y 1 estan ID y Date. Ultimo Class tampoco.
            for(col=0, dDistancia=0; col < iTotalAttributes; col++){

                // Miro el attribute si esta fijado.
                if(piFijados[col] == 1){
                    // Cogemos el máximo Max de los dos datasets.
                    if(MMsuspects->GetAttributes()->GetAttributeMaxValue(col) >
                            MMdataset->GetAttributes()->GetAttributeMaxValue(col))
                        fMax = MMsuspects->GetAttributes()->GetAttributeMaxValue(col);
                    else
                        fMax = MMdataset->GetAttributes()->GetAttributeMaxValue(col);
                    // Cogemos el mínimo Min de los dos datasets
                    if(MMsuspects->GetAttributes()->GetAttributeMinValue(col) <
                            MMdataset->GetAttributes()->GetAttributeMinValue(col))
                        fMin = MMsuspects->GetAttributes()->GetAttributeMinValue(col);
                    else
                        fMin = MMdataset->GetAttributes()->GetAttributeMinValue(col);
                    // Calculo de la distancia entre individuos, para el attribute #col.
                    if(!strcmp(MMsuspects->GetAttributes()->GetAttribute(col)->acType,
                               (char *)qPrintable("real"))){
                        dDistancia += distanceNormal(MMsuspects->DataSetGetRow(filSu)[col],
                                                     MMdataset->DataSetGetRow(filDs)[col],
                                                     fMax, fMin);
                    }
                }// fin evaluar attribute fijado.

            }// Fin cols. Fin individuo.
            // Miro si distancia de individuo hay que añadirla entre los K individuos.
            dDistancia = qSqrt(dDistancia);
            // Si este individuo del training se parece, guardo su distancia y clase.
            feasibleDistance(MMdst, MMcls, filSu,
                             MMdataset->DataSetGetRow(filDs)[CLASS_COLUMN],
                             dDistancia);
        }
        // Le paso DSdist, DSclass, col de resultados, col de probabilidad y #suspect.
        determineDangerousity(MMdst, MMcls, K_VALUE, K_VALUE + 1, filSu);
        if(verificaClass(MMdst, MMsuspects, filSu))
            iAciertos++;
    }
    fProbabilidad = (float)iAciertos/MMsuspects->GetDataSetTotalRegisters()*100;
    return fProbabilidad;
}



//+-------------------------------------------------------------------------------------
// Calcula la probabilidad total de aciertos para una combinacion de atributos dada.
//
// Prueba del K-NN sin necesidad de datasets de distancia ni class en la f(x) caller.
// Crea en el ambito de esta f(x) los dataset de distancia y class para los calculos.
// Una vez se abandona el ambito, se pierden los resultados, devolviendo el calculo de
// la probabilidad para la combincacion piFijados proporcionada.
//
// Si bBar == true muestra la barra de progreso en funcion del numero de suspects.
//+-------------------------------------------------------------------------------------
float MarsMinerMainWin::K_NN_WithMaskAuto(int *piFijados, bool bBar){
    int filSu, filDs, col, iTotalAttributes, iAciertos,// iProgress,
            iTotalRegistersSuspects, iTotalRegistersDataset;//, iIndividuo;
    double dDistancia = 0;
    float fMax=0, fMin=0, fProbabilidad=0.0;
    bool vacio = true;

    for(col=0; col < MMsuspects->GetAttributes()->GetAttributesTotal(); col++){
        if(piFijados[col] == 1)
            vacio = false;
    }
    // Si piFijados esta todo a 0, devuelvo probabilidad 0.
    if(vacio == true)
        return 0.0;

    clsMarsMiner MMdst;
    clsMarsMiner MMcls;
    // Establezco el numero de columnas de los dataset de distancia y clase.
    MMdst.GetAttributes()->SetAttributesTotal(K_VALUE + 2);
    MMcls.GetAttributes()->SetAttributesTotal(K_VALUE);

    // Guardo tamaños dataset en vars.
    iTotalAttributes        = MMsuspects->GetAttributes()->GetAttributesTotal();
    iTotalRegistersSuspects = MMsuspects->GetDataSetTotalRegisters();
    iTotalRegistersDataset  = MMdataset->GetDataSetTotalRegisters();

    for(filSu=0, iAciertos=0/*, iProgress = 1*/; filSu< iTotalRegistersSuspects;
        filSu++){ //coemento pq progreso no se implementa aki.// , iProgress++){

        if(bBar)
            // Establezco estado progress bar.
            ui->progressBar_distances->setValue(filSu);

        // Agrego una nueva Row para el sospechoso # filSu.
        float *fMyRow = MMdst.DataSetGetNewRow();
        MMdst.DataSetInicRow(fMyRow, -1);
        MMdst.DataSetAddRow(&fMyRow);

        // Agrego una nueva row para la clase del sospechoso #filSu.
        float *fMyRowClass = MMcls.DataSetGetNewRow();
        MMcls.DataSetInicRow(fMyRowClass, -1);
        MMcls.DataSetAddRow(&fMyRowClass);

        // Compruebo la distancia de cada suspect con cada individuo del Training DS.
        for(filDs=0/*, iIndividuo=0*/; filDs< iTotalRegistersDataset; filDs++){
            // Empiezo desde la 2 ya que en 0 y 1 estan ID y Date. Ultimo Class tampoco.
            for(col=0, dDistancia=0; col < iTotalAttributes; col++){

                // Miro el attribute si esta fijado.
                if(piFijados[col] == 1){
                    // Cogemos el máximo Max de los dos datasets.
                    if(MMsuspects->GetAttributes()->GetAttributeMaxValue(col) >
                            MMdataset->GetAttributes()->GetAttributeMaxValue(col))
                        fMax = MMsuspects->GetAttributes()->GetAttributeMaxValue(col);
                    else
                        fMax = MMdataset->GetAttributes()->GetAttributeMaxValue(col);
                    // Cogemos el mínimo Min de los dos datasets
                    if(MMsuspects->GetAttributes()->GetAttributeMinValue(col) <
                            MMdataset->GetAttributes()->GetAttributeMinValue(col))
                        fMin = MMsuspects->GetAttributes()->GetAttributeMinValue(col);
                    else
                        fMin = MMdataset->GetAttributes()->GetAttributeMinValue(col);
                    // Calculo de la distancia entre individuos, para el attribute #col.
                    if(!strcmp(MMsuspects->GetAttributes()->GetAttribute(col)->acType,
                               (char *)qPrintable("real"))){
                        dDistancia += distanceNormal(MMsuspects->DataSetGetRow(filSu)[col],
                                                     MMdataset->DataSetGetRow(filDs)[col],
                                                     fMax, fMin);
                    }
                }// fin evaluar attribute fijado.

            }// Fin cols. Fin individuo.
            // Miro si distancia de individuo hay que añadirla entre los K individuos.
            dDistancia = qSqrt(dDistancia);
            // Si este individuo del training se parece, guardo su distancia y clase.
            feasibleDistance(&MMdst, &MMcls, filSu,
                             MMdataset->DataSetGetRow(filDs)[CLASS_COLUMN],
                             dDistancia);
        }
        // Le paso DSdist, DSclass, col de resultados, col de probabilidad y #suspect.
        determineDangerousity(&MMdst, &MMcls, K_VALUE, K_VALUE + 1, filSu);
        if(verificaClass(&MMdst, MMsuspects, filSu))
            iAciertos++;

    }
    fProbabilidad = (float)iAciertos/MMsuspects->GetDataSetTotalRegisters()*100;
    return fProbabilidad;
}




//+-------------------------------------------------------------------------------------
// Calcula la mejor solucion para TODO EL ESPACIO de busqueda.
// Se le pasa un array de fijados inicializado a 0 y recorre todo el espacio de busqueda
// en busca de la combinacion de atributos que da mayor probabilidad de acierto.
// Al terminar, se consigue el array de piFijados cargado con los atributos mas
// importantes para identificar a un individuo de la manera más aproximada.
//+-------------------------------------------------------------------------------------
float MarsMinerMainWin::BckBestProb(int *piFijados){
    float fP = 0.0, fPbck = 0.0;
    int iAtt_Mask, iTotalAttributes;
    bool mejorP_encontrada = false;
    int value = 0;

    iTotalAttributes = MMsuspects->GetAttributes()->GetAttributesTotal();
    int *aiMask     = new int[iTotalAttributes];
    int *aiMejor    = new int[iTotalAttributes];

    // Evaluo Prob nodo actual.
    fP = K_NN_WithMaskAuto(piFijados, false);

    // Recorro hijos del nodo actual.
    for(iAtt_Mask = 0; iAtt_Mask < iTotalAttributes; iAtt_Mask++){
        if(piFijados[iAtt_Mask] == 0){
            // Si nodo no evaluado.
            value = ui->progressBar_distances->value();
            ui->progressBar_distances->setValue(++value);

            InicializaArray(aiMask, iTotalAttributes, 0);
            aiMask[iAtt_Mask] = 1;
            ArrayOr(piFijados, aiMask, iTotalAttributes);

            // DEBUG
            //qDebug() << "piMask: ";
            //for(int i=0; i<iTotalAttributes; i++)
            //    qDebug() << aiMask[i];
            // FIN_DEBUG

            // Recojo mekor Prob del hijo aiMask.
            fPbck = BckBestProb(aiMask);
            // Si prob de uno de sus hijos mayor, la guardo.
            if(fPbck > fP){
                fP = fPbck;
                CopyArrays(aiMask, aiMejor, iTotalAttributes);
                mejorP_encontrada = true;
            }
        }
    }
    if(mejorP_encontrada){
        // Encontrada Prob de un nodo hijo mayor a la del nodo actual,
        // se la paso a nodo superior.
        CopyArrays(aiMejor, piFijados, iTotalAttributes);
    }
    delete [] aiMask;
    delete [] aiMejor;
    return fP;
}




//+-------------------------------------------------------------------------------------
// Copia el array de enteros piOrigen sobre el array destino piDestino.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::CopyArrays(int *piOrigen, int *piDestino, int iTotalAttributes){
    for(int i = 0; i < iTotalAttributes; i++)
        piDestino[i] = piOrigen[i];
}


//+-------------------------------------------------------------------------------------
// Inicializa un array de enteros de iNumElementos al valor iValue.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::InicializaArray(int *piArray, int iNumElementos, int iValue){
    for(int i=0; i < iNumElementos; i++)
        piArray[i] = iValue;
}


//+-------------------------------------------------------------------------------------
// Hace la operacion OR entre los arrays Origen y Destino, dejando el resultado en el
// array de destino. DESTINO = ORIG or DESTINO;
// Pone los 1's de cada posicion de Origen en la posicion asociada de destino.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::ArrayOr(int *piOrig, int *piDestino, int iNumElementos){
    for(int i=0; i < iNumElementos; i++)
        if(piOrig[i] == 1)
            piDestino[i] = 1;
}




















//+-------------------------------------------------------------------------------------
// TEST
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::loadDistanceInTableViewTEST(void){
    int col/*, iTotalAttributes*/, iTotalRegistersSuspects;//, iTotalRegistersDataset;
    float fProbabilidad=0.0;

    ui->tabWidget->setCurrentIndex(2);
    ui->lineEdit_Kvalue->setFocus();
    if(suspectsLoaded == false || datasetLoaded == false){
        // Mesnsaje conforme no se puede realizar el calculo hasta tener los datasets.
        ui->label_CalculateDistances->setText("First, you must load Suspects and Training datasets.");
    }else{
        //iTotalAttributes        = MMsuspects->GetAttributes()->GetAttributesTotal();
        iTotalRegistersSuspects = MMsuspects->GetDataSetTotalRegisters();
        //iTotalRegistersDataset  = MMdataset->GetDataSetTotalRegisters();

        // Recojo el K_VALUE, en caso de que lo hayan introducido.
        int iKtest =  ui->lineEdit_Kvalue->text().toInt();
        if(iKtest > 0 && iKtest < iTotalRegistersSuspects)
            K_VALUE = iKtest;
        else
            K_VALUE = 0;

        if(K_VALUE > 0){
            // Preparo los Header del DataSet de Distancias para los K individuos.
            QString qsIndividuo;
            for(col=0; col < K_VALUE; col++){
                qsIndividuo = QString("Individuo: %1").arg(col);
                MMdistances->GetAttributes()->SetAttribute((char *)qPrintable(qsIndividuo),
                                                           "distance", 0, 0, col);
                qsIndividuo = QString("Class: %1").arg(col);
                MMclass->GetAttributes()->SetAttribute((char *)qPrintable(qsIndividuo),
                                                       "class", 0, 0, col);
            }
            // Agrego los atributos extra al DS de distancias.
            MMdistances->GetAttributes()->SetAttribute("Danger 1-0 Good","bin",0,0,col);
            MMdistances->GetAttributes()->SetAttribute("Probability", "real", 0, 0, col + 1);

            // Establezco el numero de columnas de los dataset de distancia y clase.
            MMdistances->GetAttributes()->SetAttributesTotal(K_VALUE + 2);
            MMclass->GetAttributes()->SetAttributesTotal(K_VALUE);

            // Evaluo la probabilidad para una determinada combinacion de attributes.
            int aiFijados[] = {0,0, 1,1, 1,1, 1,1, 1,0};
            fProbabilidad = K_NN_WithMask(aiFijados, MMdistances, MMclass);

            ui->lineEdit_Probabilidad->setText(QString::number( fProbabilidad, 'f', 8 ));
            loadDataSetInTableView(MMdistances, ui->tableView_Distances);
            loadDataSetInTableView(MMclass, ui->tableView_Class);
            loadDistHeadInTableView(MMsuspects, MMdataset, ui->tableView_DistancesRanges);
            ui->label_CalculateDistances->setText("Distances Calculated");
        }else{
            ui->label_CalculateDistances->setText("You must enter a valid K_VALUE and Pre Process option.");
        }
    }
}





///////////////////////////////////////////////////////////////////////////////////////////
// Clustering
///////////////////////////////////////////////////////////////////////////////////////////

//+-------------------------------------------------------------------------------------
// Botón para la carga del DataSet a Clusterizar.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::OpenFileCluster(void) {
    // Recojo el K_VALUE, en caso de que lo hayan introducido.
    int iKtest =  ui->lineEdit_KvalueCluster->text().toInt();
    if(iKtest > 0)
        K_VALUE = iKtest;
    else
        K_VALUE = 0;

    if(K_VALUE == 0){
        ui->tabWidget_Clustering->setCurrentIndex(0);
        ui->label_CalculateClustering->setText(
                    "First, you must select a valid K_VALUE.");
    }else{
        // Abro dialogo de seleccion de archivo.
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open arff File"), "C:/", tr("arff or txt Files (*.arff *.txt)"));

        // Me aseguro de tener creado los objetos, antes de cargarlos.
        if(MMCluster == NULL)
            MMCluster  = new clsMarsMiner;

        // NO IMPLEMENTADO PARA Clustering. Solo para K-NN, de momento.
        // establezco la propiedad de pre-proceso, si lo ha escogido en combobox.
        /*if(ui->comboBox_Preprocess->currentIndex() == 0)
            MMCluster->SetPreProceso(false);
        else
            MMCluster->SetPreProceso(true);*/

        ui->tabWidget_Clustering->setCurrentIndex(0);
        if(MMCluster->BuildDataset((char *)qPrintable(fileName)) == false){
            ui->lineEdit_OpenCluster->setText(
                        "Error loading DataSet to Cluster from selected file !!");
            clusteringLoaded = false;
        }else{
            ui->lineEdit_OpenCluster->setText(fileName);
            loadDataSetInTableView(MMCluster, ui->tableView_Cluster);
            clusteringLoaded = true;
            // Nuevo. Auto Cargo la CLASS_COLUMN que antes hacia a mano.
            if(CLASS_COLUMN == 0)
                // Todavía no ha sido establecida. La estbablezco al ultimo att.
                CLASS_COLUMN = MMsuspects->GetAttributes()->GetAttributesTotal()-1;
        }
    }
}


//+-------------------------------------------------------------------------------------
// Botón Calcula Clutstering.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::ClusteringCalculLauncher(void) {

    //if(ui->lineEdit_KvalueCluster->text() )
    // Recojo el K_VALUE, en caso de que lo hayan introducido.
    int iKtest =  ui->lineEdit_KvalueCluster->text().toInt();
    if(iKtest > 0 && iKtest < 8)
        K_VALUE = iKtest;
    else
        K_VALUE = 0;

    if(K_VALUE == 0){
        //ui->tabWidget->setCurrentIndex(2);
        ui->label_CalculateClustering->setText(
                    "First, you must select a valid K_VALUE.");
    }else{
        if(clusteringLoaded == false){
            // Msg: NO se puede realizar el calculo hasta tener el dataset.
            ui->label_CalculateClustering->setText(
                        "First, you must load dataset to Cluster.");
        }else{
            // Inicio clustering para la K y dataset especificados.
            KMeans(K_VALUE);
            DrawSolutions();
            ui->tabWidget_Clustering->setCurrentIndex(K_VALUE);

        }// Fin if(clusteringLoadeD)
    }// Fin if(K_VALUE)
}// ClusteringCalculLauncher




//+-------------------------------------------------------------------------------------
// Pre: Kmax debe estar entre 1 y 7 (ambos inclusive, 7de momento).
// Objetivo.
// Se ha de llegar a la estabilizacion de los centroids y valorar el porcentage de
// individuos que hay de una clase y de otra en cada centroid.
// P.ej. c1 60% Red, 40% Purple => Prob de agrupabilidad por clase 60/40.
//+-------------------------------------------------------------------------------------
Cluster ***MarsMinerMainWin::KMeans(int Kmax) {
    bool centroids_Estabilizados;
    float dist_MIN = 1000;                 // Valor que siempre sera superior la 1a vez.
    int cluster_MIN = -1;
    float fDistance = -1;
    int currentBarValue=0;

    int K=1;                // Inicio con K=1 clusters
    // Declaro un array de pointers a Clsuter para guardar las Soluciones.
    //Soluciones = (Cluster **)malloc(K_VALUE * sizeof(Cluster*));
    Soluciones = new Cluster**[Kmax];
    Cluster **clusters;// = new Cluster*[K];
    // establezco lo que será el 100% de la barra de progreso.
    ui->progressBar_clustering->setMaximum(MMCluster->GetDataSetTotalRegisters()*Kmax);
    while(K <= Kmax){       // K es la actual.
        // Calculo solucion cluster K.
        // La solucion K tiene K clusters que declaro con estos tamaños.
        clusters = new Cluster*[K];
        qDebug() << "AttributesTotal" << MMCluster->GetAttributes()->GetAttributesTotal() << Qt::endl;;
        // Creo los K cluster.
        for(int c=0; c<K; c++)
            clusters[c] = new Cluster(
                        MMCluster->GetAttributes()->GetAttributesTotal() - 1, // pq NO HAY QUE TENER EN CUENTA LA CLASE en el cluster
                        MMCluster->GetDataSetTotalRegisters(),
                        MMCluster->GetAttributes());
        // Inic centroid de cada cluster a valores random dentro del rango de su atributo.
        InicializaCentroids(K, clusters);
        qDebug() << "AttributesTotal Cluster" << clusters[K-1]->getTotalAttributes() << Qt::endl;;
        centroids_Estabilizados = false;
        while(!centroids_Estabilizados){
            for(int individuo=0; individuo < MMCluster->GetDataSetTotalRegisters();
                individuo++ ){
                // Para cada individuo del DataSet a clusterizar...
                dist_MIN = 1000;                    // Valor que siempre sera superior la 1a vez.
                cluster_MIN = -1;                   // Valor inicial que cambiará la 1a vez.
                for(int grupo=0; grupo < K; grupo++){
                    // Compruebo distancia de individuo con centroid de cada grupo/cluster.
                    fDistance = DistanceBetweenRows(MMCluster->DataSetGetRow(individuo),
                                                    clusters[grupo]->getCentroid(), 0,
                                                    clusters[grupo]->getTotalAttributes(), // - 1,
                                                    MMCluster->GetAttributes());
                    // Asigno al usuario al cluster con centroid que hace que sea mínima.
                    if(fDistance <= dist_MIN){
                        // Guardo nueva dist_MIN y el cluster con el que se ha dado.
                        dist_MIN = fDistance;
                        cluster_MIN = grupo;
                    }
                }// Ahora ya tengo distancia mínima y cluster asociado. Le agrego el individuo.
                clusters[cluster_MIN]->addIndividuoAndDistance(individuo, dist_MIN);
                clusters[cluster_MIN]->addToAvg(MMCluster->DataSetGetRow(individuo));
                // Gestiono barra de progreso.
                if(currentBarValue < individuo*K){
                    ui->progressBar_clustering->setValue(currentBarValue++);
                }
            }// Fin DS
            // Recalculo centroids y actualizo en la misma f(x) si hay alguno no estabilizado.
            centroids_Estabilizados = recalculaCentroids(K, clusters, MMCluster->GetAttributes());
            if(!centroids_Estabilizados){
                // limipo los K clusters
                for(int c=0; c<K; c++)
                    clusters[c]->clearCluster();
            }
        }// Fin while(!centroids)
        // Gestiono barra de progreso.
        if(currentBarValue < MMCluster->GetDataSetTotalRegisters()*K){
            currentBarValue = MMCluster->GetDataSetTotalRegisters()*K;
            ui->progressBar_clustering->setValue(currentBarValue);
        }
        // Guardo la solución actual numero K.
        Soluciones[K-1] = clusters;
        K += 1;
    }
    ui->progressBar_clustering->setValue(MMCluster->GetDataSetTotalRegisters()*Kmax);
    // Devuelvo las soluciones calculadas.
    return Soluciones;
}





//+-------------------------------------------------------------------------------------
// Inicializa los centroid de cada cluster del array de cluster que recibe.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::InicializaCentroids(int numCentroids, Cluster **clusters) {
    for(int i=0; i<numCentroids; i++)
        clusters[i]->initCentroid();
}



//+-------------------------------------------------------------------------------------
// Recibe las rows sobre las que aplicar la funcion de distancia, los indices de las
// columnas de start y end de los atributos, los atributos en los que se encuentran los
// valores Max y Min que tiene cada atributo, de manera que se pueda normalizar.
//+-------------------------------------------------------------------------------------
float MarsMinerMainWin::DistanceBetweenRows(float *rowA, float *rowB, int start, int end,
                                            clsAttribute *atts) {
    float fMax, fMin, fDistancia=0;
    //F(x) Distancia COPIADA DE K-NN
    double dDistancia=0;
    for(int col=start; col < end; col++){
        fMax = atts->GetAttributeMaxValue(col);
        fMin = atts->GetAttributeMinValue(col);
        // Calculo de la distancia entre individuos, para el attribute #col.
        if(!strcmp(atts->GetAttribute(col)->acType,
                 (char *)qPrintable("real"))){
          dDistancia += distanceNormal(rowA[col], rowB[col], fMax, fMin);
        }
    }// Fin cols.
    // Calculo raiz.
    fDistancia = (float)qSqrt(dDistancia);
    return fDistancia;
    //FIN   F(x) Distancia COPIADA DE K-NN
}// Fin DistanceBetweenRows()




//+-------------------------------------------------------------------------------------
// Devuelve false si el centroid no está estabilizado y ha sido necesario recalcular.
// Al recalcular, se guarda la media de los individuos del cluster como centroid.
//+-------------------------------------------------------------------------------------
bool MarsMinerMainWin::recalculaCentroids(int K, Cluster **clusters, clsAttribute *attts) {
    //float *fAverage = NULL;
    float *fCurrentCentroid = NULL;
    float fDistance;
    float **fMedias = new float*[K];
    bool centroidEstabilizado = true;

    for(int i=0; i<K; i++){
        // Calculo y recojo la media del cluster i.
        //fAverage = clusters[i]->getAvg();
        fMedias[i] = clusters[i]->getAvg();
        // Recojo valor del centroid del cluster i.
        fCurrentCentroid = clusters[i]->getCentroid();
        // Calculo distancia entre la media y el centroid.
        fDistance = DistanceBetweenRows(fMedias[i],
                                        fCurrentCentroid,
                                        0,
                                        clusters[i]->getTotalAttributes(),
                                        attts);
        qDebug() << "fDistance " << fDistance << "RECLA CENTROID " << RECALCUL_CENTROID_VALUE << Qt::endl;;
        if(fDistance > RECALCUL_CENTROID_VALUE){
            // Si la distancia entre ambos supera la constante establecida, recalculamos.
            centroidEstabilizado = false;
        }
        ShowCentroid(clusters[i]->getCentroid(), clusters[i]->getTotalAttributes());
    }
    if(!centroidEstabilizado){
        // Para recalcular, le asigno las medias de cada cluster a su centroid.
        for(int i=0; i<K; i++){
            clusters[i]->setCentroid(fMedias[i]);
            // incremento el numero de recalculos para este cluster.
            clusters[i]->incrRecalculalos();
            // TEST
            ShowCentroid(clusters[i]->getCentroid(), clusters[i]->getTotalAttributes());
        }
    }
    return centroidEstabilizado;
}



//+-------------------------------------------------------------------------------------
// Funcion de debug para visualizar el contenido del centroid.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::ShowCentroid(float *centroid, int numAtts) {
    for(int i=0; i<numAtts; i++)
        qDebug() << "att " << i << ": " << centroid[i] << Qt::endl;;
}



//+-------------------------------------------------------------------------------------
// Carga el array de clusters 'Soluciones' en los graphic view. Utiliza tantos Graphic
// View como soluciones tenga almacenadas.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::DrawSolutions(void) {
    Cluster **cluster = NULL;
    qreal x, y;
    QGraphicsView *m_graphView = NULL;
    QPen *pRed   = new  QPen(Qt::red, 0.5, Qt::SolidLine);
    QPen *pPurple = new  QPen(Qt::blue , 0.5, Qt::SolidLine);
    QPen *p = NULL;
    QFont clusterTextFont("Courier", 18, QFont::Bold, true);
    QString message;
    int clase=0, numReds, numPurples;
    // A los zeros les llamo reds, pero podría darse el caso de que no fuese asi.

    if(Soluciones == NULL)
        return;

    qDebug() << "KVALUE " << K_VALUE << Qt::endl;;
    for(int i=0; i<K_VALUE; i++){
        cluster = Soluciones[i];
        scene = new QGraphicsScene;
        if(i == 0)
            m_graphView = ui->graphicsView_Cluster1;
        else if(i == 1)
            m_graphView = ui->graphicsView_Cluster2;
        else if(i == 2)
            m_graphView = ui->graphicsView_Cluster3;
        else if(i == 3)
            m_graphView = ui->graphicsView_Cluster4;
        else if(i == 4)
            m_graphView = ui->graphicsView_Cluster5;
        else if(i == 5)
            m_graphView = ui->graphicsView_Cluster6;
        else if(i == 6)
            m_graphView = ui->graphicsView_Cluster7;

        m_graphView->setScene(scene);


        for(int c=0; c<=i; c++){
            // Pinto linea indica inicio metrica distancia para cluster c.
            y = m_graphView->height()-(ELLIPSE_SIZE*c*1.5);
            // Printo texto con nombre del cluster
            message = QString("cluster %1").arg(c);
            QGraphicsTextItem *myText = new QGraphicsTextItem(message);
            myText->setPos(DISPERSION_FACTOR*c - 150, y - 5);
            myText->setFont(clusterTextFont);
            // Agrego el texto al grafico
            scene->addItem(myText);
            scene->addLine(DISPERSION_FACTOR*c, y, DISPERSION_FACTOR*c,
                           y+ELLIPSE_SIZE,
                           QPen(QBrush(Qt::green),5));

            numReds = numPurples = 0;
            for(int ind=0; ind<cluster[c]->getTotalIndividuos(); ind++){
                x = cluster[c]->getDistance(ind)*DISTANCE_SCALE +
                        DISPERSION_FACTOR*c;
                // Recojo la clase.
                clase = MMCluster->DataSetGetRow(cluster[c]->getIndividuo(ind))
                        [cluster[c]->getTotalAttributes()]; //-1];
                qDebug() << "clase " << clase << "texto " <<
                            MMCluster->GetClass(clase) << Qt::endl;;
                //if(!strcmp(MMCluster->GetClass(clase), "Red")){
                if(clase == 1){
                    p=pRed;
                    numReds++;
                }else if(clase == 0){
                    p=pPurple;
                    numPurples++;
                }
                scene->addEllipse(x, y, ELLIPSE_SIZE, ELLIPSE_SIZE,
                                  *p,                   //QPen(Qt::blue, 0.25,Qt::SolidLine),
                                  QBrush(Qt::NoBrush)); //(Qt::black));
            }
            qDebug() << "inds: " << cluster[c]->getTotalIndividuos() << "reds "
                     << numReds << "purples " << numPurples << Qt::endl;;
            LoadSolutionData(i+1, c, numReds, numPurples, cluster[c]->getRecalculos());
        }
        m_graphView->show();
    }
}



//+-------------------------------------------------------------------------------------
// Función auxiliar a DrawSolutions() que carga los datos en los campos de la derecha
// habilitados en cada tab asociados a los dibujos del Graphic View adjunto.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::LoadSolutionData(int knum, int cluster,
                                        int reds, int purples, int recalculos) {

    //ui->label_K1_class0->setStyleSheet("QLineEdit{background: red;}");

    float f = getRate(reds, purples);
    qDebug() << "f " << f << Qt::endl;;
    if(knum == 1){
        // K==1 graphic view objects
        ui->label_K1_class0->setText(MMCluster->GetClass(0));
        ui->label_K1_class1->setText(MMCluster->GetClass(1));
        ui->lineEdit_K1_class0->setText(QString("%1").arg(purples));
        ui->lineEdit_K1_class1->setText(QString("%1").arg(reds));
        ui->lineEdit_K1_rate->setText(QString::number(f, 'f', 2)); //<< QString::number(f, 'g', 2));
        ui->lineEdit_K1_recalculate->setText(QString::number(recalculos));
    }else if(knum == 2) {
        // K==2 graphic view objects
        if(cluster == 0){
            ui->label_K2_class0_c0->setText(MMCluster->GetClass(0));
            ui->label_K2_class1_c0->setText(MMCluster->GetClass(1));
            ui->lineEdit_K2_class0_c0->setText(QString("%1").arg(purples));
            ui->lineEdit_K2_class1_c0->setText(QString("%1").arg(reds));
            ui->lineEdit_K2_rate_c0->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K2_recalculate_c0->setText(QString::number(recalculos));
        }else if(cluster == 1) {
            ui->label_K2_class0_c1->setText(MMCluster->GetClass(0));
            ui->label_K2_class1_c1->setText(MMCluster->GetClass(1));
            ui->lineEdit_K2_class0_c1->setText(QString("%1").arg(purples));
            ui->lineEdit_K2_class1_c1->setText(QString("%1").arg(reds));
            ui->lineEdit_K2_rate_c1->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K2_recalculate_c1->setText(QString::number(recalculos));
        }

    }else if(knum == 3) {
        // K==3 graphic view objects
        if(cluster == 0){
            ui->label_K3_class0_c0->setText(MMCluster->GetClass(0));
            ui->label_K3_class1_c0->setText(MMCluster->GetClass(1));
            ui->lineEdit_K3_class0_c0->setText(QString("%1").arg(purples));
            ui->lineEdit_K3_class1_c0->setText(QString("%1").arg(reds));
            ui->lineEdit_K3_rate_c0->setText(QString::number(f, 'f', 2)); // <<QString::number(f, 'g', 2));
            ui->lineEdit_K3_recalculate_c0->setText(QString::number(recalculos));

        }else if(cluster == 1) {
            ui->label_K3_class0_c1->setText(MMCluster->GetClass(0));
            ui->label_K3_class1_c1->setText(MMCluster->GetClass(1));
            ui->lineEdit_K3_class0_c1->setText(QString("%1").arg(purples));
            ui->lineEdit_K3_class1_c1->setText(QString("%1").arg(reds));
            ui->lineEdit_K3_rate_c1->setText(QString::number(f, 'f', 2)); //<<QString::number(f, 'g', 2));
            ui->lineEdit_K3_recalculate_c1->setText(QString::number(recalculos));

        }else if(cluster == 2) {
            ui->label_K3_class0_c2->setText(MMCluster->GetClass(0));
            ui->label_K3_class1_c2->setText(MMCluster->GetClass(1));
            ui->lineEdit_K3_class0_c2->setText(QString("%1").arg(purples));
            ui->lineEdit_K3_class1_c2->setText(QString("%1").arg(reds));
            ui->lineEdit_K3_rate_c2->setText(QString::number(f, 'f', 2)); // <<QString::number(f, 'g', 2));
            ui->lineEdit_K3_recalculate_c2->setText(QString::number(recalculos));

        }

    }else if(knum == 4) {
        // K==3 graphic view objects
        if(cluster == 0){
            ui->label_K4_class0_c0->setText(MMCluster->GetClass(0));
            ui->label_K4_class1_c0->setText(MMCluster->GetClass(1));
            ui->lineEdit_K4_class0_c0->setText(QString("%1").arg(purples));
            ui->lineEdit_K4_class1_c0->setText(QString("%1").arg(reds));
            ui->lineEdit_K4_rate_c0->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K4_recalculate_c0->setText(QString::number(recalculos));

        }else if(cluster == 1) {
            ui->label_K4_class0_c1->setText(MMCluster->GetClass(0));
            ui->label_K4_class1_c1->setText(MMCluster->GetClass(1));
            ui->lineEdit_K4_class0_c1->setText(QString("%1").arg(purples));
            ui->lineEdit_K4_class1_c1->setText(QString("%1").arg(reds));
            ui->lineEdit_K4_rate_c1->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K4_recalculate_c1->setText(QString::number(recalculos));

        }else if(cluster == 2) {
            ui->label_K4_class0_c2->setText(MMCluster->GetClass(0));
            ui->label_K4_class1_c2->setText(MMCluster->GetClass(1));
            ui->lineEdit_K4_class0_c2->setText(QString("%1").arg(purples));
            ui->lineEdit_K4_class1_c2->setText(QString("%1").arg(reds));
            ui->lineEdit_K4_rate_c2->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K4_recalculate_c2->setText(QString::number(recalculos));

        }else if(cluster == 3) {
            ui->label_K4_class0_c3->setText(MMCluster->GetClass(0));
            ui->label_K4_class1_c3->setText(MMCluster->GetClass(1));
            ui->lineEdit_K4_class0_c3->setText(QString("%1").arg(purples));
            ui->lineEdit_K4_class1_c3->setText(QString("%1").arg(reds));
            ui->lineEdit_K4_rate_c3->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K4_recalculate_c3->setText(QString::number(recalculos));

        }
    }else if(knum == 5) {
        // K==3 graphic view objects
        if(cluster == 0){
            ui->label_K5_class0_c0->setText(MMCluster->GetClass(0));
            ui->label_K5_class1_c0->setText(MMCluster->GetClass(1));
            ui->lineEdit_K5_class0_c0->setText(QString("%1").arg(purples));
            ui->lineEdit_K5_class1_c0->setText(QString("%1").arg(reds));
            ui->lineEdit_K5_rate_c0->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K5_recalculate_c0->setText(QString::number(recalculos));

        }else if(cluster == 1) {
            ui->label_K5_class0_c1->setText(MMCluster->GetClass(0));
            ui->label_K5_class1_c1->setText(MMCluster->GetClass(1));
            ui->lineEdit_K5_class0_c1->setText(QString("%1").arg(purples));
            ui->lineEdit_K5_class1_c1->setText(QString("%1").arg(reds));
            ui->lineEdit_K5_rate_c1->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K5_recalculate_c1->setText(QString::number(recalculos));

        }else if(cluster == 2) {
            ui->label_K5_class0_c2->setText(MMCluster->GetClass(0));
            ui->label_K5_class1_c2->setText(MMCluster->GetClass(1));
            ui->lineEdit_K5_class0_c2->setText(QString("%1").arg(purples));
            ui->lineEdit_K5_class1_c2->setText(QString("%1").arg(reds));
            ui->lineEdit_K5_rate_c2->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K5_recalculate_c2->setText(QString::number(recalculos));

        }else if(cluster == 3) {
            ui->label_K5_class0_c3->setText(MMCluster->GetClass(0));
            ui->label_K5_class1_c3->setText(MMCluster->GetClass(1));
            ui->lineEdit_K5_class0_c3->setText(QString("%1").arg(purples));
            ui->lineEdit_K5_class1_c3->setText(QString("%1").arg(reds));
            ui->lineEdit_K5_rate_c3->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K5_recalculate_c3->setText(QString::number(recalculos));

        }else if(cluster == 4) {
            ui->label_K5_class0_c4->setText(MMCluster->GetClass(0));
            ui->label_K5_class1_c4->setText(MMCluster->GetClass(1));
            ui->lineEdit_K5_class0_c4->setText(QString("%1").arg(purples));
            ui->lineEdit_K5_class1_c4->setText(QString("%1").arg(reds));
            ui->lineEdit_K5_rate_c4->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K5_recalculate_c4->setText(QString::number(recalculos));

        }
    }else if(knum == 6) {
        // K==3 graphic view objects
        if(cluster == 0){
            ui->label_K6_class0_c0->setText(MMCluster->GetClass(0));
            ui->label_K6_class1_c0->setText(MMCluster->GetClass(1));
            ui->lineEdit_K6_class0_c0->setText(QString("%1").arg(purples));
            ui->lineEdit_K6_class1_c0->setText(QString("%1").arg(reds));
            ui->lineEdit_K6_rate_c0->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K6_recalculate_c0->setText(QString::number(recalculos));

        }else if(cluster == 1) {
            ui->label_K6_class0_c1->setText(MMCluster->GetClass(0));
            ui->label_K6_class1_c1->setText(MMCluster->GetClass(1));
            ui->lineEdit_K6_class0_c1->setText(QString("%1").arg(purples));
            ui->lineEdit_K6_class1_c1->setText(QString("%1").arg(reds));
            ui->lineEdit_K6_rate_c1->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K6_recalculate_c1->setText(QString::number(recalculos));

        }else if(cluster == 2) {
            ui->label_K6_class0_c2->setText(MMCluster->GetClass(0));
            ui->label_K6_class1_c2->setText(MMCluster->GetClass(1));
            ui->lineEdit_K6_class0_c2->setText(QString("%1").arg(purples));
            ui->lineEdit_K6_class1_c2->setText(QString("%1").arg(reds));
            ui->lineEdit_K6_rate_c2->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K6_recalculate_c2->setText(QString::number(recalculos));

        }else if(cluster == 3) {
            ui->label_K6_class0_c3->setText(MMCluster->GetClass(0));
            ui->label_K6_class1_c3->setText(MMCluster->GetClass(1));
            ui->lineEdit_K6_class0_c3->setText(QString("%1").arg(purples));
            ui->lineEdit_K6_class1_c3->setText(QString("%1").arg(reds));
            ui->lineEdit_K6_rate_c3->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K6_recalculate_c3->setText(QString::number(recalculos));

        }else if(cluster == 4) {
            ui->label_K6_class0_c4->setText(MMCluster->GetClass(0));
            ui->label_K6_class1_c4->setText(MMCluster->GetClass(1));
            ui->lineEdit_K6_class0_c4->setText(QString("%1").arg(purples));
            ui->lineEdit_K6_class1_c4->setText(QString("%1").arg(reds));
            ui->lineEdit_K6_rate_c4->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K6_recalculate_c4->setText(QString::number(recalculos));

        }else if(cluster == 5) {
            ui->label_K6_class0_c5->setText(MMCluster->GetClass(0));
            ui->label_K6_class1_c5->setText(MMCluster->GetClass(1));
            ui->lineEdit_K6_class0_c5->setText(QString("%1").arg(purples));
            ui->lineEdit_K6_class1_c5->setText(QString("%1").arg(reds));
            ui->lineEdit_K6_rate_c5->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K6_recalculate_c5->setText(QString::number(recalculos));

        }
    }else if(knum == 7) {
        // K==3 graphic view objects
        if(cluster == 0){
            ui->label_K7_class0_c0->setText(MMCluster->GetClass(0));
            ui->label_K7_class1_c0->setText(MMCluster->GetClass(1));
            ui->lineEdit_K7_class0_c0->setText(QString("%1").arg(purples));
            ui->lineEdit_K7_class1_c0->setText(QString("%1").arg(reds));
            ui->lineEdit_K7_rate_c0->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K7_recalculate_c0->setText(QString::number(recalculos));

        }else if(cluster == 1) {
            ui->label_K7_class0_c1->setText(MMCluster->GetClass(0));
            ui->label_K7_class1_c1->setText(MMCluster->GetClass(1));
            ui->lineEdit_K7_class0_c1->setText(QString("%1").arg(purples));
            ui->lineEdit_K7_class1_c1->setText(QString("%1").arg(reds));
            ui->lineEdit_K7_rate_c1->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K7_recalculate_c1->setText(QString::number(recalculos));

        }else if(cluster == 2) {
            ui->label_K7_class0_c2->setText(MMCluster->GetClass(0));
            ui->label_K7_class1_c2->setText(MMCluster->GetClass(1));
            ui->lineEdit_K7_class0_c2->setText(QString("%1").arg(purples));
            ui->lineEdit_K7_class1_c2->setText(QString("%1").arg(reds));
            ui->lineEdit_K7_rate_c2->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K7_recalculate_c2->setText(QString::number(recalculos));

        }else if(cluster == 3) {
            ui->label_K7_class0_c3->setText(MMCluster->GetClass(0));
            ui->label_K7_class1_c3->setText(MMCluster->GetClass(1));
            ui->lineEdit_K7_class0_c3->setText(QString("%1").arg(purples));
            ui->lineEdit_K7_class1_c3->setText(QString("%1").arg(reds));
            ui->lineEdit_K7_rate_c3->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K7_recalculate_c3->setText(QString::number(recalculos));

        }else if(cluster == 4) {
            ui->label_K7_class0_c4->setText(MMCluster->GetClass(0));
            ui->label_K7_class1_c4->setText(MMCluster->GetClass(1));
            ui->lineEdit_K7_class0_c4->setText(QString("%1").arg(purples));
            ui->lineEdit_K7_class1_c4->setText(QString("%1").arg(reds));
            ui->lineEdit_K7_rate_c4->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K7_recalculate_c4->setText(QString::number(recalculos));

        }else if(cluster == 5) {
            ui->label_K7_class0_c5->setText(MMCluster->GetClass(0));
            ui->label_K7_class1_c5->setText(MMCluster->GetClass(1));
            ui->lineEdit_K7_class0_c5->setText(QString("%1").arg(purples));
            ui->lineEdit_K7_class1_c5->setText(QString("%1").arg(reds));
            ui->lineEdit_K7_rate_c5->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K7_recalculate_c5->setText(QString::number(recalculos));

        }else if(cluster == 6) {
            ui->label_K7_class0_c6->setText(MMCluster->GetClass(0));
            ui->label_K7_class1_c6->setText(MMCluster->GetClass(1));
            ui->lineEdit_K7_class0_c6->setText(QString("%1").arg(purples));
            ui->lineEdit_K7_class1_c6->setText(QString("%1").arg(reds));
            ui->lineEdit_K7_rate_c6->setText(QString::number(f, 'f', 2));
            ui->lineEdit_K7_recalculate_c6->setText(QString::number(recalculos));

        }
    }
}



//+-------------------------------------------------------------------------------------
// Funcion auxiliar a LoadSolutionData() que calcula el porcentaje que se ha de mostrar
// en los campos adjuntos a cada Graphic View.
//+-------------------------------------------------------------------------------------
float MarsMinerMainWin::getRate(int numReds, int numPurples) {
    float value = 0;
    float total = numReds + numPurples;
    if(numReds == 0 || numPurples == 0) return 100;
    if(numReds>numPurples){
        value = ((float)numReds)/total*100;
        //qDebug() << "value " << value << Qt::endl;;
        return value; //((float)numPurples/(float)numReds)*100;
    }else{
        value = ((float)numPurples)/total*100;
        //qDebug() << "value " << value << Qt::endl;;
        return value; //((float)numReds/(float)numPurples)*100;
    }
}



//+-------------------------------------------------------------------------------------
// SLOT's para el Menu de Configuration->Clustering de la barra de menú principal.
//+-------------------------------------------------------------------------------------
void MarsMinerMainWin::Ellipse20(void) {
    ELLIPSE_SIZE = 20;
    DrawSolutions();
}
void MarsMinerMainWin::Ellipse40(void) {
    ELLIPSE_SIZE = 40;
    DrawSolutions();
}
void MarsMinerMainWin::Ellipse60(void) {
    ELLIPSE_SIZE = 60;
    DrawSolutions();
}
void MarsMinerMainWin::Ellipse80(void) {
    ELLIPSE_SIZE = 80;
    DrawSolutions();
}
void MarsMinerMainWin::Ellipse100(void) {
    ELLIPSE_SIZE = 100;
    DrawSolutions();
}
void MarsMinerMainWin::Scale100(void) {
    DISTANCE_SCALE = 100;
    DrawSolutions();
}
void MarsMinerMainWin::Scale200(void) {
    DISTANCE_SCALE = 200;
    DrawSolutions();
}
void MarsMinerMainWin::Scale300(void) {
    DISTANCE_SCALE = 300;
    DrawSolutions();
}
void MarsMinerMainWin::Scale400(void) {
    DISTANCE_SCALE = 400;
    DrawSolutions();
}
void MarsMinerMainWin::Scale500(void) {
    DISTANCE_SCALE = 500;
    DrawSolutions();
}

void MarsMinerMainWin::dialChange(int dial) {
    double value = 0;
    value = (double)dial/ui->dial_distance->maximum();
    RECALCUL_CENTROID_VALUE = value;    // Guardo nuevo valor
    ui->doubleSpinBox_distance->setValue(value);
}
void MarsMinerMainWin::spinChange(double spin) {
    int value = 0;
    value = (int)(spin*ui->dial_distance->maximum());
    RECALCUL_CENTROID_VALUE = spin;     // Guardo nuevo valor
    ui->dial_distance->setValue(value);
}


//+-------------------------------------------------------------------------------------
//+-------------------------------------------------------------------------------------
// end. hidalgoss, Barcelona 16 de Diciembre de 2012.
//+-------------------------------------------------------------------------------------
//+-------------------------------------------------------------------------------------



















/*scene = new QGraphicsScene;
//scene->setSceneRect(0,0,790,290);
QGraphicsView *m_graphView = ui->graphicsView_Cluster1;
//m_graphView->setFixedSize(800, 300);
m_graphView->setScene(scene);
scene->addLine(0, 250, 700, 250, QPen(QBrush(Qt::black),1));
scene->addEllipse(100, 100, 10, 10, QPen(Qt::blue,4,Qt::SolidLine), QBrush(Qt::black));
m_graphView->show();*/


/*//F(x) Distancia COPIADA DE K-NN
  // Empiezo desde la 2 ya que en 0 y 1 estan ID y Date. Ultimo Class tampoco.
  for(col=2, dDistancia=0; col < (iTotalAttributes - 1); col++){
      // Cogemos el máximo Max de los dos datasets.
      if(MMsuspects->GetAttributes()->GetAttributeMaxValue(col) >
              MMdataset->GetAttributes()->GetAttributeMaxValue(col))
          fMax = MMsuspects->GetAttributes()->GetAttributeMaxValue(col);
      else
          fMax = MMdataset->GetAttributes()->GetAttributeMaxValue(col);
      // Cogemos el mínimo Min de los dos datasets
      if(MMsuspects->GetAttributes()->GetAttributeMinValue(col) <
              MMdataset->GetAttributes()->GetAttributeMinValue(col))
          fMin = MMsuspects->GetAttributes()->GetAttributeMinValue(col);
      else
          fMin = MMdataset->GetAttributes()->GetAttributeMinValue(col);
      // Calculo de la distancia entre individuos, para el attribute #col.
      if(!strcmp(MMsuspects->GetAttributes()->GetAttribute(col)->acType,
                 (char *)qPrintable("real"))){
          dDistancia += distanceNormal(MMsuspects->DataSetGetRow(filSu)[col],
                                       MMdataset->DataSetGetRow(filDs)[col],
                                       fMax, fMin);
      }
  }// Fin cols. Fin individuo.

  // Miro si distancia de individuo hay que añadirla entre los K individuos.
  dDistancia = qSqrt(dDistancia);
  // Si este individuo del training se parece, guardo su distancia y clase.
  feasibleDistance(MMdistances, MMclass, filSu,
                   MMdataset->DataSetGetRow(filDs)[CLASS_COLUMN],
                   dDistancia);
//FIN   F(x) Distancia COPIADA DE K-NN*/






//+-------------------------------------------------------------------------------------
/*
void MarsMinerMainWin::IterativeDistanceWithAutoAttributeSelection(void){
    int filSu, filDs, col, iTotalAttributes, iAciertos, iProgress,
            iTotalRegistersSuspects, iTotalRegistersDataset, iIndividuo;
    double dDistancia = 0;
    float fMax=0, fMin=0, fProbabilidad=0.0;

    ui->tabWidget->setCurrentIndex(2);
    ui->lineEdit_Kvalue->setFocus();

    if(suspectsLoaded == false || datasetLoaded == false){
        // Msg: NO se puede realizar el calculo hasta tener los datasets.
        ui->label_CalculateDistances->setText(
                    "First, you must load Suspects and Training datasets.");

    }else{
        iTotalAttributes        = MMsuspects->GetAttributes()->GetAttributesTotal();
        iTotalRegistersSuspects = MMsuspects->GetDataSetTotalRegisters();
        iTotalRegistersDataset  = MMdataset->GetDataSetTotalRegisters();

        // Recojo el K_VALUE, en caso de que lo hayan introducido.
        int iKtest =  ui->lineEdit_Kvalue->text().toInt();
        if(iKtest > 0 && iKtest < iTotalRegistersSuspects)  K_VALUE = iKtest;
        else K_VALUE = 0;

        if(K_VALUE > 0){
            ui->label_CalculateDistances->setText("Calculating distances...");

            // CREO ARRAY A 0 DE FIJADOS
            int *piFijados = new int[iTotalAttributes];

            // Inicializo array de fijados a 0.
            InicializaArray(piFijados,iTotalAttributes, 0);
            // Calculo de los attributes que dan mayor Probabilidad de aciertos.
            ////fProbabilidad = GreedyIterative(piFijados, iTotalAttributes);
            // Ahora tengo dataset de distances y class cargados con los resultados.

            fProbabilidad = GreedyIterativeAuto(piFijados, iTotalAttributes);

            // deberia cargarme los resultados de la combinacion piFijados en dist y class.
            float fTestP = K_NN_WithMask(piFijados, MMdistances, MMclass);

            //MONTO LAS CABECERAS DE DISTANCES Y CLASS
        //// Preparo los Header del DataSet de Distancias para los K individuos.
            QString qsIndividuo;
            for(col=0; col < K_VALUE; col++){
                qsIndividuo = QString("Individuo: %1").arg(col);
                MMdistances->GetAttributes()->SetAttribute((char *)qPrintable(qsIndividuo),
                                                           "distance", 0, 0, col);
                qsIndividuo = QString("Class: %1").arg(col);
                MMclass->GetAttributes()->SetAttribute((char *)qPrintable(qsIndividuo),
                                                       "class", 0, 0, col);
            }
            // Agrego los atributos extra al DS de distancias.
            MMdistances->GetAttributes()->SetAttribute("Danger 1-0 Good","bin",0,0,col);
            MMdistances->GetAttributes()->SetAttribute("Probability", "real", 0, 0, col + 1);

            // Establezco el numero de columnas de los dataset de distancia y clase.
            MMdistances->GetAttributes()->SetAttributesTotal(K_VALUE + 2);
            MMclass->GetAttributes()->SetAttributesTotal(K_VALUE);
        //// Fin Preparo headers de distancia y class.


            //MUESTRO LOS RESULTADOS
            ui->lineEdit_Probabilidad->setText(QString::number( fProbabilidad, 'f', 8 ));
            loadDataSetInTableView(MMdistances, ui->tableView_Distances);
            loadDataSetInTableView(MMclass, ui->tableView_Class);
            loadDistHeadInTableViewWithMask(MMsuspects, MMdataset,
                                            ui->tableView_DistancesRanges, piFijados);
            ui->label_CalculateDistances->setText("Distances Calculated");

            delete [] piFijados;
        }else{
            ui->label_CalculateDistances->setText("You must enter a valid K_VALUE and Pre Process option.");
        }
    }
}
*/


//+-------------------------------------------------------------------------------------
/*
void MarsMinerMainWin::BackTrackingDistanceWithAutoAttributeSelection(void){
    int filSu, filDs, col, iTotalAttributes, iAciertos, iProgress,
            iTotalRegistersSuspects, iTotalRegistersDataset, iIndividuo;
    double dDistancia = 0;
    float fMax=0, fMin=0, fProbabilidad=0.0;

    ui->tabWidget->setCurrentIndex(2);
    ui->lineEdit_Kvalue->setFocus();
    if(suspectsLoaded == false || datasetLoaded == false){
        // Mesnsaje conforme no se puede realizar el calculo hasta tener los datasets.
        ui->label_CalculateDistances->setText("First, you must load Suspects and Training datasets.");
    }else{
        iTotalAttributes        = MMsuspects->GetAttributes()->GetAttributesTotal();
        iTotalRegistersSuspects = MMsuspects->GetDataSetTotalRegisters();
        iTotalRegistersDataset  = MMdataset->GetDataSetTotalRegisters();

        // Recojo el K_VALUE, en caso de que lo hayan introducido.
        int iKtest =  ui->lineEdit_Kvalue->text().toInt();
        if(iKtest > 0 && iKtest < iTotalRegistersSuspects)
            K_VALUE = iKtest;
        else
            K_VALUE = 0;

        if(K_VALUE > 0){
            // CREO ARRAY A 0 DE FIJADOS
            int *piFijados = new int(iTotalAttributes);

            //double maximo = qPow(10, 10)/1000000;
            //ui->progressBar_distances->setMaximum(maximo);
            //ui->progressBar_distances->setValue(0);

            // Inicializo array de fijados a 0.
            InicializaArray(piFijados,iTotalAttributes, 0);
            // Inicio el calculo de los attributes que dan mayor Probabilidad de aciertos.
            fProbabilidad = BckBestProb(piFijados);
            // Ahora tengo dataset de distances y class cargados con los resultados.

            //MONTO LAS CABECERAS DE DISTANCES Y CLASS
        //// Preparo los Header del DataSet de Distancias para los K individuos.
            QString qsIndividuo;
            for(col=0; col < K_VALUE; col++){
                qsIndividuo = QString("Individuo: %1").arg(col);
                MMdistances->GetAttributes()->SetAttribute((char *)qPrintable(qsIndividuo),
                                                           "distance", 0, 0, col);
                qsIndividuo = QString("Class: %1").arg(col);
                MMclass->GetAttributes()->SetAttribute((char *)qPrintable(qsIndividuo),
                                                       "class", 0, 0, col);
            }
            // Agrego los atributos extra al DS de distancias.
            MMdistances->GetAttributes()->SetAttribute("Danger 1-0 Good","bin",0,0,col);
            MMdistances->GetAttributes()->SetAttribute("Probability", "real", 0, 0, col + 1);

            // Establezco el numero de columnas de los dataset de distancia y clase.
            MMdistances->GetAttributes()->SetAttributesTotal(K_VALUE + 2);
            MMclass->GetAttributes()->SetAttributesTotal(K_VALUE);
        //// Fin Preparo headers de distancia y class.

            // utilizo esta f(x) para cargar distances y class.
            fProbabilidad = K_NN_WithMask(piFijados, MMdistances, MMclass);

            //MUESTRO LOS RESULTADOS
            ui->lineEdit_Probabilidad->setText(QString::number( fProbabilidad, 'f', 8 ));
            loadDataSetInTableView(MMdistances, ui->tableView_Distances);
            loadDataSetInTableView(MMclass, ui->tableView_Class);
            loadDistHeadInTableView(MMsuspects, MMdataset, ui->tableView_DistancesRanges);
            ui->label_CalculateDistances->setText("Distances Calculated");
        }else{
            ui->label_CalculateDistances->setText("You must enter a valid K_VALUE and Pre Process option.");
        }
    }
    //return 1; //TEST
}
*/


//+-------------------------------------------------------------------------------------
/*
void MarsMinerMainWin::RecursiveDistanceWithAutoAttributeSelection(void){
    int filSu, filDs, col, iTotalAttributes, iAciertos, iProgress,
            iTotalRegistersSuspects, iTotalRegistersDataset, iIndividuo;
    double dDistancia = 0;
    float fMax=0, fMin=0, fProbabilidad=0.0;

    ui->tabWidget->setCurrentIndex(2);
    ui->lineEdit_Kvalue->setFocus();
    if(suspectsLoaded == false || datasetLoaded == false){
        // Mesnsaje conforme no se puede realizar el calculo hasta tener los datasets.
        ui->label_CalculateDistances->setText("First, you must load Suspects and Training datasets.");
    }else{
        iTotalAttributes        = MMsuspects->GetAttributes()->GetAttributesTotal();
        iTotalRegistersSuspects = MMsuspects->GetDataSetTotalRegisters();
        iTotalRegistersDataset  = MMdataset->GetDataSetTotalRegisters();

        // Recojo el K_VALUE, en caso de que lo hayan introducido.
        int iKtest =  ui->lineEdit_Kvalue->text().toInt();
        if(iKtest > 0 && iKtest < iTotalRegistersSuspects)
            K_VALUE = iKtest;
        else
            K_VALUE = 0;

        if(K_VALUE > 0){





            ui->lineEdit_Probabilidad->setText(QString::number( fProbabilidad, 'f', 8 ));
            loadDataSetInTableView(MMdistances, ui->tableView_Distances);
            loadDataSetInTableView(MMclass, ui->tableView_Class);
            loadDistHeadInTableView(MMsuspects, MMdataset, ui->tableView_DistancesRanges);
            ui->label_CalculateDistances->setText("Distances Calculated");
        }else{
            ui->label_CalculateDistances->setText("You must enter a valid K_VALUE and Pre Process option.");
        }
    }
    //return 1; //TEST
}
*/




//+-------------------------------------------------------------------------------------
// Open File function. Falta que funcione desde llamada de SLOT con parametro.
//+-------------------------------------------------------------------------------------
/*
void MarsMinerMainWin::OpenFile(int iDataset){

    clsMarsMiner *MM = NULL;
    if(iDataset == SUSPECTS)
        MM = MMsuspects;
    else
        MM = MMdataset;
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open arff File"), "C:/", tr("arff or txt Files (*.arff *.txt)"));
    if(MMdataset->BuildDataset((char *)qPrintable(fileName)) == false)
        ui->lineEdit_OpenSuspects->setText("Error loading DataSet from selected file !!");
    else{
        //qDebug() << "hola" << fileName << "adios";
        ui->lineEdit_OpenSuspects->setText(fileName);
        loadDataSetInTableView(MM, ui->tableView_Suspects);
    }
}*/




/* Ejemplo tableView

QAbstractItemModel *model = new QStandardItemModel(9, 3, this);
model->setHeaderData(0, Qt::Horizontal, tr("Header 0"));
model->setHeaderData(1, Qt::Horizontal, tr("Header 1"));
model->setHeaderData(2, Qt::Horizontal, tr("Header 2"));
for(int fil=0; fil<9; fil++){
    for(int col=0; col<3; col++){
        QModelIndex index = model->index(fil, col, QModelIndex());
        QString cad = QString("datos de col: %1 %2.").arg(col).arg(fil);
        //QString cad = "datos de col: " + col + " y fil: " + fil + " ";
        //QVariant cad = "datos de col: " + col + " y fil: " + fil + " ";
        model->setData(index,  cad);
    }
}
ui->tableView_Suspects->setModel(model);
*/



//QString("%1").arg(col).arg(fil);
