#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#define CHAR_SPACING 0
#define LINE_SPACING 0

size_t get_file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

char *read_file_as_string(char *file_name) {
    FILE *f = fopen(file_name, "r+");
    if(!f) {
        printf("Failed to open file\n");
        return NULL;
    }

    size_t file_size = get_file_size(f);

    char *file_buf = NULL;
    arrsetlen(file_buf, file_size + 1);
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

    file_buf[file_size] = 0;
    return file_buf;
}

int is_new_line(char c) {
    return c == '\n' || c == '\r';
}

size_t *get_line_starts(char *text) {
    size_t *lines = NULL;
    arrpush(lines, 0);
    for(size_t i = 0; i < arrlenu(text); ++i) {
        if(is_new_line(text[i])) {
            arrpush(lines, i + 1);
        }
    }
    return lines;
}

int line_of_idx(size_t *lines, size_t cursor_pos) {
    for(size_t i = arrlenu(lines) - 1; i >= 0; --i) {
        if(lines[i] <= cursor_pos) {
            return i;
        }
    }
    assert(0 && "No\n");
    return -1;
}

int current_offset_from_line_start(size_t *lines, size_t cursor_pos) {
    for(size_t i = arrlenu(lines) - 1; i >= 0; --i) {
        if(lines[i] <= cursor_pos) {
            return cursor_pos - lines[i];
        }
    }
    assert(0 && "No\n");
    return -1;
}

size_t line_length(size_t *lines, char *text, size_t idx) {
    if(idx + 1 >= arrlenu(lines)) {
        assert(idx < arrlenu(lines) && "Index was greater than length of array\n");
        return (arrlenu(text) - 1) - lines[idx];
    }
    return lines[idx + 1] - lines[idx];
}

void save_buffer_to_file(char *buf, char *name) {
    FILE *f = fopen(name, "w");
    int bytes_written = fwrite(buf, arrlen(buf), 1, f);
    if(!bytes_written) {
        printf("Failed to write!\n");
    }
    fclose(f);
}

int main(int argc, char **argv) {

    if(argc <= 1) {
        printf("provide file path\n");
        return 1;
    }

    char *file_buf = read_file_as_string(argv[1]);
    if(!file_buf) {
        printf("Failed to read entire file\n");
        return 1;
    }

    size_t cursor_pos = 0;
    size_t *lines = get_line_starts(file_buf);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(600, 400, "Program");
    Font font = LoadFontEx("../assets/JetBrainsMono-Regular.ttf", 18, NULL, 0);
    // Since it's a monofont we can measure the size of any character and get the width of any text
    Vector2 char_size = MeasureTextEx(font, "A", font.baseSize, CHAR_SPACING);

    if(!IsFontValid(font)) {
        printf("Failed to load font\n");
        return 1;
    }

    int scrolled_line = 0;
    float cursor_update_time = 0.1;
    float last_update = 0;
    SetTextLineSpacing(LINE_SPACING);
    while(!WindowShouldClose()) {
        last_update += GetFrameTime();
        if(IsKeyDown(KEY_LEFT)) {
            if(cursor_pos > 0 && last_update >= cursor_update_time) {
                --cursor_pos;
                last_update = 0;
            }
        }
        if(IsKeyDown(KEY_RIGHT)) {
            if(cursor_pos < arrlenu(file_buf) && last_update >= cursor_update_time){
                ++cursor_pos;
                last_update = 0;
            }
        }
        if(IsKeyDown(KEY_UP)) {
            if(last_update >= cursor_update_time) {
                if(scrolled_line > 0) {
                    --scrolled_line;
                    last_update = 0;
                }
                if(line_of_idx(lines, cursor_pos) > 0) {
                    size_t line_idx_of_prev_line = line_of_idx(lines, cursor_pos) - 1;
                    size_t text_idx_of_prev_line_start = lines[line_idx_of_prev_line];
                    size_t cursor_offset = current_offset_from_line_start(lines, cursor_pos);
                    if(cursor_offset >= line_length(lines, file_buf, line_idx_of_prev_line)) {
                        cursor_pos = text_idx_of_prev_line_start + line_length(lines, file_buf, line_idx_of_prev_line) - 1;
                        last_update = 0;
                    }
                    else {
                        cursor_pos = text_idx_of_prev_line_start + cursor_offset;
                        last_update = 0;
                    }
                }
            }
        }
        if(IsKeyDown(KEY_DOWN)) {
            if(last_update >= cursor_update_time) {
                if(scrolled_line < (int)arrlenu(lines) - 1) {
                    ++scrolled_line;
                    last_update = 0;
                }
                if(line_of_idx(lines, cursor_pos) < (int)arrlenu(lines) - 2) {
                    size_t line_idx_of_next_line = line_of_idx(lines, cursor_pos) + 1;
                    size_t text_idx_of_next_line_start = lines[line_idx_of_next_line];
                    size_t cursor_offset = current_offset_from_line_start(lines, cursor_pos);
                    if(cursor_offset >= line_length(lines, file_buf, line_idx_of_next_line)) {
                        cursor_pos = text_idx_of_next_line_start + line_length(lines, file_buf, line_idx_of_next_line) - 1;
                        last_update = 0;
                    }
                    else {
                        cursor_pos = text_idx_of_next_line_start + cursor_offset;
                        last_update = 0;
                    }
                }
            }
        }
        if(IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
            if(IsKeyPressed(KEY_S)) {
                save_buffer_to_file(file_buf, argv[1]);
            }
        {
            char c = 0;
            while((c = GetCharPressed())) {
                arrins(file_buf, cursor_pos, c);
                ++cursor_pos;
                lines = get_line_starts(file_buf);
            }
        }
        if(IsKeyDown(KEY_BACKSPACE)) {
            if(cursor_pos > 0 && last_update >= cursor_update_time) {
                arrdel(file_buf, cursor_pos - 1);
                --cursor_pos;
                lines = get_line_starts(file_buf);
                last_update = 0;
            }
        }
        if(IsKeyDown(KEY_ENTER)) {
            if(last_update >= cursor_update_time) {
                arrins(file_buf, cursor_pos, '\n');
                ++cursor_pos;
                lines = get_line_starts(file_buf);
                last_update = 0;
            }
        }

        BeginDrawing();
        ClearBackground(WHITE);

        int margin_x = 10, margin_y = 10;
        DrawTextEx(font, &file_buf[lines[scrolled_line]], (Vector2){margin_x, margin_y}, font.baseSize, CHAR_SPACING, BLACK);
        DrawRectangle(
            margin_x + current_offset_from_line_start(lines, cursor_pos) * (char_size.x + CHAR_SPACING),
            margin_y + (line_of_idx(lines, cursor_pos) - scrolled_line) * (char_size.y + LINE_SPACING), char_size.x, char_size.y, (Color){ 0, 0, 0, 100 });

        DrawRectangle(0, GetScreenHeight() - 30, GetScreenWidth(), 30, GRAY);
        char buf[64];
        snprintf(buf, sizeof buf, "line: %d, offset: %d, index: %lu, line length: %lu",
                 line_of_idx(lines, cursor_pos), current_offset_from_line_start(lines, cursor_pos),
                 cursor_pos, line_length(lines, file_buf, line_of_idx(lines, cursor_pos)));
        DrawText(buf, 10, GetScreenHeight() - 25, 18, BLACK);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
