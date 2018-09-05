#ifndef LEDBAR
#define LEDBAR

#include "baseNode.h"
#include "ledGadget.h"

class ledBarNode : public baseNode
{
public:
  ledBarNode(storage* s) : baseNode(s),
  _leds(&_ledController)
  {
    _leds.setEffect("sparks");
  }

  virtual void updateNode()
  {

  }

  virtual void ledLoop()
  {
    _leds.update();
    baseNode::ledLoop();
  }
private:
  ledBar _leds;
protected:

};

#endif
