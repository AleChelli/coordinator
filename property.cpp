/**************************************************************************\
* Apio Library                                                         *
* https://github.com/Apio/library-Apio                             *
* Copyright (c) 2012-2014, Apio Inc. All rights reserved.              *
* ------------------------------------------------------------------------ *
*  This program is free software; you can redistribute it and/or modify it *
*  under the terms of the BSD License as described in license.txt.         *
\**************************************************************************/

#include <Arduino.h>
#include "property.h"
#include <avr/eeprom.h>
#include "atmegarfr2.h"
#include "StringBuffer.h"
#include "String.h"

ApioProperty property;

ApioProperty::ApioProperty() {
}

ApioProperty::~ApioProperty() { }

void ApioProperty::Trigger(char *propertyName, int pin, int onValue, int offValue){
  //Serial.println(Apio.property+" "+propertyName);
  if(Apio.property == propertyName){
    if(onValue == Apio.value.toInt()){
      digitalWrite(pin, onValue);
    } else if (offValue == Apio.value.toInt()){
      digitalWrite(pin, offValue);
    }
  }
}

void ApioProperty::Slider(String propertyName, int pin){
  if(Apio.property == propertyName){
    analogWrite(pin, Apio.value.toInt());
  }

}
