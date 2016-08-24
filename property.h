/**************************************************************************\
* Apio Library                                                         *
* https://github.com/Apio/library-Apio                             *
* Copyright (c) 2012-2014, Apio Inc. All rights reserved.              *
* ------------------------------------------------------------------------ *
*  This program is free software; you can redistribute it and/or modify it *
*  under the terms of the BSD License as described in license.txt.         *
\**************************************************************************/


#ifndef LIB_APIO_PROPERTY_H_
#define LIB_APIO_PROPERTY_H_

#include "Apio.h"


class ApioProperty : public ApioClass {

  public:
    ApioProperty();
    ~ApioProperty();

    void Trigger(char *propertyName, int pin, int onValue, int offValue);
    void Slider(String propertyName, int pin);


};

extern ApioProperty property;

#endif
