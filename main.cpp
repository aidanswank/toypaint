#include <stdio.h>
#include "ui.h"

#include "SDL2/SDL.h"
#include "SDL_ttf.h"

Vec2 window_size = {800, 600};
SDL_Renderer* sdl2_renderer;
TTF_Font* font;
UICore ui_core;

void sdl2_draw_rect(const Rect rect, const Color color, void* userdata) {
    SDL_Renderer* renderer = (SDL_Renderer*)userdata;
    
    // Set the draw color for filling the rectangle
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_RenderFillRect(renderer, &sdl_rect);
    
    // // Set the draw color for the outline
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // SDL_RenderDrawRect(renderer, &sdl_rect);
    
    // Apply padding
    int padding = 2; // You can adjust this value or make it a parameter
    SDL_Rect inner_rect = {
        rect.x + padding,
        rect.y + padding,
        rect.w - 2 * padding,
        rect.h - 2 * padding
    };
    
    // // Draw the inner rectangle (with padding)
    // SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    // SDL_RenderFillRect(renderer, &inner_rect);
    
    // Draw the inner outline
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    SDL_RenderDrawRect(renderer, &inner_rect);
    
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

void color_picker_sliders(Rect *layout, Color* color, const char* label)
{
    Rect panel_column = cut_top(layout, 32);

    ui_core.label(rectcut(&panel_column, RectCut_Left), label);

    panel_column = cut_top(layout, 32);
    // Red slider
    ui_core.label(rectcut(&panel_column, RectCut_Left), "R");
    float red_slider_val = (float)(color->r);
    if (ui_core.slider_rect(panel_column, &red_slider_val, 0, 255)) {
        color->r = (unsigned char)(red_slider_val);
    }
    
    // Green slider
    panel_column = cut_top(layout, 32);
    ui_core.label(rectcut(&panel_column, RectCut_Left), "G");
    float green_slider_val = (float)(color->g);  // Changed from color->r to color->g
    if (ui_core.slider_rect(panel_column, &green_slider_val, 0, 255)) {
        color->g = (unsigned char)(green_slider_val);
    }
    
    // Blue slider
    panel_column = cut_top(layout, 32);
    ui_core.label(rectcut(&panel_column, RectCut_Left), "B");
    float blue_slider_val = (float)(color->b);  // Changed from color->r to color->b
    if (ui_core.slider_rect(panel_column, &blue_slider_val, 0, 255)) {
        color->b = (unsigned char)(blue_slider_val);
    }

    // Alpha slider
    panel_column = cut_top(layout, 32);
    ui_core.label(rectcut(&panel_column, RectCut_Left), "A");
    float alpha_slider_val = (float)(color->a);  // Changed from color->r to color->b
    if (ui_core.slider_rect(panel_column, &alpha_slider_val, 0, 255)) {
        color->a = (unsigned char)(alpha_slider_val);
    }
}

void print_rect(Rect rect)
{
    printf("x%i y%i w%i h%i\n", rect.x, rect.y, rect.w, rect.h);
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
    Rect toolbar = cut_top(&layout, 32);

    ui_core.draw_rect(toolbar, ui_core.theme.panel_color);

    static bool hide_sidepanel = false;
    static bool hide_sidepanel2 = false;


    static bool is_playing = false;
    if(ui_core.button(rectcut(&toolbar, RectCut_Right), "Play"))
    {
        is_playing = !is_playing;
        printf("button 3\n");
    }

    Rect bottombar = cut_bottom(&layout, 32);

    ui_core.button(rectcut(&bottombar, RectCut_Right), "Example");

    static float slider_val = 0;
    if(is_playing)
    {
        slider_val += 0.1;
    }
    ui_core.slider_rect(bottombar, &slider_val, 0, 32);

  if(!hide_sidepanel)
    {
        Rect panel_left = cut_left(&layout, 256);
        
        static float scroll_y = 0;

        Rect scrollbar_rect = get_right(&panel_left, 16);


        ui_core.draw_rect(panel_left, ui_core.theme.panel_color);
        panel_left.y += scroll_y*-1;

        overflow = true;

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
        color_picker_sliders(&panel_left, &ui_core.theme.slider_color, "slider handle color");

        // print_rect(panel_left);
        int scrollmax = 0;
        if(panel_left.h<0)
        {
            

            scrollmax = panel_left.h * -1;
            if (scroll_y > scrollmax)
            {
                scroll_y = scrollmax;
            }
            ui_core.vslider_rect(scrollbar_rect, scroll_y, 0, scrollmax, false);
        }
            // cut_left(&panel_left,16);

        overflow = false;
    }

    if(!hide_sidepanel2)
    {
        // Rect panel_left = cut_left(&layout, 256);

        Rect panel_left = cut_bottom(&layout, 256);
        // Rect temp = panel_left;

        ui_core.draw_rect(panel_left, ui_core.theme.panel_color);

        Rect panel_column = cut_top(&panel_left, 32);
        ui_core.label(rectcut(&panel_column, RectCut_Left), "Label 1");

        // // Rect panel_row = panel_left;
        // for(int i = 0; i < 2; i++)
        // {
        //     // panel_row = add_top(&panel_row, 32);
        //     // ui_core.draw_string("test", {panel_row.x, panel_row.y-32}, {255,255,255,255});
        //     color_picker_sliders(&panel_left, &ui_core.theme.button_color, "button color");
        //     // ui_core.button_rect(panel_row);
        //     // ui_core.label(rectcut(&panel_row, RectCut_Left), "Label 1");
        // }

        // rectcut(&panel_left, RectAdd_Top)
        // Rect rect = rectcut_cut(layout, size);
        // ui_core.draw_rect

        panel_column = cut_top(&panel_left, 32);
        ui_core.button(rectcut(&panel_column, RectCut_Left), "test button");


            // panel_column = add_top(&panel_column, 32);
            // ui_core.label(rectcut(&panel_column, RectCut_Top), "test label");

            // printf("%i%i\n",panel_column.y,panel_column.h);
        // for(int i = 0; i < 8; i++)
        // {
        // }
        // get_top(&panel_left, );

        // printf("panel %i\n", panel_left.y);
    }

    ui_core.draw_string("left over space... example main content...", {layout.x, layout.y}, ui_core.theme.text_color);

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Theme"))
    {
        hide_sidepanel = !hide_sidepanel;
        printf("button 1 %i\n", hide_sidepanel);
    }

    // render last so its above everything
    static const char* options[] = {"Open", "Save", "idk"};
    static int selected_index = 0;
    static bool combo_open = false;

    if (ui_core.combo_box(rectcut(&toolbar, RectCut_Left), "Combo Box", options, 3, selected_index, combo_open)) {
        printf("Selected option changed to: %s\n", options[selected_index]);
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Edit"))
    {
        printf("button 2\n");
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Panel"))
    {
        hide_sidepanel2 = !hide_sidepanel2;
        printf("panel tog %i\n", hide_sidepanel);
    }

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
    if (!font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return 1;
    }
    
    SDL_Event event;

    UIRenderer renderer = {
        sdl2_renderer,  // userdata
        sdl2_draw_rect,
        sdl2_draw_string,
        sdl2_get_text_width,
    };

    ui_core.init(renderer);

    bool is_running = true;

    SDL_AddEventWatch(sdl2_event_watch, NULL);
    while (is_running)
    {


        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
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