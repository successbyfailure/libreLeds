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

  }

  virtual void setupNode()  {;}
  virtual void sensorLoop() {;}

  virtual void nodeLoop()   {;}

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
