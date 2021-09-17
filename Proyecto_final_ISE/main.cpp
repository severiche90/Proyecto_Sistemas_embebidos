//=====[Libraries]=============================================================

#include "invernadero_system.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    invernaderoSystemInit();
    
    while (true) {
        invernaderoSystemUpdate();
    }
}