#pragma once

#include "editor.h"

#define CHAR_SPACING 0
#define LINE_SPACING 0

int main(int argc, char **argv) {
    if(argc <= 1) {
        printf("provide file path\n");
        return 1;
    }

    OpenBuffer buffer = {0};
    buffer.file_name = argv[1];
    buffer.buffer = read_entire_file(buffer.file_name);
    if(!buffer.buffer) {
        arrpush(buffer.buffer, '\n');
    }
    buffer.line_indices = get_line_indices(buffer.buffer);
    buffer.cursor_pos = 0;
    buffer.scrolled_lines = 0;
    buffer.mode = NORMAL;
    buffer.editing = true;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(600, 400, "Program");
    Font font = LoadFontEx("../assets/JetBrainsMono-Regular.ttf", 18, NULL, 0);
    // Since it's a monofont we can measure the size of any character and get the width of any text
    Vector2 char_size = MeasureTextEx(font, "A", font.baseSize, CHAR_SPACING);

    if(!IsFontValid(font)) {
        printf("Failed to load font\n");
        return 1;
    }
    SetTextLineSpacing(LINE_SPACING);

    while(buffer.editing) {

        Input input = {0};
        {
            char c;
            while((c = GetCharPressed())) {
                arrpush(input.chars_pressed, c);
            }
        }
        {
            int k;
            while((k = GetKeyPressed())) {
                arrpush(input.keys_pressed, k);
            }
        }

        edit_buffer(&buffer, &input);

        arrfree(input.chars_pressed);
        arrfree(input.keys_pressed);
        input.chars_pressed = NULL;
        input.keys_pressed = NULL;

        BeginDrawing();
        ClearBackground(WHITE);

        int margin_x = 10, margin_y = 10;
        // add null terminator
        arrpush(buffer.buffer, 0);
        DrawTextEx(font, &buffer.buffer[buffer.line_indices[buffer.scrolled_lines]], (Vector2){margin_x, margin_y}, font.baseSize, CHAR_SPACING, BLACK);
        arrpop(buffer.buffer);
        DrawRectangle(
            margin_x + get_line_start_offset_from_buf_index(buffer.line_indices, buffer.cursor_pos) * (char_size.x + CHAR_SPACING),
            margin_y + (get_line_index_from_buffer_index(buffer.line_indices, buffer.cursor_pos) -
            buffer.scrolled_lines) * (char_size.y + LINE_SPACING), char_size.x, char_size.y, (Color){ 0, 0, 0, 100 });

        DrawRectangle(0, GetScreenHeight() - 30, GetScreenWidth(), 30, GRAY);
        char buf[64];
        snprintf(buf, sizeof buf, "line: %zu, offset: %d, index: %lu, line length: %lu",
                 get_line_index_from_buffer_index(buffer.line_indices, buffer.cursor_pos),
                 get_line_start_offset_from_buf_index(buffer.line_indices, buffer.cursor_pos),
                 buffer.cursor_pos,
                 line_length(buffer.line_indices, buffer.buffer,
                             get_line_index_from_buffer_index(buffer.line_indices, buffer.cursor_pos)));
        DrawText(buf, 10, GetScreenHeight() - 25, 18, BLACK);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
