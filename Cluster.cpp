#include "Cluster.h"
#include <QDebug>
#include <QTime>

// Constructor xdefecto
//-----------------------------------------------------------------------------------
Cluster::Cluster() {
    this->fCentroid = NULL;
    this->iIndividuo = NULL;
    this->fDistancia = NULL;
    this->iTotalAttributes = 0;
    this->iTotalIndividuos = 0;
    this->fRowToAvg = NULL;
    this->fMax = NULL;
    this->fMax = NULL;
    this->iTotalDataset = 0;
    this->iRecalculos = 0;
}


//-----------------------------------------------------------------------------------
// Constructor que dimensiona el array de individuos, distancias y centroid.
// iTotalAttributes no debe contemplar la clase ya que condiciona la función de distanciz
// en el clustering. Al llamar a este constructor, se le deben de pasar iTotalAtttributes - 1
// del objeto MM cargado.
//-----------------------------------------------------------------------------------
Cluster::Cluster(int iTotalAttributes, int iTotalIndividuos, clsAttribute *atts) {

    this->fCentroid         = new float[iTotalAttributes];
    this->iIndividuo        = new int[iTotalIndividuos];
    this->fDistancia        = new float[iTotalIndividuos];
    this->iTotalAttributes  = iTotalAttributes;
    //this->iTotalIndividuos  = iTotalIndividuos;
    this->iTotalDataset     = iTotalIndividuos; // ESta es la que guarda la dimension.
    this->iTotalIndividuos  = 0; // Es 0. Solo se utiliza para dimensionar arrays
    this->fRowToAvg         = new float[iTotalAttributes];
    this->fMax              = new float[iTotalAttributes];
    this->fMin              = new float[iTotalAttributes];
    this->iRecalculos       = 0;
    this->setMaxMinRange(atts);
    // Inic array de medias a 0
    init_fRowToAvg();
}


//-----------------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------------
Cluster::~Cluster(void) {
    if(fCentroid != NULL)
        delete fCentroid;
    if(iIndividuo != NULL)
        delete iIndividuo;
    if(fDistancia != NULL)
        delete fDistancia;
    if(fRowToAvg != NULL)
        delete fRowToAvg;
    if(fMax != NULL)
        delete fMax;
    if(fMin != NULL)
        delete fMin;
    iTotalAttributes = 0;
    iTotalIndividuos = 0;
    iRecalculos = 0;
}



//-----------------------------------------------------------------------------------
// Agrega el indice que ocupa el individuo en el dataset en la posicion
// indicada por iTotalIndividuos y su distancia.
//-----------------------------------------------------------------------------------
void Cluster::addIndividuoAndDistance(int iIndividuo, float fDistance) {
    this->iIndividuo[this->iTotalIndividuos] = iIndividuo;
    this->fDistancia[this->iTotalIndividuos] = fDistance;
    this->iTotalIndividuos++;
}



//-----------------------------------------------------------------------------------
// Inicializo el array de centroid a valores random entre 0 y 1.Se considera
// que se trabaja con valores normalizados y su valor oscila en este rango.
//-----------------------------------------------------------------------------------
/*void Cluster::initCentroid(void) {
    int value;
    if(!this->fCentroid){
        for(int i=0; i<iTotalAttributes; i++){
            value = rand();
            this->fCentroid[i] = ((float)value)/((float)RAND_MAX);
            qDebug() << "random; " << this->fCentroid[i] << endl;
        }
    }
}*/
//-----------------------------------------------------------------------------------
// Pre: Deben estar inicializados los arrays fMax y fMin que indican los
// valores máximo y mínimo de cada uno de los atributos que forman el centroid.
//-----------------------------------------------------------------------------------
void Cluster::initCentroid(void) {
    float value;
    //if(!this->fCentroid){
        for(int i=0; i<iTotalAttributes; i++){
            qDebug() << "RAND_MAX: " << RAND_MAX << Qt::endl;
            value = ((float)rand()/(float)RAND_MAX);
            // Tengo value entre 0 y 1. Hago p.ej.:
            // vaue=0.25,  60-20=40, 40*0,25=10, 20+10=30 ok.
            // Multiplico la diferencia Max-Min por value y la sumo al Min.
            // Esto me da un valor random que siempre oscila entre Min y Max.
            this->fCentroid[i] = ((value * (fMax[i] - fMin[i])) + fMin[i]);
            qDebug() << "random; " << this->fCentroid[i] << Qt::endl;
            qDebug() << "value: " << value << Qt::endl;
            qDebug() << "Max: " << fMax[i] << Qt::endl;
            qDebug() << "Min: " << fMin[i] << Qt::endl;
        }
    //}
}


//-----------------------------------------------------------------------------------
// Pre: la row de floats debe tener el mismo número de atributos que se le han
// asignado a este objeto cluster (iTotalAttributes).
//-----------------------------------------------------------------------------------
void Cluster::setCentroid(float *new_centroid) {
    for(int i=0; i < this->iTotalAttributes; i++){
        this->fCentroid[i] = new_centroid[i];
    }
}



//-----------------------------------------------------------------------------------
// Suma la nueva row de atts normalizada a los ya existentes.
// Recibe la nueva row a sumar y utiliza los valores de rango para los attributes
// de los arrays del cluster fMax y fMin, de manera que se puedan normalizar.
//
// Pre: El rango de valores fMax y fMin que cada atributo puede tomar, debe haber
// sido inicializado en este cluster a través de setMaxMinRange().
//-----------------------------------------------------------------------------------
void Cluster::addToAvg(float *fRow) {
    float fDiferencia = 0;
    for(int i=0; i<this->iTotalAttributes; i++){
        if((fDiferencia = (this->fMax[i] - this->fMin[i])) == 0){
            if(fMax[i] != 0)
                this->fRowToAvg[i] += fRow[i]/fMax[i]; //0;
            // Si fMax[i] es 0 no hay atributos mayores a 0 en esta columna.
        }else
            this->fRowToAvg[i] += fRow[i]/fDiferencia;
    }
}



//-----------------------------------------------------------------------------------
// Inicializa el array de medias a 0.
//-----------------------------------------------------------------------------------
void Cluster::init_fRowToAvg(void) {
    for(int i=0; i<this->iTotalAttributes; i++)
        this->fRowToAvg[i] = 0;
}



//-----------------------------------------------------------------------------------
// El numero de atributos del objeto clsAttribute que recibe debe ser el mismo que el
// numero de atributos que se han definido para este cluster.
//-----------------------------------------------------------------------------------
int Cluster::setMaxMinRange(clsAttribute *atts) {
    if(!this->fMax || !this->fMin)
        return 1;       // Error. arrays fMax y fMin a NULL.
    for(int i=0; i<this->iTotalAttributes; i++) {   //atts->GetAttributesTotal(); i++){
        this->fMax[i] = atts->GetAttributeMaxValue(i);
        this->fMin[i] = atts->GetAttributeMinValue(i);
    }
    return 0;
}



//-----------------------------------------------------------------------------------
// Limpia el cluster una vez recalculado el centroid. Hay que volver a agregar
// a los individuos y sus distancias, así como la media que se va calculando.
//-----------------------------------------------------------------------------------
void Cluster::clearCluster(void) {
    iTotalIndividuos = 0;
    // Inicializo array de medias a 0.
    init_fRowToAvg();
}



//-----------------------------------------------------------------------------------
// OJO Es peligrosa porque no sabremos el porcentaje de ajuste que ha sufrido el cluster.
//-----------------------------------------------------------------------------------
//void Cluster::recalculateCentroid(void) {
//    setCentroid(getAvg());
//}




//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Cluster::incrRecalculalos(void) {
    this->iRecalculos++;
}





//-----------------------------------------------------------------------------------
// Devuelve la row de floats que representa el centroid de este cluster.
// Es la direccion al primeer elemento float de la row.
//-----------------------------------------------------------------------------------
float *Cluster::getCentroid(void) {
    return fCentroid;
}


//-----------------------------------------------------------------------------------
// Devuelve el numero total de atributos que forma el cluster.
//-----------------------------------------------------------------------------------
int Cluster::getTotalAttributes(void) {
    return this->iTotalAttributes;
}


//-----------------------------------------------------------------------------------
// Devuelve el numero de individuos que forma el cluster.
//-----------------------------------------------------------------------------------
int Cluster::getTotalIndividuos(void) {
    return this->iTotalIndividuos;
}


//-----------------------------------------------------------------------------------
// Carga los datos del individuo especificado por iIndex a la posicion iTotalIndividuos,
// indrementandola en uno, dejandola lista para la siguiente inserción.
//-----------------------------------------------------------------------------------
void Cluster::loadIndividuoAndDistance(int iPos, int *iIndIndex, float *fIndDistance) {
    if(!this->iIndividuo[iPos]){
        // Si no es null, cargo el indice y la distancia almacenada del individuo iIndividuo.
        *iIndIndex      = this->iIndividuo[iPos];
        *fIndDistance   = this->fDistancia[iPos];
    }
}



//-----------------------------------------------------------------------------------
// Devuelve el individuo de la posición iPos. Guardana el indice en el DS.
//-----------------------------------------------------------------------------------
int Cluster::getIndividuo(int iPos) {
    return this->iIndividuo[iPos];
}



//-----------------------------------------------------------------------------------
// Devuelve la distancia almacenada del individuo indicado.
//-----------------------------------------------------------------------------------
float Cluster::getDistance(int iPos) {
    return this->fDistancia[iPos];
}



//-----------------------------------------------------------------------------------
// Devuelve el array de floats que indica el rango maximo de sus valores.
//-----------------------------------------------------------------------------------
float *Cluster::getMaxRange(void) {
    return this->fMax;
}


//-----------------------------------------------------------------------------------
// Devuelve el  array de floats que indica el rango mínimo de sus valores.
//-----------------------------------------------------------------------------------
float *Cluster::getMinRange(void) {
    return this->fMin;
}



//-----------------------------------------------------------------------------------
// Devuelve la row fRowToAvg una vez ha dividido cada atributo entre el número de individuos
// que hay en el cluster en el momento de invocar a esta función y ha multiplicado cada
// atributo por la diferencia que se utilizo para normalizarlo, de manera que se obtiene
// la media en la escala real del atributo.
//-----------------------------------------------------------------------------------
float *Cluster::getAvg(void)  {
    float fDiferencia=0;
    for(int i=0; i<this->iTotalAttributes; i++){
        // Recupero valor atributo
        if((fDiferencia = (this->fMax[i] - this->fMin[i])) == 0){
            if(fMax[i] != 0)
                this->fRowToAvg[i] *= fMax[i];  // Al reves de addToAvg()
            // Si fMax[i] es 0 no hay atributos mayores a 0 en esta columna.
        }else{
            this->fRowToAvg[i]*= fDiferencia;   // Al reves de addToAvg()
        }
        // Recuperado valor de atributo. Hago su media.
        this->fRowToAvg[i] /= iTotalIndividuos;
    }
    return this->fRowToAvg;
}



//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
int Cluster::getRecalculos(void) {
    return this->iRecalculos;
}
/*
float *Cluster::getAvg(void)  {
    for(int i=0; i<this->iTotalAttributes; i++){

        this->fRowToAvg[i] /= iTotalIndividuos;
        this->fRowToAvg[i] *= (this->fMax[i] - this->fMin[i]);
    }
    return this->fRowToAvg;
}

int Cluster::getRecalculos(void) {
    return this->iRecalculos;
}*/


//+-------------------------------------------------------------------------------------
// Sobrecarga del operator= para copiar la informacion de un objeto clsMarsMiner a otro.
//+-------------------------------------------------------------------------------------
Cluster &Cluster::operator= (const Cluster& m){
    //int col;
    // Elimino objetos anteriores, si es que hay.
    if(fCentroid != NULL)
        delete fCentroid;
    if(iIndividuo != NULL)
        delete iIndividuo;
    if(fDistancia != NULL)
        delete fDistancia;
    if(fRowToAvg != NULL)
        delete fRowToAvg;
    if(fMax != NULL)
        delete fMax;
    if(fMin != NULL)
        delete fMin;

    // pido nuevo array de objetos Cluster.
    this->fCentroid         = new float[m.iTotalAttributes];
    this->iIndividuo        = new int[m.iTotalIndividuos];
    this->fDistancia        = new float[m.iTotalIndividuos];
    this->iTotalAttributes  = m.iTotalAttributes;
    this->iTotalIndividuos  = m.iTotalIndividuos;
    this->fRowToAvg         = new float[m.iTotalAttributes];
    this->fMax              = new float[m.iTotalAttributes];
    this->fMin              = new float[m.iTotalAttributes];

    // Copio el contenido del recibido en este cluster.
    /*for(col=0; col<m.iTotalAttributes; col++){
        this->fCentroid[col]    = m.getCentroid()[col];
        // PENDIENTE
    }

    for (fil = 0; fil < m.iDataSetTotalRegisters; fil++)
        for(int col = 0; col < Attributes->GetAttributesTotal(); col++)
            fDataSet[fil][col] = m.fDataSet[fil][col];

    iDataSetTotalRegisters = m.iDataSetTotalRegisters;
    PRE_PROCESO = m.PRE_PROCESO;
    */
    return *this;
}
