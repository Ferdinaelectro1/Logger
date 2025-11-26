//
// Created by ferdinand on 24/11/2025.
//

#ifdef __linux__
#include <iostream>
#include <ostream>
#endif

#ifdef  __AVR__
#include <Arduino.h>
#endif

#include "Logger.h"

int main() {
    Logger<256>::log(LogLevel::ERROR ,"Core Dumped ");
    Logger<256>::logF(LogLevel::INFO,"The value of this variable is %d",10);
    Logger<256>::logF(LogLevel::FATAL,"This variable cannot be const . So a != %d",355);
    return 0;
}