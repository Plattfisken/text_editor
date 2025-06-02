#ifndef EDITOR_H
#define EDITOR_H

typedef enum {
    NORMAL,
    INSERT
} MODE;

typedef struct {
    char *file_name;
    char *buffer;
    size_t *line_indices;
    size_t cursor_pos;
    size_t scrolled_lines;
    MODE mode;
    bool editing;
} OpenBuffer;

typedef struct {
    int *keys_pressed;
} Input;

#endif
