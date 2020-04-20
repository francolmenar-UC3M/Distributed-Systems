program FILESTORAGE {
  version FILESTORAGEVER {
    int addFile(char* filename) = 1;
    int remove_file(char* filename) = 2;
    int get_file(char* filename, void* buffer) = 3;
  } = 1;
} = 99;
