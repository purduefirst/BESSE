#ifndef _DEBUG_H
#define _DEBUG_H

inline void debugPrint(const char *msg)
{
    #ifdef DEBUG
        Serial.println(msg);
    #else
    #endif
}

#endif