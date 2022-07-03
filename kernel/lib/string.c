
char *strcpy(char *src, char *dst)
{
        while(*src) {
                *dst = *src;
                dst++;
                src++;
        }
        *dst = '\0';
        return dst;
}
