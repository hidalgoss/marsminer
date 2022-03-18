//-----------------------------------------------------------------------------------
// Declaracion de la clase clsAttribute.
// File: Attribute.h
//-----------------------------------------------------------------------------------
#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <iostream>
#include <string.h>

#define MAX_ATTRIBUTES			100
#define MAX_ATTRIBUTE_VALUE		1000000	// este valor debe poderse almacenar en un float (7 digitos).
#define MIN_ATTRIBUTE_VALUE		-1		// este valor debe poderse almacenar en un float (7 digitos).

typedef struct sAttribute_{
    char	acName[50];
    char	acType[50];
    float   fMaxValue;
    float   fMinValue;

}sAttribute;                    // Tipo sAttribute PUBLICO.


//+-------------------------------------------------------------------------------------
// Clase clsAttribute
//+-------------------------------------------------------------------------------------
class  clsAttribute{
private:
    sAttribute	*asAttributes;
    int			iAttributesTotal;

public:
    clsAttribute(void);                     // constructor xdefecto
    clsAttribute(int iNumAttributes);       // constructor de array de sAttributes.
    clsAttribute(const clsAttribute & m);   // Constructor de copia.
    ~clsAttribute(void);                    // destructor

    // SetThings
    void		SetAttribute(const char *acName, const char *acType,
                             float fMax, float fMin, int iPos);
    void		SetAttributesTotal(int iTotal);
    void        SetAttributeMaxValue(int iCol, float fMax);
    void        SetAttributeMinValue(int iCol, float fMin);

    // GetThings
    sAttribute	*GetAttribute(int iPos);
    int			GetAttributesTotal(void);
    float       GetAttributeMaxValue(int iCol);
    float       GetAttributeMinValue(int iCol);

    // Sobrecarga de operadores
    clsAttribute &operator= (const clsAttribute &m);
};

#endif // ATTRIBUTE_H
