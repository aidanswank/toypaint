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
    
    // Set the draw color for the outline
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &sdl_rect);
    
    // Reset to default color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

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

void draw_ui() {
    SDL_RenderClear(sdl2_renderer);
    ui_core.new_frame();

    SDL_SetRenderDrawColor(sdl2_renderer, 0, 0, 0, 255);

    // Create the main layout rect
    Rect layout = { 0, 0, window_size.x, window_size.y};

    // Cut the top for the toolbar
    Rect toolbar = cut_top(&layout, 32);

    ui_core.draw_rect(toolbar, {100,100,100,100});

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Button 1"))
    {
        printf("button 1\n");
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Left), "Button 2"))
    {
        printf("button 2\n");
    }

    if(ui_core.button(rectcut(&toolbar, RectCut_Right), "Button 3"))
    {
        printf("button 3\n");
    }

    Rect bottombar = cut_bottom(&layout, 32);

    ui_core.button(rectcut(&bottombar, RectCut_Right), "Example");

    static float slider_val = 0;
    ui_core.slider_rect(bottombar, slider_val, 0, 32);

    Rect panel_left = cut_left(&layout, 200);
    ui_core.draw_rect(panel_left,{100,100,100,100});

    Rect panel_column = cut_top(&panel_left, 32);
    if(ui_core.button(rectcut(&panel_column, RectCut_Left), "Sidebar Button 1"))
    {
        printf("ahhhhhhh\n");
    }

    panel_column = cut_top(&panel_left, 32);
    if(ui_core.button(rectcut(&panel_column, RectCut_Left), "Sidebar Button 2"))
    {
        printf("ahhhhhhh\n");
    }

    ui_core.draw_string("left over space...", {layout.x, layout.y}, ui_core.theme.text_color);

    SDL_RenderPresent(sdl2_renderer);
}
// hack so window resize and doesnt stretch canvas
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
    SDL_Window* window = SDL_CreateWindow("Layout Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_size.x, window_size.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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