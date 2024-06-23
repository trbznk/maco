#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "raylib.h"
#include "casc.h"

#define CELL_INPUT_BUFFER_SIZE 1024
#define CELL_OUTPUT_BUFFER_SIZE 1024

const Color COLOR_DARK_GREEN = {18, 40, 36, 255};
const Color COLOR_LIGHT_GREEN = {171, 225, 136, 255};
const Color COLOR_MOONSTONE = {57, 162, 174, 255};
const Color COLOR_POMP_AND_POWER = {129, 110, 148, 255};
const Color COLOR_CHESTNUT = {162, 73, 54, 255};

void init_gui() {
    SetTraceLogLevel(LOG_ERROR);

    String window_title = init_string("casc");
    i32 screen_width = 800;
    i32 screen_height = 600;
    usize cursor_position = 0;

    Allocator gui_allocator = init_allocator();

    String cell_input_buffer = init_string("");

    // dont like this @todo
    String cell_output_buffer = init_string("");

    InitWindow(screen_width, screen_height, window_title.str);

    // Font font = LoadFontEx("./fonts/monaspace-v1.101/fonts/otf/MonaspaceNeon-Regular.otf", 96, 0, 0);
    // Font font = LoadFontEx("./fonts/PkgTTF-Iosevka-30/Iosevka-Regular.ttf", 96, 0, 0);
    Font font = LoadFontEx("./fonts/liberation_mono/LiberationMono-Regular.ttf", 96, 0, 0);
    Vector2 font_size = MeasureTextEx(font, "i", 48, 0);
    Vector2 padding = { font_size.x/2, font_size.y/4 };
    float line_height = font_size.y + 2*padding.y;

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    SetTargetFPS(60);

    bool last_char_is_caret = false;
    while (!WindowShouldClose()) {
        //
        // Control
        //
        char c = GetCharPressed();

        // handle the caret key ^ seperatly because it has dead key mechanism.
        // So we need to clear the buffer here and call GetCharPressed again,
        // because the first call has not the right return value for us.
        if (last_char_is_caret && c != 0) {
            if (c < 0) {
                c = 'a';
            } else {
                c = GetCharPressed();
            }
            last_char_is_caret = false;
        }
        if (GetKeyPressed() == 161) {
            c = '^';
            last_char_is_caret = true;
        }
        
        if (c != 0) {
            cell_input_buffer = string_insert(&gui_allocator, cell_input_buffer, char_to_string(&gui_allocator, c), cursor_position);
            cursor_position += 1;
        } else if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            if (cursor_position > 0) {
                i32 idx_to_delete = cursor_position-1;
                cell_input_buffer = string_concat(
                    &gui_allocator,
                    string_slice(&gui_allocator, cell_input_buffer, 0, idx_to_delete),
                    string_slice(&gui_allocator, cell_input_buffer, idx_to_delete+1, cell_input_buffer.size)
                );
                cursor_position--;
            }
        } else if (IsKeyPressed(KEY_LEFT)) {
            if (cursor_position > 0) {
                cursor_position -= 1;
            }
        } else if (IsKeyPressed(KEY_RIGHT)) {
            if (cursor_position < cell_input_buffer.size) {
                cursor_position += 1;
            }
        } else if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_ENTER)) {
            Allocator allocator = init_allocator();

            // printf("cell_input_buffer.size=%zu\n", cell_input_buffer.size);
            
            Lexer lexer = {0};
            lexer.source = cell_input_buffer;
            lexer.allocator = &allocator;

            Interp ip;
            ip.allocator = &allocator;

            AST *output = parse(&lexer);
            // print(ast_to_debug_string(&allocator, output));
            output = interp(&ip, output);

            cell_output_buffer = ast_to_string(&gui_allocator, output);

            free_allocator(&allocator);
        }

        //
        // Draw
        //
        BeginDrawing();

        ClearBackground(WHITE);

#if 0
        {
            Color c = { 0xFF, 0x00, 0x00, 0xFF};
            DrawRectangleLines(0, 0, screen_width, screen_height, c); 
            DrawRectangleLines(0, 0, screen_width, line_height, c); 
            DrawRectangleLines(0, line_height, screen_width, line_height, c); 
        }
#endif

        Vector2 input_text_pos = { padding.x, padding.y };
        DrawTextEx(font, cell_input_buffer.str, input_text_pos, font_size.y, 0, COLOR_DARK_GREEN);

        {
            i32 x = padding.x + font_size.x * cursor_position;
            i32 y = padding.y;
            DrawRectangle(x, y, font_size.x/8, font_size.y, COLOR_POMP_AND_POWER);
        }

        DrawLine(0, line_height, screen_width, line_height, COLOR_MOONSTONE);

        if (cell_output_buffer.size > 0) {
            Vector2 output_text_pos = { padding.x, line_height + padding.y };
            DrawTextEx(font, cell_output_buffer.str, output_text_pos, font_size.y, 2, COLOR_DARK_GREEN);
        }

        EndDrawing();
    }

    UnloadFont(font);

    CloseWindow();
}