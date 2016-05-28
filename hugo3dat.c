/* 
 hugo3dat.c 
 Hugo III: Jungle of Doom
 scenery unpacker

 bugs: unpacks more than just the PCX data.

 Copyright (C) 2016 Jeff Zaroyko
*/ 

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

struct toc_entry
{
  int offset;
  int size;
  FILE* archive;
};

int len1 = 484746;
int len2 = 137029;

char* filename(int fileno)
{
  char* str = malloc(14);
  sprintf(str, "FILE%d.pcx", fileno);
  return str;
}

int main(int argc, char *argv[])
{
  FILE* scenery1 = fopen("SCENERY1.DAT", "r");
  FILE* scenery2 = fopen("SCENERY2.DAT", "r");
  if(!scenery1 || !scenery2)
    assert("input files (SCENERY1.DAT, SCENERY2.DAT) could not be opened\n");
  int ret1;
  int ret2;
  struct toc_entry entries[1024];
  int entry_count = 0;
  FILE* archive = scenery1;
  while(1)
  {
    uint32_t offset;
    uint32_t filesize;

    ret1 = fread(&offset, 4, 1, scenery1);
    ret2 = fread(&filesize, 4, 1, scenery1);
    if(ret1 <1 || ret2 <1)
      break;
    if(offset == 0 && filesize == 0)  // empty entry
      continue;
    if(offset == 0 && filesize != 0) // move onto SCENERY2.DAT
      archive = scenery2;
    if(archive == scenery2 && (offset+filesize > len2)) // probably not reading entries anymore.
      break;
    printf("ret1 %u, ret2 %u, offset %u, filesize %u, archive %p\n", ret1, ret2, offset, filesize, archive);
    entries[entry_count].offset = offset;
    entries[entry_count].size = filesize;
    entries[entry_count].archive = archive;
    entry_count++;
  }
  printf("entry_count is %d\n", entry_count);
  for(int i = 0; i < entry_count; i++)
    {
      char data[entries[i].size];
      printf("entries[i].archive %p, entries[i].offset %u\n", entries[i].archive, entries[i].offset); 
      assert(0 == fseek(entries[i].archive, entries[i].offset, SEEK_SET));
      assert(fread(&data, entries[i].size, 1, entries[i].archive));
      FILE *output = fopen(filename(i), "w");
      fwrite(&data, entries[i].size, 1, output);
      fclose(output);
     }
  return 0;    
}
