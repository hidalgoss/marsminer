//-----------------------------------------------------------------------------------
// Implementacion de la clase clsMarsMiner.
// File: MarsMiner.cpp
//-----------------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#include "MarsMiner.h"
#include <QDebug>

// Constructor xdefecto
//+-------------------------------------------------------------------------------------
clsMarsMiner::clsMarsMiner(void){
	// Inic atributos de la clase.
	// ...

    // Debo pedir un num minimo de attributes, ya que no se el nunero hasta terminar.
    // Pero hay que recordar que el constructor inicializa el numero de attributes
    // del array construido a este número, por lo que debemos ponerlo a 0 manualmente
    // ya que a tavés del siguiente while se suman los que se van agregando.
    Attributes = new clsAttribute(MAX_ATTRIBUTES);
    Attributes->SetAttributesTotal(0);
	SetDataSetTotalRegisters(0);
	fDataSet = NULL;
    PRE_PROCESO = false;   // xdefecto
}



// Constructor de copia - Recibe obj clsMarsMiner y crea otro con sus mismos datos.
//+-------------------------------------------------------------------------------------
clsMarsMiner::clsMarsMiner(const clsMarsMiner & m){
    int i, j;
	float *fpNewRow = NULL;
    //float *row = NULL;

    i = m.Attributes->GetAttributesTotal();
    Attributes = new clsAttribute[m.Attributes->GetAttributesTotal()];
    //Attributes = m.Attributes;
    // lo lleno con la info del objeto a copiar.
    for(i=0; i < m.Attributes->GetAttributesTotal(); i++){
        Attributes->SetAttribute(m.Attributes->GetAttribute(i)->acName,
                                 m.Attributes->GetAttribute(i)->acType,
                                 m.Attributes->GetAttribute(i)->fMaxValue,
                                 m.Attributes->GetAttribute(i)->fMinValue, i);
    }
    Attributes->SetAttributesTotal(m.Attributes->GetAttributesTotal());

    fDataSet = NULL;
    // OJO: TotalRegisters no arranca inicializado. Lo inicializo ahora.
    SetDataSetTotalRegisters(0);
    for(j = 0; j < m.iDataSetTotalRegisters; j++){
		fpNewRow = DataSetGetNewRow();
        for(i = 0; i < m.Attributes->GetAttributesTotal(); i++){
			fpNewRow[i] = m.fDataSet[j][i];			
		}
		DataSetAddRow(&fpNewRow);
    }
    // DataSetAddRow() se encarga de incrementar el TotalRegisters a cada row,
    // por lo que no hace falta el SetDataSetTotalRegisters().
    //SetDataSetTotalRegisters(m.iDataSetTotalRegisters);
    PRE_PROCESO = m.PRE_PROCESO;
}

//+-------------------------------------------------------------------------------------
// Destructor no esta testado.
//+-------------------------------------------------------------------------------------
clsMarsMiner::~clsMarsMiner(){
    if(fDataSet){
        for(int j = 0; j < GetDataSetTotalRegisters(); j++){
            delete [] fDataSet[j];
        }
        fDataSet = NULL;
    }
    SetDataSetTotalRegisters(0);
}

// SetThings
//+-------------------------------------------------------------------------------------


//+-------------------------------------------------------------------------------------
// Para poder poner el DataSet a NULL hay que liberar todas las posiciones, en caso de
// que no este vacio.
//+-------------------------------------------------------------------------------------
void clsMarsMiner::SetDataSetToNull(void){
	for(int i = 0; i < iDataSetTotalRegisters; i++){
		free(fDataSet[i]);
	}
	if(fDataSet != NULL) 
		free(fDataSet);
	fDataSet = NULL;
}


//+-------------------------------------------------------------------------------------
// Establece el numero total de registros que hay en el DataSet.
//+-------------------------------------------------------------------------------------
void clsMarsMiner::SetDataSetTotalRegisters(int iTotal){
	iDataSetTotalRegisters = iTotal;
}


//+-------------------------------------------------------------------------------------
// Establece el valor del atributo PRE_PROCESO.
//+-------------------------------------------------------------------------------------
void clsMarsMiner::SetPreProceso(bool bPreprocess){
    PRE_PROCESO = bPreprocess;
}


//+-------------------------------------------------------------------------------------
// Establece la cadena de la clase para el tipo indicado (0 o 1).
//+-------------------------------------------------------------------------------------
int clsMarsMiner::SetClass(int iType, char *acClass) {
    if(!acClass)
        return 0;       // Devuelvo error.
    strcpy(this->acClass[iType], acClass);
    return 1;           //
}

// GetThings
//+-------------------------------------------------------------------------------------

//+-------------------------------------------------------------------------------------
// Devuelve un double pointer con el DataSet.
//+-------------------------------------------------------------------------------------
float **clsMarsMiner::GetDataSet(void){
	return fDataSet;
}


//+-------------------------------------------------------------------------------------
// Devuelve el Total de registros en el DataSet.
//+-------------------------------------------------------------------------------------
int clsMarsMiner::GetDataSetTotalRegisters(void){
	return iDataSetTotalRegisters;
}


//+-------------------------------------------------------------------------------------
//
//+-------------------------------------------------------------------------------------
clsAttribute *clsMarsMiner::GetAttributes(void){
    return Attributes;
}


//+-------------------------------------------------------------------------------------
// Obtiene el valor del atributo PRE_PROCESO.
//+-------------------------------------------------------------------------------------
bool clsMarsMiner::GetPreProceso(void){
    return PRE_PROCESO;
}


//+-------------------------------------------------------------------------------------
// Obtiene la clase asociada al tipo especificado (0 o 1).
//+-------------------------------------------------------------------------------------
char *clsMarsMiner::GetClass(int iType) {
    return this->acClass[iType];
}


// Sobrecarga de operadores
//+-------------------------------------------------------------------------------------

//+-------------------------------------------------------------------------------------
// Sobrecarga del operator= para copiar la informacion de un objeto clsMarsMiner a otro.
//+-------------------------------------------------------------------------------------
clsMarsMiner &clsMarsMiner::operator= (const clsMarsMiner& m){
    int fil;
    // Elimino objetos anteriores, si es que hay.
    if(Attributes != NULL)
        delete Attributes;

    if(fDataSet){
        for (fil = 0; fil < m.iDataSetTotalRegisters; fil++){
            if(fDataSet[fil])
                // Elimino memoria de rows.
                delete [] fDataSet[fil];
        }
    }

    // pido nuevo array de objetos clsAttribute.
    Attributes = new clsAttribute[MAX_ATTRIBUTES];
    Attributes = m.Attributes;
    // Solicito memoria para filas.
    fDataSet = new float* [m.iDataSetTotalRegisters];
    for (fil = 0; fil < m.iDataSetTotalRegisters; fil++)
        // Solicito memoria para columnas.
        fDataSet[fil] = new float [Attributes->GetAttributesTotal()];

    for (fil = 0; fil < m.iDataSetTotalRegisters; fil++)
        for(int col = 0; col < Attributes->GetAttributesTotal(); col++)
            fDataSet[fil][col] = m.fDataSet[fil][col];

    iDataSetTotalRegisters = m.iDataSetTotalRegisters;
    PRE_PROCESO = m.PRE_PROCESO;
    return *this;
}






// Funciones/Métodos del DataSet
//+-------------------------------------------------------------------------------------
// Para agregar una nueva row al DataSet, solicitamos una nueva row (solicita memoria)
// con DataSetGetNewRow(). Nos da una Row con tantos floats como iAttibutesTotal haya.
// Rellenamos los float de la row y una vez llenos, la guardamos en el DataSet mediante
// DataSetAddRow().



//+-------------------------------------------------------------------------------------
// Devuelve la row numero iRow del DataSet de rows.
// Devuelve NULL en caso de que esta no exista.
//+-------------------------------------------------------------------------------------
float *clsMarsMiner::DataSetGetRow(int iRow){
	if(fDataSet == NULL)
		return NULL;
	return fDataSet[iRow];
}



//+-------------------------------------------------------------------------------------
// Solicita memoria para tantos elementos 'float' como attributes tenga una row. 
// Devuelve la direccion al primer elemento de la row, si ha conseguido memoria.
// Devuleve NULL, en caso contrario.
//
// Pre: iAttributesTotal debe haberse inicializado correctamente.
//+-------------------------------------------------------------------------------------
float *clsMarsMiner::DataSetGetNewRow(void){		 
    float *p = new float [Attributes->GetAttributesTotal()];
	return p;
}



//+-------------------------------------------------------------------------------------
// Recibe un pointer por referencia, que contiene al que apunta al primer sAttribute 
// de todos los sAttributes de una Row (Recibe una row por referencia).
//
// Pre: Debe haberse pedido la row mediante DataSetGetNewRow().
// Post: Añade la row recibida al final del DataSet incrementando automaticamente el 
//		valor de iDataSetTotalRegisters (Total de rows). 
//
// **fDataSet	=> fDataSet[row] == *fDataSet	=>	fDataSet[row_y][col_x] == **fDataSet
//+-------------------------------------------------------------------------------------
bool clsMarsMiner::DataSetAddRow(float **fpRow){
    //float *fNewRow;
    //int iTotalRegs = GetDataSetTotalRegisters();

//    int a = GetDataSetTotalRegisters();
//    if(fDataSet == NULL){
//        a = GetDataSetTotalRegisters();
//    }
	if((fDataSet = (float **)realloc(fDataSet, (GetDataSetTotalRegisters() + 1) *
		sizeof(float *))) == NULL)
			return false;
	// guardo el la @ del pointer recibido por referencia.
	fDataSet[GetDataSetTotalRegisters()] = *fpRow;
	SetDataSetTotalRegisters(GetDataSetTotalRegisters() + 1);
	return true;
}


//+-------------------------------------------------------------------------------------
//
//+-------------------------------------------------------------------------------------
void clsMarsMiner::DataSetSetRowValue(int iRow, int iAttribute, float fValue){
    fDataSet[iRow][iAttribute] = fValue;
}


//+-------------------------------------------------------------------------------------
// Inicializa la row numero iRow del Dataset al valor indicado.
// Pre: La row indicada debe existir en memoria y Dataset.
//+-------------------------------------------------------------------------------------
void clsMarsMiner::DataSetInicRowNum(int iRow, float fValue){
    for(int col=0; col < GetAttributes()->GetAttributesTotal(); col++)
        DataSetSetRowValue(iRow, col, fValue);
}



//+-------------------------------------------------------------------------------------
// Inicializa la row facilitada al valor indicado.
// Pre: La row facilitada ya debe haber sido creada en memoria  mediante la funcion
//      DataSetGetNewRow().
//+-------------------------------------------------------------------------------------
void clsMarsMiner::DataSetInicRow(float *fRow, float fValue){
    for(int col=0; col < GetAttributes()->GetAttributesTotal(); col++)
        fRow[col] = fValue;
}



//+-------------------------------------------------------------------------------------
// Devuelve el Maximo valor del attibute de la posicion iPos en todo el DataSet.
// Devuelve -1 en caso de que se encuentre vacio el DataSet.
//
// Pre: Los attributes deben ser positivos para que funcione bien.
//+-------------------------------------------------------------------------------------
float clsMarsMiner::DataSetGetAttributeMax(int iPos){
    float iMax = MIN_ATTRIBUTE_VALUE;
    for(int i=0; i < GetDataSetTotalRegisters(); i++){
        if(fDataSet[i][iPos] > iMax)
            iMax = fDataSet[i][iPos];
    }
    if(iMax == MIN_ATTRIBUTE_VALUE) return -1;
    else return iMax;
}



//+-------------------------------------------------------------------------------------
// Devuelve el Minimo valor del attibute de la posicion iPos en todo el DataSet.
// Devuelve -1 en caso de que se encuentre vacio el DataSet.
//
// Pre: Los attributes deben ser positivos para que funcione bien.
//+-------------------------------------------------------------------------------------
float clsMarsMiner::DataSetGetAttributeMin(int iPos){
    float iMin = MAX_ATTRIBUTE_VALUE;
    for(int i=0; i < GetDataSetTotalRegisters(); i++){
        if(fDataSet[i][iPos] < iMin)
            iMin = fDataSet[i][iPos];
    }
    if(iMin == MAX_ATTRIBUTE_VALUE) return -1;
    else return iMin;
}



//+-------------------------------------------------------------------------------------
// Devuelve la media de todo el dataset para una columna/attribute dada/o.
// No contabiliza los registros a FAKE_VALUE en la media. Solo la hace de los correctos.
// Si el dataset esta vacio, devuelve 0.
//+-------------------------------------------------------------------------------------
float clsMarsMiner::DataSetAvgAttribute(int iCol){
    int fil, iFakes;
    float fAverage=0.0;

    for(fil=0, iFakes=0; fil < GetDataSetTotalRegisters(); fil++){
        if(fDataSet[fil][iCol] != FAKE_VALUE)
            fAverage += fDataSet[fil][iCol];
        else
            iFakes++;
    }
    if(GetDataSetTotalRegisters() == 0 || (GetDataSetTotalRegisters() - iFakes) == 0)
        return 0;
    else
        return (fAverage/(GetDataSetTotalRegisters() - iFakes));
}



//+-------------------------------------------------------------------------------------
//	f(x) PUBLICA int SubString(char *so, char *sd, char i, int n);
//	Precondicion: tanto el puntero de cadena origen como el destio
//	deben apuntar a una direccion valida de memoria.
//
//	Se le pasa una cadena so de la que extraera y cargara en sd la subcadena
//	que encuentre despues del identificador i, encontrado por n vez.
//	Si le pasamos s="22/03/1999-9-21", i='-', n=2 devolvera la subcadena
//	"21" ya que es la encontrada tras el segundo identificador.
//	Si n=0, devuleve 22/03/1999" que es la subcadena que hay tras el
//	identificador n=0.
//
//	Devuelve cierto si hay datos validos en cadena destino.
//+-------------------------------------------------------------------------------------
int clsMarsMiner::SubString(char *so, char *sd, char cId, int n){
	int iEncontrados;
    char *pini;
	
	for(pini=sd, iEncontrados=0; *so; so++){
		if(*so!=cId) *(sd++)=*so;
		else{
			iEncontrados++;			
			if(n<iEncontrados){		
				*sd='\0';			
				return 1;			
			}else sd=pini;			
		}
	}//Fin for
	if(n==iEncontrados && !*so) *sd='\0';	
	else return 0;
	return 1;
}//Fin SubString()



//+-------------------------------------------------------------------------
//	EliminaEsp
//	Precondicion:	La cadena destino que recibe debe poder albergar la
//	longitud de la cadena resultante. Como maximo sera de longitud igual
//	a la de cadena origen,
//
//	Elimina todos los espacios que encuentra de la cadena origen dejando
//	la cadena resultante en la cadena destino que recibe como parametro.
//	Devuelve puntero a cadena destino por si se quiere utilizar.
//+-------------------------------------------------------------------------
char *EliminaEsp(char *sO, char *sD){
    for(;*sO;sO++){
        if(*sO!=' '){*sD=*sO; sD++;}
    }
    *sD='\0';
    return sD;
}//Fin EliminaEsp()



//+-------------------------------------------------------------------------------------
//	f(x) PUBLICA int SubString(FILE *so, char *sd, char i, int n);
//	Precondicion: tanto el puntero de origen como el destino
//	deben apuntar a una direccion valida de FILE stream, como de memoria.
//
//	Se le pasa un FILE stream *so del que extraera y cargara en sd la subcadena
//	que encuentre despues del identificador i, encontrado por n vez.
//	Si le pasamos s="22/03/1999-9-21", i='-', n=2 devolvera la subcadena
//	"21" ya que es la encontrada tras el segundo identificador.
//	Si n=0, devuleve 22/03/1999" que es la subcadena que hay tras el
//	identificador n=0.
//
//	Devuelve cierto si hay datos validos en cadena destino.
//+-------------------------------------------------------------------------------------
int clsMarsMiner::fSubString(FILE *so, char *sd, char cId, int n){
	int iEncontrados = 0;
	char c, *pini;
    //bool Encontrado = false;
	
	for(pini=sd, iEncontrados=0, c=fgetc(so); !feof(so); c=fgetc(so)){		
		if(c != cId){
			*(sd++)=c;
		}else{
			iEncontrados++;			
			if(n<iEncontrados){		
				*sd='\0';			
				return 1;			
			}else sd=pini;		
		}		
	}//Fin for
	if(n==iEncontrados && feof(so)) *sd='\0';	
	else return 0;
	return 1;
}//Fin SubString()



//+-------------------------------------------------------------------------------------
// Carga el DataSet de la clase MarsMiner con los atributos y la información que
// contiene el archivo arff indicado como parametro.
// Devuelve true si el DataSet ha sido cargado con exito o false en caso contrario.
//+-------------------------------------------------------------------------------------
bool clsMarsMiner::BuildDataset(char *cFilename){
	FILE *pFile = NULL;
	char acData[200];
    char acAux[100]; //, acPreproceso[100];
    float valor = 0.0, *fpNewRow = NULL; //, fAverage = 0.0
    int iNumAttributes = 0, i = 0, col, fil;
    //int test = 0; // test
    char acZero[200], acUno[200], acCad[200]; // NUEVA SSS
    //bool creado = false;

	if(!(pFile = fopen(cFilename, "r")))
		return false;

    // Debo pedir un num minimo de attributes, ya que no se el nunero hasta terminar.
    // Pero hay que recordar que el constructor inicializa el numero de attributes
    // del array construido a este número, por lo que debemos ponerlo a 0 manualmente
    // ya que a tavés del siguiente while se suman los que se van agregando.
    //Attributes = new clsAttribute(MAX_ATTRIBUTES);
    //Attributes->SetAttributesTotal(0);

	acData[0] = '\0';
	i = -1;
	while(!feof(pFile) && strcmp(acData, "@data")){
		fscanf(pFile, "%s", acData);
		cout << "*" << acData << "*" << endl;
		if(!strcmp(acData, "@attribute")){
			i = 0;			
		}
        if(i == 1){
			// guardo nombre attribute
			strcpy(acAux, acData);			
		}else if(i == 2){
            //if(!strcmp(acData, "{0,"))
            if(acData[0] == '{'){
                //  bin detectado. recojo valores.
                SubString(acData, acCad, ',', 0);
                SubString(acCad, acZero, '{', 1);                
                strcpy(this->acClass[0], acZero);
                qDebug() << "*********" << acZero << "*a**" <<
                         this->acClass[0] << "****" << Qt::endl; // TEST
                strcpy(acData, "bin");
            }
			// guardo nombre y type de attribute en posicion iNumAttributes.
            Attributes->SetAttribute(acAux, acData, 0, 0, iNumAttributes++);
			// incremento el numero de attributes que podrá gestionar.
            Attributes->SetAttributesTotal(Attributes->GetAttributesTotal() + 1);
            //test = Attributes->GetAttributesTotal(); // test
        }else if(i == 3){
            SubString(acData, acUno, '}', 0);            
            strcpy(this->acClass[1], acUno);
            qDebug() << "*********" << acUno  <<  "*b**" <<
                        this->acClass[1] << "****" << Qt::endl; // TEST
            //creado = true;
        }
		i++;		
	}
	// test
	for(i = 0; i < iNumAttributes; i++){
        cout << "*" << Attributes->GetAttribute(i)->acName << "*" << endl;
        cout << "*" << Attributes->GetAttribute(i)->acType << "*" << endl;
	}// fin test


	// Inicio la recoleccion de floats sobre el DataSet.
	if(!strcmp(acData, "@data")){        
        // Importante: salto de linea de arff file lo detecta fscanf => Leo linea.
        fscanf(pFile, "%s", acData);
		while(!feof(pFile)){
            // solicito nueva row para el DataSet.
            fpNewRow = DataSetGetNewRow();
            // cargo la nueva row del DataSet.
            for(col = 0; col < Attributes->GetAttributesTotal(); col++){
                SubString(acData, acAux, ',', col);
                if(PRE_PROCESO == true){
                    // Compruebo los interrogantes.
                    if(!strstr (acAux, "?")){
                        // no se encontró la subcadena "?".
                        fpNewRow[col] = atof(acAux);
                    }else{
                        fpNewRow[col] = FAKE_VALUE;
                    }
                }else{
                    // no compruebo los interrogantes.
                    //fpNewRow[col] = atof(acAux);
                    if(col == Attributes->GetAttributesTotal()-1) {
                        if(!strcmp(this->acClass[0], acAux)) {
                            // class bin 0
                            fpNewRow[col] = 0;
                        }else if(!strcmp(this->acClass[1], acAux)) {
                            // class bin 1
                            fpNewRow[col] = 1;
                        }
                    }else{
                        // no compruebo los interrogantes.
                        fpNewRow[col] = atof(acAux);
                    }
                }
			}
            // guardo row solicitada y cargada en DataSet.
            DataSetAddRow(&fpNewRow);
            // Importante: salto de linea de arff file lo detecta fscanf => Leo linea.
            fscanf(pFile, "%s", acData);
		}
	}// Fin recoleccion de floats. fDataSet Cargado.


    if(PRE_PROCESO == true){
        // Me deshago de los FAKE_VALUE, poniendo la media en su lugar.
        for(col = 0; col < Attributes->GetAttributesTotal(); col++){
            for(fil=0; fil < GetDataSetTotalRegisters(); fil++){
                if(DataSetGetRow(fil)[col] == FAKE_VALUE)
                    DataSetSetRowValue(fil, col, DataSetAvgAttribute(col));
            }
        }
    }

    // Calculo el Max y Min de cada Attribute en DataSet y lo guardo en su sitio.
    for(col = 0; col < Attributes->GetAttributesTotal(); col++){
        Attributes->SetAttributeMaxValue(col, DataSetGetAttributeMax(col));
        Attributes->SetAttributeMinValue(col, DataSetGetAttributeMin(col));
    }

    // test
    for(i = 0; i < GetDataSetTotalRegisters(); i++){
        fpNewRow = DataSetGetRow(i);
        for(int j = 0; j < iNumAttributes; j++){
            valor = fpNewRow[j];
            cout << " *" << valor << "* ";
        }
        cout << endl;
    }// fin test
    cout << " *" << DataSetGetAttributeMax(2) << "* " << endl;
    cout << " *" << DataSetGetAttributeMin(2) << "* " << endl;
	return true;
}





