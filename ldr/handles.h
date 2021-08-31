#pragma once

/*
The purpose of this API is to emulate the fork syscall, which will let us drop
the ugly umount hack. It is used to track objects belonging to the Linux
executable (allocated memory, file descriptors, FILE* handles) and free/close
them when an exit is emulated, to prevent memory leaks.
*/

void* resource_alloc(size_t sz, void(*free)(void*));
void* resource_realloc(void* h, size_t sz);
void resource_free(void* h);
void resource_collect(void);
