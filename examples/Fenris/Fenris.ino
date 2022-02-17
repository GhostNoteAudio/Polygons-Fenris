#include "Fenris.h"

void setup()
{
    Polygons::init();
    Fenris::start();
}

int i = 0;
void loop()
{
    delay(50);
    if (i++ % 100 == 0)
        Serial.println(Timers::GetCpuLoad() * 100);
    Fenris::loop();
}