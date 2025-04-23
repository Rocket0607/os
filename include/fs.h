#ifndef FS_H
#define FS_H

struct File {
    char id;
    char name[10];
    char data[100];
};

extern struct File *fs_ptr;

void init_fs();

void create_file(char *name, char *data);

#endif