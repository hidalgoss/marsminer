#include "marsminermainwin.h"
#include "ui_marsminermainwin.h"

#define SUSPECTS    1
#define DATASET     2
//#define K_VALUE     7       // Valor que le damos a K.
#define CLASS_COLUMN    9   // Columna del attributo binario. OJO pq lo ponemos a mano.
#define K_EXTRA_COLS    2   // cols extra aparte de las K cols. 2 para clase y probabilidad.



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

    K_VALUE = 0;

    suspectsLoaded  = false;        // se pone a true si se han cargado bien.
    datasetLoaded   = false;
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
            //qDebug() << "hola" << fileName << "adios";
            ui->lineEdit_OpenSuspects->setText(fileName);
            loadDataSetInTableView(MMsuspects, ui->tableView_Suspects);
            suspectsLoaded = true;
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
                       (char *)qPrintable("real")))
                cad = QString::number( MM->DataSetGetRow(fil)[col], 'f', 8 );
            else if(!strcmp(MM->GetAttributes()->GetAttribute(col)->acType,
                            (char *)qPrintable("bin")))
                cad = QString::number( MM->DataSetGetRow(fil)[col]);
            else if(!strcmp(MM->GetAttributes()->GetAttribute(col)->acType,
                            (char *)qPrintable("distance")))
                cad = QString::number( MM->DataSetGetRow(fil)[col], 'f', 8 );
            else if(!strcmp(MM->GetAttributes()->GetAttribute(col)->acType,
                            (char *)qPrintable("class")))
                cad = QString::number( MM->DataSetGetRow(fil)[col]);
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
            iTotalRegistersSuspects, iTotalRegistersDataset, iIndividuo;
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
                for(filDs=0, iIndividuo=0; filDs< iTotalRegistersDataset; filDs++){
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
        //qDebug() << " -- " << MM->DataSetGetRow(iIndividuo)[col] << endl;
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
        return qPow((fX - fY), 2);
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

    // Visualizo ls QTableView ya vacios.
    ui->tableView_Suspects->repaint();
    ui->tableView_Dataset->repaint();
    ui->tableView_Distances->repaint();
    ui->tableView_DistancesRanges->repaint();
    ui->tableView_Class->repaint();

    // Elimino los objetos utilizados.
    if(MMsuspects != NULL)
        delete MMsuspects;
    if(MMdataset != NULL)
        delete MMdataset;
    if(MMdistances != NULL)
        delete MMdistances;
    if(MMclass != NULL)
        delete MMclass;

    // Los inicializo a NULL para pedir, si es el caso, de nuevo el objeto.
    MMsuspects  = NULL;
    MMdataset   = NULL;
    MMdistances = NULL;
    MMclass     = NULL;
    // Le indico a loadDistanceInTableView() que los DS no estan cargados.
    suspectsLoaded  = false;
    datasetLoaded   = false;
    K_VALUE = 0;

    // inicializamos resto de elementos.
    ui->lineEdit_Probabilidad->clear();
    ui->lineEdit_OpenSuspects->clear();
    ui->lineEdit_OpenDataset->clear();
    ui->label_CalculateDistances->setText("Datasets successfully cleared.");
    ui->progressBar_distances->setValue(0);
    ui->lineEdit_Kvalue->clear();
    ui->tabWidget->setCurrentIndex(2);
    ui->lineEdit_Kvalue->setFocus();
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
    int col, iTotalAttributes, iTotalRegistersSuspects, iTotalRegistersDataset;
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
        iTotalRegistersDataset  = MMdataset->GetDataSetTotalRegisters();

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
                // TEST
                ////qDebug() << "piMask: ";
                ////for(int i=0; i<iTotalAttributes; i++)
                ////    qDebug() << piMask[i];
                // FIN_TEST

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

    int filSu, filDs, col, iTotalAttributes, iAciertos, iProgress,
            iTotalRegistersSuspects, iTotalRegistersDataset, iIndividuo;
    double dDistancia = 0;
    float fMax=0, fMin=0, fProbabilidad=0.0;

    // Establezco el numero de columnas de los dataset de distancia y clase.
    MMdst->GetAttributes()->SetAttributesTotal(K_VALUE + 2);
    MMcls->GetAttributes()->SetAttributesTotal(K_VALUE);

    // Guardo tamaños dataset en vars.
    iTotalAttributes        = MMsuspects->GetAttributes()->GetAttributesTotal();
    iTotalRegistersSuspects = MMsuspects->GetDataSetTotalRegisters();
    iTotalRegistersDataset  = MMdataset->GetDataSetTotalRegisters();

    for(filSu=0, iAciertos=0, iProgress = 1; filSu< iTotalRegistersSuspects;
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
        for(filDs=0, iIndividuo=0; filDs< iTotalRegistersDataset; filDs++){
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

    int filSu, filDs, col, iTotalAttributes, iAciertos, iProgress,
            iTotalRegistersSuspects, iTotalRegistersDataset, iIndividuo;
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

    for(filSu=0, iAciertos=0, iProgress = 1; filSu< iTotalRegistersSuspects;
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
        for(filDs=0, iIndividuo=0; filDs< iTotalRegistersDataset; filDs++){
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

            // TEST
            //qDebug() << "piMask: ";
            //for(int i=0; i<iTotalAttributes; i++)
            //    qDebug() << aiMask[i];
            // FIN_TEST

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
    int col, iTotalAttributes, iTotalRegistersSuspects, iTotalRegistersDataset;
    float fProbabilidad=0.0;

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
