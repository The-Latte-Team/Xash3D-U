#ifndef CAFE_UTILS_H
#define CAFE_UTILS_H

extern char *sdCard;

void prepend(char* s, const char* t);
int str_cut(char *str, int begin, int len);
char *GetSDCardPath();

#endif // CAFE_UTILS_H
