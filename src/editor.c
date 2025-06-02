#pragma once

#include "editor.h"

#define ESC 27

bool is_white_space(char c) {
    return is_new_line(c) || c == ' ' || c == '\t';
}

bool is_alphabetic(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_number(char c) {
    return (c >= '0' && c <= '9');
}

bool is_alpha_numeric(char c) {
    return is_alphabetic(c) || is_number(c);
}

size_t *get_line_indices(char *text) {
    size_t *lines = NULL;
    arrpush(lines, 0);
    for(size_t i = 0; i < arrlenu(text); ++i) {
        if(is_new_line(text[i])) {
            arrpush(lines, i + 1);
        }
    }
    return lines;
}

size_t get_line_index_from_buffer_index(size_t *line_indices, size_t buffer_index) {
    for(size_t i = arrlenu(line_indices) - 1; i >= 0; --i) {
        if(line_indices[i] <= buffer_index) {
            return i;
        }
    }
    assert(0 && "No\n");
    return -1;
}

uint32_t get_line_start_offset_from_buf_index(size_t *lines, size_t cursor_pos) {
    for(size_t i = arrlenu(lines) - 1; i >= 0; --i) {
        if(lines[i] <= cursor_pos) {
            return cursor_pos - lines[i];
        }
    }
    assert(0 && "No\n");
    return -1;
}

size_t line_length(size_t *lines, char *text, size_t idx) {
    assert(idx < arrlenu(lines) && "Index was greater than length of array\n");
    if(idx + 1 >= arrlenu(lines)) {
        return (arrlenu(text)) - lines[idx];
    }
    return lines[idx + 1] - lines[idx];
}

void cursor_move_left(OpenBuffer *buffer_state) {
    if(buffer_state->cursor_pos > 0) {
        --buffer_state->cursor_pos;
    }
}

void cursor_move_right(OpenBuffer *buffer_state) {
    if(buffer_state->cursor_pos < arrlenu(buffer_state->buffer) - 1){
        ++buffer_state->cursor_pos;
    }
}

void cursor_move_up(OpenBuffer *buffer_state) {
    size_t current_line_index = get_line_index_from_buffer_index(buffer_state->line_indices, buffer_state->cursor_pos);
    if(current_line_index > 0) {
        size_t prev_line_index = current_line_index - 1;
        size_t prev_line = buffer_state->line_indices[prev_line_index];
        size_t cursor_offset = get_line_start_offset_from_buf_index(buffer_state->line_indices, buffer_state->cursor_pos);
        size_t prev_line_length = line_length(buffer_state->line_indices, buffer_state->buffer, prev_line_index);
        if(cursor_offset >= prev_line_length) {
            buffer_state->cursor_pos = prev_line + prev_line_length - 1;
        }
        else {
            buffer_state->cursor_pos = prev_line + cursor_offset;
        }
    }
}

void cursor_move_down(OpenBuffer *buffer_state) {
    size_t current_line_index = get_line_index_from_buffer_index(buffer_state->line_indices, buffer_state->cursor_pos);
    if(current_line_index < arrlenu(buffer_state->line_indices) - 1) {
        size_t next_line_index = current_line_index + 1;
        size_t next_line = buffer_state->line_indices[next_line_index];
        size_t cursor_offset = get_line_start_offset_from_buf_index(buffer_state->line_indices, buffer_state->cursor_pos);
        size_t next_line_length = line_length(buffer_state->line_indices, buffer_state->buffer, next_line_index);
        if(cursor_offset >= next_line_length) {
            buffer_state->cursor_pos = next_line + next_line_length - 1;
        }
        else {
            buffer_state->cursor_pos = next_line + cursor_offset;
        }
    }
}

void cursor_move_left_word(OpenBuffer *buffer_state) {
    // move ahead at least one char
    if(buffer_state->cursor_pos < arrlenu(buffer_state->buffer)) ++buffer_state->cursor_pos;

    while(buffer_state->cursor_pos < arrlenu(buffer_state->buffer) &&
          (is_alpha_numeric(buffer_state->buffer[buffer_state->cursor_pos]) ||
          buffer_state->buffer[buffer_state->cursor_pos] == '_'))
        ++buffer_state->cursor_pos;
    while(buffer_state->cursor_pos < arrlenu(buffer_state->buffer) &&
          is_white_space(buffer_state->buffer[buffer_state->cursor_pos]))
        ++buffer_state->cursor_pos;
}

void insert_char(OpenBuffer *buffer, char c) {
    arrins(buffer->buffer, buffer->cursor_pos, c);
    ++buffer->cursor_pos;
    buffer->line_indices = get_line_indices(buffer->buffer);
}

void delete_char(OpenBuffer *buffer) {
    arrdel(buffer->buffer, buffer->cursor_pos - 1);
    --buffer->cursor_pos;
    buffer->line_indices = get_line_indices(buffer->buffer);
}

void edit_buffer_normal(OpenBuffer *buffer, Input *input) {
    for(int i = 0; i < arrlen(input->keys_pressed); ++i) {
        switch(input->keys_pressed[i]) {
            case 'h': {
                cursor_move_left(buffer);
            } break;
            case 'j': {
                cursor_move_down(buffer);
            } break;
            case 'k': {
                cursor_move_up(buffer);
            } break;
            case 'l': {
                cursor_move_right(buffer);
            } break;
            case 'w': {
                cursor_move_left_word(buffer);
            } break;
            case 'i': {
                buffer->mode = INSERT;
            } break;
            case 's': {
                save_string_to_file(buffer->buffer, buffer->file_name);
            } break;
            case 'q': {
                buffer->editing = false;
            } break;
        }
    }
}

void edit_buffer_insert(OpenBuffer *buffer, Input *input) {
    for(int i = 0; i < arrlen(input->keys_pressed); ++i) {
        switch(input->keys_pressed[i]) {
            case ESC: {
                buffer->mode = NORMAL;
            } break;
            case '\b': {
                if(buffer->cursor_pos > 0) {
                    delete_char(buffer);
                }
            } break;
            case '\r': {
                insert_char(buffer, '\n');
            } break;
            default: {
                insert_char(buffer, input->keys_pressed[i]);
            } break;
        }
    }
}

void edit_buffer(OpenBuffer *buffer, Input *input) {
    switch(buffer->mode) {
        case NORMAL: {
            edit_buffer_normal(buffer, input);
        } break;
        case INSERT: {
            edit_buffer_insert(buffer, input);
        } break;
    }
}

void open_new_buffer(OpenBuffer *buffer, char *file_name) {
    buffer->file_name = file_name;
    buffer->buffer = read_entire_file(buffer->file_name);
    if(!buffer->buffer) {
        arrpush(buffer->buffer, '\n');
    }
    buffer->line_indices = get_line_indices(buffer->buffer);
    buffer->cursor_pos = 0;
    buffer->scrolled_lines = 0;
    buffer->mode = NORMAL;
    buffer->editing = true;
}

