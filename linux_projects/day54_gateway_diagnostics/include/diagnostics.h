#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
#include<stddef.h>

void diagnostics_init(void);
void diagnostics_build_response(char *buffer,size_t buffer_size);

#endif