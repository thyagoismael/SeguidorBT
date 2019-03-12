#ifndef _TRIDENTTD_ANANLOGWRITE32_H__
#define _TRIDENTTD_ANANLOGWRITE32_H__

namespace TridentTD
{
  namespace
  {
    byte _init[] = {0,0};
  }
  
  bool analogWrite(uint8_t pin , uint8_t value )
  {
    if(pin < 1 || pin >16)
      return false;
      
    uint8_t ch  =  pin - 1;
    uint8_t bit =  ch - (( pin > 8)? 8:0);
    
    if(! bitRead((pin <= 8) ? _init[0]:_init[1], bit))
    {
      ledcSetup(ch, 5000, 13);
      ledcAttachPin(pin, ch);
      bitWrite((pin <= 8) ? _init[0] : _init[1], bit, 1);
    }
    ledcWrite( ch, 8191/255 * min(value, 255));
    return true;
  }
}

#endif
