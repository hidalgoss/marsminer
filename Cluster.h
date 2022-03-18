#ifndef CLUSTER_H
#define CLUSTER_H

#include "Attribute.h"      // Agrego esta lib para MAX_ATTRIBUTES

class Cluster {

private:
    int     iTotalDataset;
    float   *fCentroid;
    int     iTotalAttributes;   // Indica la dimension del array del centroid
    int     *iIndividuo;        // # indice del individuo en el dataset. Es dinámico pq no sabemos el nº.
    int     iTotalIndividuos;   // longitud del array *individuo.
    float   *fDistancia;        // Distancia del individuo. Es dinámica y se crea conjuntamente con el ind.
    float   *fRowToAvg;         // Mantiene la suma de los atts para cada individuo del cluster.
    float   *fMax;              // Max valor del atributo
    float   *fMin;              // Min valor del atributo
    int     iRecalculos;        // Indica el numero de veces que se ha recalculado el centroid de este cluster.


public:
    Cluster();
    Cluster(int iTotalAttributes,
            int iTotalIndividuos,
            clsAttribute *atts);
    Cluster &operator= (const Cluster & m);     // Sobrecarga de operadores
    ~Cluster(void);                             // Destructor

    // Setters
    void    addIndividuoAndDistance(int iIndividuo, float fDistance);
    void    initCentroid(void);                 // Inicializa el centroid de este cluster a random
    void    setCentroid(float *new_centroid);   // Recibe una row de floats. Un valor para cada atributo.
    void    addToAvg(float *fRow);              // Agrega individuo para futura media.
    void    init_fRowToAvg(void);
    int     setMaxMinRange(clsAttribute *atts);
    void    clearCluster(void);
    void    incrRecalculalos(void);

    // Getters    
    float   *getCentroid(void);     // Devuelve el centroid de este cluster
    int     getTotalAttributes(void);
    int     getTotalIndividuos(void);
    // Carga por referencia el indice del individuo y su distancia calculada.
    void    loadIndividuoAndDistance(int iPos,
                                     int *iIndIndex,
                                     float *fIndDistance);
    int     getIndividuo(int iPos);
    float   getDistance(int iPos);
    float   *getMaxRange(void);
    float   *getMinRange(void);
    float   *getAvg(void);          // Devuelve la media calculada del array fRowToAvg.
    int     getRecalculos(void);
};

#endif // CLUSTER_H
