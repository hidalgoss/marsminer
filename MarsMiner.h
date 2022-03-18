//-----------------------------------------------------------------------------------
// Declaracion de la clase clsMarsMiner.
// File: MarsMiner.h
//-----------------------------------------------------------------------------------
#ifndef MARSMINER_H
#define MARSMINER_H

//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <iomanip>			// Para el setprecision()
#include <vector>			// Para uso de resize()
#include "Attribute.h"
#include "Cluster.h"        // Mi clase Cluster

// valor con el que marcamos los campos a pre-tratar.
#define FAKE_VALUE  -100.0
//#define PRE_PROCESO false


using namespace std;		// std::cout por tanto, agrego este namespace.

//+-------------------------------------------------------------------------------------
// Clase MarsMiner.
//+-------------------------------------------------------------------------------------
class clsMarsMiner{   
private:
    clsAttribute *Attributes;
    float		**fDataSet;
    int			iDataSetTotalRegisters;
    bool        PRE_PROCESO;
    // NUEVO
    char        acClass[2][200]; // Guardo la representación del 0 y del 1 para la class.

public:
	// Constructor xdefecto.
	clsMarsMiner(void);    
	// Constructor de copia.
    clsMarsMiner(const clsMarsMiner & m);
    // destructor
    ~clsMarsMiner();

	// SetThings	
    void		SetDataSetToNull(void);
	void		SetDataSetTotalRegisters(int iTotal);
    void        SetPreProceso(bool bPreprocess);
    int         SetClass(int iType, char *acClass);

	// GetThings
    float		**GetDataSet(void);
	int			GetDataSetTotalRegisters(void);
    clsAttribute *GetAttributes(void);
    bool        GetPreProceso(void);
    char        *GetClass(int iType);

    // Sobrecarga de operadores
    clsMarsMiner &operator= (const clsMarsMiner & m);

	// Funciones publicas
	int			SubString(char *so, char *sd, char cId, int n);
	int			fSubString(FILE *so, char *sd, char cId, int n);
	bool		BuildDataset(char *cFile);
	//void		StringToNum(char *apCadNum);
	
	// Funciones para el DataSet
	bool		DataSetAddRow(float **fpRow);
	float		*DataSetGetRow(int iRow);
	float		*DataSetGetNewRow(void);
    float		DataSetGetAttributeMax(int iCol);
    float		DataSetGetAttributeMin(int iCol);
    float		DataSetAvgAttribute(int iCol);
    void        DataSetInicRowNum(int iRow, float fValue);
    void        DataSetInicRow(float *fRow, float fValue);
    void        DataSetSetRowValue(int iRow, int iAttribute, float fValue);
};

#endif
