#ifndef LOG_H
#define LOG_H
#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include <string>
#include <iostream>
const char* current_time();
#ifndef LOG
#define LOG std::cout<<current_time()<<" "
#endif
#endif // LOG_H
