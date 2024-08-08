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

    static float slider_val = 0;
    if(is_playing)
    {
        slider_val += 0.1;
    }

  if(!hide_sidepanel)
    {
        // Rect panel_left = cut_left(&layout, 256);
        Rect panel_left = resizable_panel(&layout, 256, &hide_sidepanel);
        ui_core.draw_rect(panel_left, ui_core.theme.panel_color);
        
        static float scroll_y = 0;

        // Rect scrollbar_rect = get_right(&panel_left, 16);


        // panel_left.y += scroll_y*-1;

        // overflow = true;
        begin_scroll_area(&scroll_y, &panel_left);

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
        color_picker_sliders(&panel_left, &ui_core.theme.slider_color, "slider  color");
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
        end_scroll_area(&scroll_y, &panel_left);
    }

    if(!hide_sidepanel2)
    {
        // Rect panel_left = cut_left(&layout, 256);

        Rect panel_left = cut_bottom(&layout, 256);
        // Rect temp = panel_left;

        ui_core.draw_rect(panel_left, ui_core.theme.panel_color);

        static float scroll_y = 0;
        begin_scroll_area(&scroll_y, &panel_left);

        Rect panel_column = cut_top(&panel_left, 32);
        ui_core.label(rectcut(&panel_column, RectCut_Left), "Label 1");

        color_picker_sliders(&panel_left, &ui_core.theme.hot_color, "hot color");

        for(int i=0; i<20; i++)
        {
            panel_column = cut_top(&panel_left, 32);
            ui_core.button(rectcut(&panel_column, RectCut_Left), "test button");
        }

        end_scroll_area(&scroll_y, &panel_left);

    }

    ui_core.draw_string("left over space... example main content...", {layout.x, layout.y}, ui_core.theme.text_color);

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Theme"))
    {
        hide_sidepanel = !hide_sidepanel;
        printf("button 1 %i\n", hide_sidepanel);
    }

    // // render last so its above everything
    // static const char* options[] = {"Open", "Save", "idk"};
    // static int selected_index = 0;
    // static bool combo_open = false;

    // if (ui_core.combo_box(rectcut(&toolbar, RectCut_Left), "Combo Box", options, 3, selected_index, combo_open)) {
    //     printf("Selected option changed to: %s\n", options[selected_index]);
    // }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Edit"))
    {
        printf("button 2\n");
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Panel"))
    {
        hide_sidepanel2 = !hide_sidepanel2;
        printf("panel tog %i\n", hide_sidepanel);
    }

    ui_core.button(rectcut(&bottombar, RectCut_Right), "Example");
    ui_core.slider_rect(bottombar, &slider_val, 0, 32);

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
                        // Scrolled down
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