#ifndef __UNIVERSAL_H__
#define __UNIVERSAL_H__

#include "Arduino.h"
#include <lib_aci.h>

class Universal
{
  public:
    Universal(unsigned char req, unsigned char rdy, unsigned char rst);
    virtual ~Universal();

    void setup();
    void loop();

  protected:
};

#endif
