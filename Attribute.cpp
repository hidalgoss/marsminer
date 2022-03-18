//-----------------------------------------------------------------------------------
// Implementacion de la clase clsAttribute.
// File: Attribute.cpp
//-----------------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#include "Attribute.h"



//-----------------------------------------------------------------------------------
// constructor xdefecto
//-----------------------------------------------------------------------------------
clsAttribute::clsAttribute(void){
    // Inic array de structs attributes.
    /*for(int i = 0; i < MAX_ATTRIBUTES; i++){
        asAttributes[i].acName[0] = '\0';
        asAttributes[i].acType[0] = '\0';
    }*/
    this->asAttributes = new sAttribute[MAX_ATTRIBUTES];
    // Inic el resto.
    SetAttributesTotal(0);   
}


//-----------------------------------------------------------------------------------
// constructor de array de sAttributes de long iNumAttributes.
//-----------------------------------------------------------------------------------
clsAttribute::clsAttribute(int iNumAttributes){
    this->asAttributes = new sAttribute[iNumAttributes];
    SetAttributesTotal(iNumAttributes);
}


//-----------------------------------------------------------------------------------
// constructor de copia
//-----------------------------------------------------------------------------------
clsAttribute::clsAttribute(const clsAttribute &m){
    //SetAttributesTotal(m.GetAttributesTotal());
    this->iAttributesTotal = m.iAttributesTotal;
    this->asAttributes = new sAttribute[this->iAttributesTotal];
    for(int i = 0; i < this->GetAttributesTotal(); i++){
        SetAttribute(m.asAttributes[i].acName, m.asAttributes[i].acType,
                     m.asAttributes[i].fMaxValue, m.asAttributes[i].fMinValue, i);
    }
}
/*
clsAttribute::clsAttribute(const clsAttribute *m){
    //SetAttributesTotal(m.GetAttributesTotal());
    this->iAttributesTotal = m->iAttributesTotal;
    this->asAttributes = new sAttribute[this->iAttributesTotal];
    for(int i = 0; i < this->GetAttributesTotal(); i++){
        SetAttribute(m->asAttributes[i].acName, m->asAttributes[i].acType,
                     m->asAttributes[i].fMaxValue, m->asAttributes[i].fMinValue, i);
    }
}*/

//-----------------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------------
clsAttribute::~clsAttribute(void){
    /*if(asAttributes != NULL){
        delete[] asAttributes;
    }*/
    asAttributes = NULL;
    SetAttributesTotal(0);
    //SetAttributeMaxValue(0);
    //SetAttributeMinValue(0);
}




// SetThings
//+-------------------------------------------------------------------------------------


//+-------------------------------------------------------------------------------------
// Establece los valores del attribute iPos del array de de structs asAttributes.
//+-------------------------------------------------------------------------------------
void clsAttribute::SetAttribute(const char *acName, const char *acType,
                                float fMax, float fMin, int iPos){
    if(asAttributes){
        if(asAttributes[iPos].acName && acName){
            strcpy(asAttributes[iPos].acName, acName);
        }
        if(asAttributes[iPos].acType && acType){
            strcpy(asAttributes[iPos].acType, acType);
        }
        asAttributes[iPos].fMaxValue = fMax;
        asAttributes[iPos].fMinValue = fMin;
    }
}


//+-------------------------------------------------------------------------------------
// Establece el numero total de attributes en el array de attributes.
//+-------------------------------------------------------------------------------------
void clsAttribute::SetAttributesTotal(int iTotal){
    this->iAttributesTotal = iTotal;
}

//+-------------------------------------------------------------------------------------
// Establece el valor Máximo del attribute en el array de attributes.
//+-------------------------------------------------------------------------------------
void clsAttribute::SetAttributeMaxValue(int iPos, float fMax){
    asAttributes[iPos].fMaxValue = fMax;
}


//+-------------------------------------------------------------------------------------
// Establece el valor Mínimo del attribute en el array de attributes.
//+-------------------------------------------------------------------------------------
void clsAttribute::SetAttributeMinValue(int iPos, float fMin){
    asAttributes[iPos].fMinValue = fMin;
}





// GetThings
//+-------------------------------------------------------------------------------------

//+-------------------------------------------------------------------------------------
// Devuelve un pointer al attribute de la posicion iPos.
//+-------------------------------------------------------------------------------------
sAttribute *clsAttribute::GetAttribute(int iPos){
    return &this->asAttributes[iPos];
}


//+-------------------------------------------------------------------------------------
// Devuelve el Total de attributes que tiene el DataSet.
//+-------------------------------------------------------------------------------------
int clsAttribute::GetAttributesTotal(void){
    return iAttributesTotal;
}

//+-------------------------------------------------------------------------------------
// Devuelve el valor Máximo del attribute en el array de attributes.
//+-------------------------------------------------------------------------------------
float clsAttribute::GetAttributeMaxValue(int iCol){
    return GetAttribute(iCol)->fMaxValue;
}

//+-------------------------------------------------------------------------------------
// Devuelve el valor Mínimo del attribute en el array de attributes.
//+-------------------------------------------------------------------------------------
float clsAttribute::GetAttributeMinValue(int iCol){
    return GetAttribute(iCol)->fMinValue;
}

//+-------------------------------------------------------------------------------------
// Sobrecarga operator= para suma de objetos clsAttribute.
//+-------------------------------------------------------------------------------------
clsAttribute &clsAttribute::operator =(const clsAttribute &m){
    // libero memoria de los attributes actuales, si es que hay.
    if(asAttributes != NULL)
        delete [] asAttributes;

    // creo nuevo array de sAttribute.
    asAttributes = new sAttribute[m.iAttributesTotal];
    // lo lleno con la info del objeto a copiar.
    for(int i=0; i < m.iAttributesTotal; i++){
        SetAttribute(m.asAttributes[i].acName, m.asAttributes[i].acType,
                     m.asAttributes[i].fMaxValue, m.asAttributes[i].fMinValue, i);
    }
    iAttributesTotal = m.iAttributesTotal;
    return *this;
}
