#ifndef ERROR_H
#define ERROR_H 1

void _panic()

#define panic(...) _panic(__func__, __LINE__, ##__VA_ARGS__)

#endif