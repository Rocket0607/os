#include "fs.h"

/*

Each file has an int id. id indicates order, so file w id x is at fs_ptr + (x-1)*sizeof(File) | x > 0.
When creating file, search for next available id by checking memory where id would be and checking if 0
    if fs_ptr * 29*sizeof(File) = 0, then we know 30 is empty and we can fill that in

*/
void init_fs()
{
    const int num_files = 70;
    for (int i = 0; i < num_files; ++i) {
        fs_ptr[i].id = -1;
    }
}

void create_file(char* name, char* data)
{
    // find next available file space
    char mem_id;
    char curr_id = 0;
    mem_id = fs_ptr[0].id;
    while (mem_id != -1) {
        mem_id=fs_ptr[++curr_id].id;
    }
    // copy name
    for (int i = 0; i < 10; ++i) {
        if (name[i] == '\0') {
            fs_ptr[curr_id].name[i] = name[i];
            break;
        }
        fs_ptr[curr_id].name[i] = name[i];
    }
    // copy data
    for (int i = 0; i < 100; ++i) {
        if (data[i] == '\0') {
            fs_ptr[curr_id].data[i] = data[i];
            break;
        }
        fs_ptr[curr_id].data[i] = data[i];
    }
}