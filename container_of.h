#ifndef __CONTAINER_OF_H
#define __CONTAINER_OF_H

#include <stddef.h>
#define container_of(type, ptr, member) ((type*)((char*)ptr - offsetof(type, member)))

#endif