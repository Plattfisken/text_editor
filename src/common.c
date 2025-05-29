#pragma once

int is_new_line(char c) {
    return c == '\n' || c == '\r';
}

size_t get_file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

char *read_entire_file(char *file_name) {
    FILE *f = fopen(file_name, "r");
    if(!f) {
        printf("Failed to open file\n");
        return NULL;
    }

    size_t file_size = get_file_size(f);

    char *file_buf = NULL;
    arrsetlen(file_buf, file_size);
    size_t objects_read = fread(file_buf, file_size, 1, f);
    if(objects_read * file_size != file_size * 1) {
        printf("%lu\n", ftell(f));
        if(ferror(f)) {
            printf("There was an error\n");
        };
        printf("Failed to read file contents, objects read: %lu, file size: %lu\n", objects_read, file_size);
        return NULL;
    }
    fclose(f);
    return file_buf;
}

void save_string_to_file(char *buf, char *name) {
    FILE *f = fopen(name, "w");
    buf[arrlen(buf) - 1] = '\n';
    int bytes_written = fwrite(buf, arrlen(buf), 1, f);
    if(!bytes_written) {
        printf("Failed to write!\n");
    }
    fclose(f);
}
