#pragma once

#include "editor.h"
#include <SDL3/SDL.h>

#define CHAR_SPACING 0
#define LINE_SPACING 0
#define FONT_SIZE 20

int main(int argc, char **argv) {
    if(argc <= 1) {
        printf("Provide name\n");
        return 1;
    }

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        printf("Failed to initialize sdl\n");
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Text editor", 900, 600, SDL_WINDOW_RESIZABLE);
    if(!window) {
        printf("Failed to create window\n");
        return 1;
    }

    load_ttf("../assets/JetBrainsMono-Regular.ttf");

    // create the rendering context and shit
    // ...

    OpenBuffer buffer = {0};
    open_new_buffer(&buffer, argv[1]);

    while(true) {
        Input input = {0};
        SDL_Event event;
        if(!SDL_WaitEvent(&event)) {
            printf("Failed to wait event\n");
            return 1;
        }
        switch(event.type) {
            case SDL_EVENT_QUIT: {
                return 0;
            } break;
            case SDL_EVENT_KEY_DOWN: {
                arrpush(input.keys_pressed, event.key.key);
                edit_buffer(&buffer, &input);
                printf("%d\n", event.key.key);
                arrpush(buffer.buffer, 0);
                printf("%s", buffer.buffer);
                arrpop(buffer.buffer);
            } break;
            default: {
            } break;
        }
    }


#if 0
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    InitWindow(600, 400, "Program");
    SetTargetFPS(60);
    Font font = LoadFontEx("../assets/JetBrainsMono-Regular.ttf", FONT_SIZE * 2, NULL, 0);
    // Since it's a monofont we can measure the size of any character and get the width of any text
    Vector2 char_size = Vector2Scale(MeasureTextEx(font, "A", font.baseSize, CHAR_SPACING), 0.5);

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
                printf("%d\n", c);
                fflush(stdout);
                arrpush(input.chars_pressed, c);
            }
        }
        {
            int k;
            while((k = GetKeyPressed())) {
                printf("%d\n", k);
                fflush(stdout);
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
        int current_line = get_line_index_from_buffer_index(buffer.line_indices, buffer.cursor_pos);
        int cursor_x = margin_x + get_line_start_offset_from_buf_index(buffer.line_indices, buffer.cursor_pos) * (char_size.x + CHAR_SPACING);

        while((current_line - (int)buffer.scrolled_lines) < 0) {
            --buffer.scrolled_lines;
        }

        int cursor_y = margin_y + (current_line - (int)buffer.scrolled_lines) * (char_size.y + LINE_SPACING);
        while(cursor_y > GetScreenHeight()) {
            cursor_y = margin_y + (get_line_index_from_buffer_index(buffer.line_indices, buffer.cursor_pos) - ++buffer.scrolled_lines) *
            (char_size.y + LINE_SPACING);
        }
        // add null terminator
        arrpush(buffer.buffer, 0);
        DrawTextEx(font, &buffer.buffer[buffer.line_indices[buffer.scrolled_lines]], (Vector2){margin_x, margin_y}, FONT_SIZE, CHAR_SPACING, BLACK);
        // remove it
        arrpop(buffer.buffer);

        DrawRectangle(cursor_x, cursor_y, char_size.x, char_size.y, (Color){ 0, 0, 0, 100 });

        DrawRectangle(0, GetScreenHeight() - 30, GetScreenWidth(), 30, GRAY);
        char buf[64];
        snprintf(buf, sizeof buf, "line: %zu, offset: %d, index: %lu, line length: %lu",
                 get_line_index_from_buffer_index(buffer.line_indices, buffer.cursor_pos),
                 get_line_start_offset_from_buf_index(buffer.line_indices, buffer.cursor_pos),
                 buffer.cursor_pos,
                 line_length(buffer.line_indices, buffer.buffer,
                             get_line_index_from_buffer_index(buffer.line_indices, buffer.cursor_pos)));
        DrawText(buf, 10, GetScreenHeight() - 25, FONT_SIZE, BLACK);

        EndDrawing();
    }
    CloseWindow();
#endif
    return 0;
}
