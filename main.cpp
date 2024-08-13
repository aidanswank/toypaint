#include <stdio.h>
#include "ui.h"

#include "SDL2/SDL.h"
#include "SDL_ttf.h"

Vec2 window_size = {800, 600};
SDL_Renderer* sdl2_renderer;
TTF_Font* font;
// UICore ui_core;

void sdl2_draw_box(Rect rect, Color color, void* userdata) {
    SDL_Renderer* renderer = (SDL_Renderer*)userdata;
    
    // Set the draw color for filling the rectangle
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    
    // Draw the outline
    SDL_RenderDrawRect(renderer, &sdl_rect);
    
    // Reset to default color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void sdl2_draw_rect(const Rect rect, const Color color, void* userdata) {
    SDL_Renderer* renderer = (SDL_Renderer*)userdata;
    
    // Set the draw color for filling the rectangle
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_RenderFillRect(renderer, &sdl_rect);
    
    // // Set the draw color for the outline
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // SDL_RenderDrawRect(renderer, &sdl_rect);
    
    // // Apply padding
    // int padding = 2; // You can adjust this value or make it a parameter
    // SDL_Rect inner_rect = {
    //     rect.x + padding,
    //     rect.y + padding,
    //     rect.w - 2 * padding,
    //     rect.h - 2 * padding
    // };
    
    // // // Draw the inner rectangle (with padding)
    // // SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    // // SDL_RenderFillRect(renderer, &inner_rect);
    
    // // Draw the inner outline
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    // SDL_RenderDrawRect(renderer, &inner_rect);
    
    // Reset to default color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

// void sdl2_draw_rect(const Rect rect, const Color color, void* userdata) {
//     SDL_Renderer* renderer = (SDL_Renderer*)userdata;

//     int border_size = 4;
    
//     // Set the draw color for filling the rectangle
//     SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
//     SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
//     SDL_RenderFillRect(renderer, &sdl_rect);
    
//     Color light_color = {255, 255, 255, 255}; // White
//     Color dark_color = {128, 128, 128, 255};  // Dark gray
//     // Color very_dark_color = {64, 64, 64, 255}; // Very dark gray
    
//     // Draw borders
//     for (int i = 0; i < border_size; i++) {
//         // Light border (top and left)
//         SDL_SetRenderDrawColor(renderer, light_color.r, light_color.g, light_color.b, light_color.a);
//         SDL_RenderDrawLine(renderer, rect.x + i, rect.y + i, rect.x + rect.w - 1 - i, rect.y + i); // Top
//         SDL_RenderDrawLine(renderer, rect.x + i, rect.y + i, rect.x + i, rect.y + rect.h - 1 - i); // Left
        
//         // Dark border (bottom and right)
//         SDL_SetRenderDrawColor(renderer, dark_color.r, dark_color.g, dark_color.b, dark_color.a);
//         SDL_RenderDrawLine(renderer, rect.x + i, rect.y + rect.h - 1 - i, rect.x + rect.w - 1 - i, rect.y + rect.h - 1 - i); // Bottom
//         SDL_RenderDrawLine(renderer, rect.x + rect.w - 1 - i, rect.y + i, rect.x + rect.w - 1 - i, rect.y + rect.h - 1 - i); // Right
//     }
    
//     // Reset to default color
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
// }

void sdl2_draw_string(const char *string, Vec2 pos, Color color,  void* userdata)
{
    SDL_Renderer* renderer = (SDL_Renderer*)userdata;

    SDL_Color text_color = {color.r, color.g, color.b, color.a};  // White color

    SDL_Surface *text_surface = TTF_RenderText_Solid(font, string, text_color);
    if (text_surface == NULL) {
        fprintf(stderr, "TTF_RenderText_Solid error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (text_texture == NULL) {
        fprintf(stderr, "SDL_CreateTextureFromSurface error: %s\n", SDL_GetError());
        SDL_FreeSurface(text_surface);
        return;
    }

// todo remove hardcoded -4
    SDL_Rect dest_rect = {pos.x, pos.y - 4, text_surface->w, text_surface->h};

    SDL_RenderCopy(renderer, text_texture, NULL, &dest_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

int sdl2_get_text_width(const char *string, void* userdata)
{
    int width;
    if (TTF_SizeText(font, string, &width, nullptr) != 0) {
        fprintf(stderr, "TTF_SizeText error: %s\n", TTF_GetError());
        return -1; // Return an error indicator
    }
    return width;
}

void sdl2_render_clip(void* userdata, Rect *rect)
{
    SDL_Renderer* renderer = (SDL_Renderer*)userdata;
    // SDL_Rect sdl_rect = {rect->x, rect->y, rect->w, rect->h};
    SDL_RenderSetClipRect(renderer,(SDL_Rect*)rect);
}

void color_picker_sliders(Rect *layout, Color* color, const char* label)
{
    Rect panel_column = cut_top(layout, 32);

    Rect color_picker_rect = cut_left(&panel_column, 32);
    ui_core.draw_rect(color_picker_rect,*color);
    ui_core.draw_box(color_picker_rect,{255,255,255,255});

    ui_core.label(rectcut(&panel_column, RectCut_Left), label);

    // cut_right(layout, 32);

    const char* labels[] = { "R", "G", "B", "A" };
    unsigned char* color_components[] = { &color->r, &color->g, &color->b, &color->a };

    for (int i = 0; i < 4; ++i) {
        panel_column = cut_top(layout, 32);
        // ui_core.label(rectcut(&panel_column, RectCut_Left), labels[i]);
        ui_core.draw_string(labels[i],{panel_column.x + 13, panel_column.y+6}, {255,255,255,255});

        // ui_core.draw_rect()
        cut_left(&panel_column, 32);
        float slider_val = (float)(*color_components[i]);
        if (ui_core.slider_rect(panel_column, &slider_val, 0, 255)) {
            *color_components[i] = (unsigned char)(slider_val);
        }
    }
}


bool labeled_slider(Rect *parent_rect, const char* label_str, float *value, float min_value, float max_value)
{
    Rect panel_row = cut_top(parent_rect, 32);
    ui_core.label(rectcut(&panel_row, RectCut_Left), label_str);

    return ui_core.slider_rect(panel_row, value, min_value, max_value);
}

#include <stdio.h>
#include <stdlib.h>

#define MAX_CANVAS 100000

struct AppState
{
    // Set the width and height of the canvas
    int canvas_width = 320;
    int canvas_height = 240;
    Color color_pick1 = {0, 0, 0, 255}; // black color default
    float zoom_slider_val = 1;

    Color bitmap[320*240];
};

// Serialization
bool serialize_app_state(const AppState* state, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        return false;
    }
    size_t written = fwrite(state, sizeof(AppState), 1, file);
    fclose(file);
    return (written == 1);
}

// Deserialization
AppState* deserialize_app_state(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }

    AppState* state = static_cast<AppState*>(malloc(sizeof(AppState)));
    if (state == NULL) {
        fclose(file);
        return NULL;  // Memory allocation failed
    }

    size_t read_elements = fread(state, sizeof(AppState), 1, file);
    fclose(file);

    if (read_elements != 1) {
        free(state);
        return NULL;  // Read failed
    }

    return state;
}

static AppState app_state;

#include <vector>
#include <string>

// A vector to hold the history of serialized states
std::vector<std::string> undo_stack;
int undo_index = -1; // Current index in the undo stack

// Serialize the current state and push it onto the stack
void push_undo_state() {
    std::string filename = "undo_state_" + std::to_string(undo_stack.size()) + ".bin";
    if (serialize_app_state(&app_state, filename.c_str())) {
        // Remove states that are ahead of the current undo index
        if (undo_index < static_cast<int>(undo_stack.size()) - 1) {
            undo_stack.erase(undo_stack.begin() + undo_index + 1, undo_stack.end());
        }
        undo_stack.push_back(filename);
        undo_index = undo_stack.size() - 1;
    }
}

// Undo: Load the previous state
void undo() {
    if (undo_index > 0) {
        undo_index--;
        AppState* state = deserialize_app_state(undo_stack[undo_index].c_str());
        if (state) {
            app_state = *state;
            free(state);
        }
    }
}

// Redo: Load the next state
void redo() {
    if (undo_index < static_cast<int>(undo_stack.size()) - 1) {
        undo_index++;
        AppState* state = deserialize_app_state(undo_stack[undo_index].c_str());
        if (state) {
            app_state = *state;
            free(state);
        }
    }
}

void app_init()
{
    for (int i = 0; i < app_state.canvas_width * app_state.canvas_height; ++i) {
        app_state.bitmap[i] = {255, 255, 255, 255}; // White color
    }

    push_undo_state();
}

// Call this after each stroke is finished
void end_stroke() {
    push_undo_state();
    printf("end\n");
}

void draw_line(Color* bitmap, int canvas_width, int canvas_height, int x0, int y0, int x1, int y1, Color color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < canvas_width && y0 >= 0 && y0 < canvas_height) {
            int pixel_index = y0 * canvas_width + x0;
            bitmap[pixel_index] = color;
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_ui() 
{   
    SDL_RenderClear(sdl2_renderer);
    ui_core.new_frame();

    SDL_SetRenderDrawColor(sdl2_renderer, 0, 0, 0, 255);

    // Create the main layout rect
    // Rect layout = { ui_core.theme.padding, ui_core.theme.padding, window_size.x - ui_core.theme.padding * 2, window_size.y - ui_core.theme.padding * 2};
    Rect layout = { 0, 0, window_size.x, window_size.y };

    // ui_core.draw_rect(layout,{0,0,0,255});

    // Cut the top for the toolbar
    static bool hide_toolbar = false;
    static bool is_playing = false;
    static bool hide_sidepanel = false;
    static bool hide_sidepanel2 = false;
    static bool hide_canvas_tools = false;
    static bool hide_canvas_settings = true;

    Rect toolbar = cut_top(&layout, 32);

    ui_core.draw_rect(toolbar, ui_core.theme.panel_color);

    // Rect toolbar = cut_top(&layout, 32);

    // ui_core.draw_rect(toolbar, ui_core.theme.panel_color);

    // static bool hide_sidepanel = false;
    // static bool hide_sidepanel2 = false;


    // static bool is_playing = false;
    // if(ui_core.button(rectcut(&toolbar, RectCut_Right), "Play"))
    // {
    //     is_playing = !is_playing;
    //     printf("button 3\n");
    // }

    // Rect bottombar = cut_bottom(&layout, 32);

    // static float slider_val = 0;
    // if(is_playing)
    // {
    //     slider_val += 0.1;
    // }

    if(!hide_canvas_settings)
    {
        Rect canvas_settings_rect = cut_top(&layout, 128);
        ui_core.draw_box(canvas_settings_rect, {255, 255, 0, 255});
        ui_core.label_rect(canvas_settings_rect, "todo add canvas settings");
    }

    if(!hide_sidepanel)
    {
        // Rect panel_left = cut_left(&layout, 256);
        static int panel_width = 256;
        static int resize_start = 0;
        Rect panel_left = resizable_panel(rectcut(&layout, RectCut_Right), &panel_width, &hide_sidepanel);
        ui_core.draw_rect(panel_left, ui_core.theme.panel_color);
        
        static float scroll_y = 0;

        // Rect scrollbar_rect = get_right(&panel_left, 16);


        // panel_left.y += scroll_y*-1;

        // overflow = true;
        begin_scroll_area(NULL, &scroll_y, &panel_left);

        Rect panel_column = cut_top(&panel_left, 32);
        ui_core.label(rectcut(&panel_column, RectCut_Left), "Label 1");

        static float slider_val2 = 0;
        if(ui_core.slider_rect(panel_column, &slider_val2, 0, 32))
        {
            printf("slider changed %f\n", slider_val2);
        }

        panel_column = cut_top(&panel_left, 32);
        ui_core.label(rectcut(&panel_column, RectCut_Left), "Label 2");

        static float slider_val3 = 0;
        ui_core.slider_rect(panel_column, &slider_val3, 0, 32);

        color_picker_sliders(&panel_left, &ui_core.theme.button_color, "button color");
        color_picker_sliders(&panel_left, &ui_core.theme.panel_color, "panel color");
        color_picker_sliders(&panel_left, &ui_core.theme.slider_handle_color, "slider handle color");
        color_picker_sliders(&panel_left, &ui_core.theme.slider_color, "slider color");
        color_picker_sliders(&panel_left, &ui_core.theme.hot_color, "hot color");

        // // print_rect(panel_left);
        // int scrollmax = 0;
        // if(panel_left.h<0)
        // {
            

        //     scrollmax = panel_left.h * -1;
        //     if (scroll_y > scrollmax)
        //     {
        //         scroll_y = scrollmax;
        //     }
        //     ui_core.vslider_rect(scrollbar_rect, scroll_y, 0, scrollmax, false);
        // } else {
        //     scroll_y = 0;
        // }
        //     // cut_left(&panel_left,16);

        // overflow = false;
        end_scroll_area(NULL, &scroll_y, &panel_left);
    }

    if(!hide_sidepanel2)
    {
        // Rect panel_left = cut_left(&layout, 256);

        Rect panel_rect = cut_bottom(&layout, 116);
        // static int panel_height = 256;
        // static int resize_start = 0;
        // i want to resize from the top of this panel. its panel_rect cuts from the bottom
        // Rect panel_rect = resizable_panel(rectcut(&layout, RectCut_Bottom), &panel_height, &hide_sidepanel2);
        // Rect temp = panel_rect;

        ui_core.draw_rect(panel_rect, ui_core.theme.panel_color);

        Rect frame_settings_rect = cut_top(&panel_rect, 32);
        // ui_core.draw_box(frame_settings_rect, {255,255,0,255});
        ui_core.button(rectcut(&frame_settings_rect, RectCut_Left), "Play");
        ui_core.button(rectcut(&frame_settings_rect, RectCut_Left), "Stop");
        static int num_frames = 1;
        if(ui_core.button(rectcut(&frame_settings_rect, RectCut_Left), "Add Frame"))
        {
            num_frames++;
        }
        if(ui_core.button(rectcut(&frame_settings_rect, RectCut_Left), "Remove Frame"))
        {
            num_frames--;
        }

        static float scroll_y = 0;
        static float scroll_x = 0;
        begin_scroll_area(&scroll_x, NULL, &panel_rect);

        // color_picker_sliders(&panel_rect, &ui_core.theme.hot_color, "hot color");
        // color_picker_sliders(&panel_rect, &ui_core.theme.hot_color, "hot color");

        // color_picker_sliders(&panel_rect, &ui_core.theme.hot_color, "hot color");
        // panel_rect = cut_top(&panel_rect, 32);
        for(int i = 0; i < num_frames; i++)
        {
            Rect square = cut_left(&panel_rect, 100);
            ui_core.button_rect(square);
            // ui_core.draw_box(square, {255,255,0,255});
            ui_core.label_rect(square, "frame %i", i);
            // ui_core.label_rect()
            // ui_core.button(rectcut(&panel_rect, RectCut_Left), "test button");
        }
        

        // ui_core.draw_box(panel_rect,{255,255,0,255});



        // // color_picker_sliders(&panel_rect, &ui_core.theme.hot_color, "hot color");
        // panel_rect = cut_top(&panel_rect, 32);
        // for(int i=0; i<8; i++)
        // {
        //     // panel_column = cut_left(&panel_rect, 32);
        //     ui_core.button(rectcut(&panel_rect, RectCut_Left), "test button");
        //     // ui_core.button_rect(panel_column);
        // }

        // panel_rect = panel_row;

        // cut_top(&panel_rect, 32);
        // ui_core.draw_rect(panel_rect,{255,255,0,255});


        end_scroll_area(&scroll_x, NULL, &panel_rect);

    }

    // static Color color_pick1 = {0, 0, 0, 255}; // black color default
    // static float zoom_slider_val = 1;

    if(!hide_canvas_tools)
    {
        static int canvas_tools_width = 256;
        // static int resize_start = 0;
        Rect panel_left = resizable_panel(rectcut(&layout, RectCut_Left), &canvas_tools_width, &hide_canvas_tools);
        // Rect panel_left = cut_left(&layout, 256);
        ui_core.draw_rect(panel_left, ui_core.theme.panel_color);
        
        static float scroll_y = 0;

        // Rect scrollbar_rect = get_right(&panel_left, 16);


        // panel_left.y += scroll_y*-1;

        // overflow = true;
        begin_scroll_area(NULL, &scroll_y, &panel_left);

        // Rect panel_row = cut_top(&panel_left, 32);
        // ui_core.label(rectcut(&panel_row, RectCut_Left), "zoom");

        if(labeled_slider(&panel_left, "zoom", &app_state.zoom_slider_val, 1, 5))
        {
            printf("slider changed %f\n", app_state.zoom_slider_val);
            app_state.zoom_slider_val = (int)app_state.zoom_slider_val;
        }

        static float brush_size = 1;
        if(labeled_slider(&panel_left, "brush size", &brush_size, 1, 5))
        {
            printf("slider changed %f\n", brush_size);
        }

        color_picker_sliders(&panel_left, &app_state.color_pick1, "Color 1");


        end_scroll_area(NULL, &scroll_y, &panel_left);
    }

// main content area
{
    static float scroll_y = 0;
    static float scroll_x = 0;
    static bool is_drawing = false; // Flag to track if a stroke is in progress
    begin_scroll_area(&scroll_x, &scroll_y, &layout);

    int zoomed_canvas_width = static_cast<int>(app_state.canvas_width * app_state.zoom_slider_val);
    int zoomed_canvas_height = static_cast<int>(app_state.canvas_height * app_state.zoom_slider_val);

    Rect canvas_rect = {
        layout.x,
        layout.y,
        zoomed_canvas_width,
        zoomed_canvas_height
    };

    cut_corner(&layout, zoomed_canvas_width, zoomed_canvas_height);

    static int last_mouse_x = -1, last_mouse_y = -1;
    int relative_mouse_x = (ui_core.mouse_pos.x - canvas_rect.x) / app_state.zoom_slider_val;
    int relative_mouse_y = (ui_core.mouse_pos.y - canvas_rect.y) / app_state.zoom_slider_val;

    if(ui_core.active == 0) // check if any other widget is NOT active
    {
        if (relative_mouse_x >= 0 && relative_mouse_x < app_state.canvas_width &&
            relative_mouse_y >= 0 && relative_mouse_y < app_state.canvas_height) 
        {
            if (ui_core.mouse_down && !is_drawing) {
                // Start of stroke
                is_drawing = true;
                last_mouse_x = relative_mouse_x;
                last_mouse_y = relative_mouse_y;
            } 
            
            if (ui_core.mouse_down && is_drawing) {
                // Stroke in progress
                draw_line(app_state.bitmap, app_state.canvas_width, app_state.canvas_height, 
                          last_mouse_x, last_mouse_y, relative_mouse_x, relative_mouse_y, app_state.color_pick1);
                
                last_mouse_x = relative_mouse_x;
                last_mouse_y = relative_mouse_y;
            }
        }

        if (!ui_core.mouse_down && is_drawing) {
            // End of stroke
            is_drawing = false;
            last_mouse_x = -1;
            last_mouse_y = -1;
            // printf("end\n");  // Print "end" when the stroke stops
            end_stroke();
        }
    }

    // Render the bitmap as a grid of zoomed rectangles
    for (int y = 0; y < app_state.canvas_height; y++) {
        for (int x = 0; x < app_state.canvas_width; x++) {
            int pixel_index = y * app_state.canvas_width + x;
            Rect pixel_rect = {
                canvas_rect.x + static_cast<int>(x * app_state.zoom_slider_val),
                canvas_rect.y + static_cast<int>(y * app_state.zoom_slider_val),
                static_cast<int>(app_state.zoom_slider_val),
                static_cast<int>(app_state.zoom_slider_val)
            };

            ui_core.draw_rect(pixel_rect, app_state.bitmap[pixel_index]);
        }
    }

    end_scroll_area(&scroll_x, &scroll_y, &layout);
}

    if(ui_core.button(rectcut(&toolbar, RectCut_Right), "Theme"))
    {
        hide_sidepanel = !hide_sidepanel;
        printf("button 1 %i\n", hide_sidepanel);
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Tools"))
    {
        hide_canvas_tools = !hide_canvas_tools;
        printf("tools %i\n", hide_canvas_tools);
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Save"))
    {
        // Serialization
        if (!serialize_app_state(&app_state, "app_state.bin")) {
            // Handle error
            printf("app failed to save!!!!\n");
        } else {
            printf("app saved!\n");
        }
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Load"))
    {
        // Deserialization
        AppState* restored_state = deserialize_app_state("app_state.bin");
        if (restored_state != NULL) {
            // Use restored_state
            // app_state = restored_state;
            memcpy(&app_state, restored_state, sizeof(AppState));
            printf("loaded save state\n");
            printf("loaded canvas height %i\n", restored_state->canvas_height);
            printf("loaded canvas width %i\n", restored_state->canvas_width);
            free(restored_state);  // Don't forget to free the allocated memory
        } else {
            // Handle error
            printf("failed to load state\n");
        }
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Undo"))
    {
        undo();
        printf("undo performed\n");
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Redo"))
    {
        redo();
        printf("redo performed\n");
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Right), "Settings"))
    {
        printf("button 2\n");
        hide_canvas_settings = !hide_canvas_settings;
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Frames"))
    {
        hide_sidepanel2 = !hide_sidepanel2;
        printf("panel tog %i\n", hide_sidepanel2);
    }

    // if(ui_core.button(rectcut(&toolbar, RectCut_Right), "Play"))
    // {
    //     is_playing = !is_playing;
    //     printf("button 3\n");
    // }
    // ui_core.button(rectcut(&bottombar, RectCut_Right), "Example");
    // ui_core.slider_rect(bottombar, &slider_val, 0, 32);

    SDL_RenderPresent(sdl2_renderer);
}
// hack so window resize doesnt ugly stretch canvas on mac
int sdl2_event_watch(void *userdata, SDL_Event* event) {
    if (event->type == SDL_WINDOWEVENT) {
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            int x = event->window.data1;
            int y = event->window.data2;

            window_size = {x, y};
        }
        
        if (event->window.event == SDL_WINDOWEVENT_EXPOSED) {
            draw_ui();
        }

    }

    return 1;
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    SDL_Window* window = SDL_CreateWindow("Simple Paint", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_size.x, window_size.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    sdl2_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderSetScale(sdl2_renderer, 2, 2);
    SDL_SetRenderDrawBlendMode(sdl2_renderer, SDL_BLENDMODE_BLEND);

    font = TTF_OpenFont("/Users/aidan/dev/cpp/sdl2gui/Mintsoda.ttf", 16);
    // font = TTF_OpenFont("../DejaVuSans.ttf", 16);
    if (!font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return 1;
    }
    
    SDL_Event event;

    UIRenderer renderer = {
        sdl2_renderer,  // userdata
        sdl2_draw_rect,
        sdl2_draw_box,
        sdl2_draw_string,
        sdl2_get_text_width,
        sdl2_render_clip,
    };

    app_init();

    ui_core.init(renderer);

    bool is_running = true;

    SDL_AddEventWatch(sdl2_event_watch, NULL);
    while (is_running)
    {


        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {

                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0)
                    {
                        // Scrolled up
                        // printf("Scrolled up\n");
                        ui_core.scroll_input += event.wheel.y;

                    }
                    else if (event.wheel.y < 0)
                    {
                        // Scrolled downserialize_app_state
                        // printf("Scrolled down\n");
                        ui_core.scroll_input += event.wheel.y;
                    }
                    // if (event.wheel.x > 0)
                    // {
                    //     // Scrolled right
                    //     // printf("Scrolled right\n");
                    //     // Or update your UI accordingly
                    // }
                    // else if (event.wheel.x < 0)
                    // {
                    //     // Scrolled left
                    //     printf("Scrolled left\n");
                    //     // Or update your UI accordingly
                    // }
                    break;
                case SDL_MOUSEMOTION:
                    // update mouse position
                    ui_core.mouse_pos.x = event.motion.x;
                    ui_core.mouse_pos.y = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    ui_core.mouse_down = 1;
                    break;
                case SDL_MOUSEBUTTONUP:
                    ui_core.mouse_down = 0;
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                        // If escape is pressed, return (and thus, quit)
                        return 0;
                    }
                    break;
                    case SDL_WINDOWEVENT:
                        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                        {
                            int x = event.window.data1;
                            int y = event.window.data2;
                   
                            window_size = {x, y};
                        }
                        break;
                case SDL_QUIT:
                    is_running = false;
                    break;
            }
        }

        draw_ui();
        SDL_Delay(16);
    }

    printf("hey\n");
    return 0;
}