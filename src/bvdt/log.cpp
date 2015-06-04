#include "log.h"
char time_buff[1024];
const char* current_time(){
    time_t t = time(NULL);
    tm * tt= localtime(&t);
    sprintf(time_buff,"%02d:%02d:%02d",tt->tm_hour, tt->tm_min, tt->tm_sec);
    return time_buff;
}
