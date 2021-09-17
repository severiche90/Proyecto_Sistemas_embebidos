#include "mbed.h"
#include "arm_book_lib.h"
#include "electrovalvula.h"


DigitalOut electrovalvulaPin(D0);

static bool electrovalvulaState = OFF;

bool electrovalvulaStateRead()
{
  return electrovalvulaState; 
}

void electrovalvulaInit()
{
  electrovalvulaPin = OFF;
}

void electrovalvulaStateWrite(bool state)
{
  electrovalvulaPin = state;
  electrovalvulaState = state;
}



