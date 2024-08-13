#include <algorithm>

typedef uint64_t UIId;

struct Vec2 { int x, y; };
struct Rect { 
    int x, y, w, h;
};

void print_rect(Rect rect)
{
    printf("x%i y%i w%i h%i\n", rect.x, rect.y, rect.w, rect.h);
}

struct Color { unsigned char r, g, b, a; };

// https://web.archive.org/web/20210306102303/https://halt.software/dead-simple-layouts/

typedef enum {
    RectCut_Left,
    RectCut_Right,
    RectCut_Top,
    RectCut_Bottom,
    RectAdd_Top,
} RectCutSide;

typedef struct {
    Rect* rect;
    RectCutSide side;
} RectCut;

// Rect cut_left(Rect* rect, int a) {
//     int x = rect->x;
//     int w = std::min(a, rect->w);
//     rect->x += w;
//     rect->w -= w;
//     return {x, rect->y, w, rect->h};
// }

// Rect cut_right(Rect* rect, int a) {
//     // int w = std::min(a, rect->w);
//     int w = a;
//     rect->w -= w;
//     return {rect->x + rect->w, rect->y, w, rect->h};
// }

static bool overflow_vert = false;
static bool overflow_horz = false;

Rect cut_left(Rect* rect, int a) {
    int x = rect->x;
    int w;
    
    if(overflow_horz) {
        w = a;
    } else {
        w = std::min(a, rect->w);
    }
    
    rect->x += w;
    rect->w -= w;
    return {x, rect->y, w, rect->h};
}

// Rect cut_right(Rect* rect, int a) {
//     int w;
    
//     if(overflow_horz) {
//         w = a;
//     } else {
//         w = std::min(a, rect->w);
//     }
    
//     rect->w -= w;
//     return {rect->x + rect->w, rect->y, w, rect->h};
// }

Rect cut_right(Rect* rect, int a) {
    int w;
    
    if(overflow_horz) {
        w = a;
        // Allow width to become negative if content exceeds visible area
        // rect->w -= w;
    } else {
        w = std::min(a, rect->w);
    }
    rect->w -= w;
    
    return {rect->x + rect->w, rect->y, w, rect->h};
}

Rect cut_top(Rect* rect, int a) {
    int y = rect->y;
    int h;
    
    if(overflow_vert)
    {
        h = a;
    } else {
        h = std::min(a, rect->h);
    }

    rect->y += h;
    rect->h -= h;
    return {rect->x, y, rect->w, h};
}

Rect cut_bottom(Rect* rect, int a) {
    int h = std::min(a, rect->h);
    rect->h -= h;
    return {rect->x, rect->y + rect->h, rect->w, h};
}

Rect cut_corner(Rect* rect, int width, int height) {
    int w, h;
    int original_x = rect->x;
    int original_y = rect->y;

    // Handle horizontal cutting
    if (overflow_horz) {
        w = width;
    } else {
        w = std::min(width, rect->w);
    }
    rect->x += w;
    rect->w -= w;

    // Handle vertical cutting
    if (overflow_vert) {
        h = height;
    } else {
        h = std::min(height, rect->h);
    }
    rect->y += h;
    rect->h -= h;

    // Return the cut corner rectangle
    return {original_x, original_y, w, h};
}


// Get functions - same as cut, but keep input rect intact
Rect get_left(const Rect* rect, int a) {
    int w = (a < rect->w) ? a : rect->w;
    return (Rect){ rect->x, rect->y, w, rect->h };
}

Rect get_right(const Rect* rect, int a) {
    int w = (a < rect->w) ? a : rect->w;
    return (Rect){ rect->x + rect->w - w, rect->y, w, rect->h };
}

Rect get_top(const Rect* rect, int a) {
    int h = (a < rect->h) ? a : rect->h;
    return (Rect){ rect->x, rect->y, rect->w, h };
}

Rect get_bottom(const Rect* rect, int a) {
    int h = (a < rect->h) ? a : rect->h;
    return (Rect){ rect->x, rect->y + rect->h - h, rect->w, h };
}

// Add functions - add rectangle outside of the input rectangle
Rect add_left(const Rect* rect, int a) {
    return (Rect){ rect->x - a, rect->y, a, rect->h };
}

Rect add_right(const Rect* rect, int a) {
    return (Rect){ rect->x + rect->w, rect->y, a, rect->h };
}

Rect add_top(Rect* rect, int a) {
    // rect->y += a;
    // rect->h = a;
    return (Rect){ rect->x, rect->y + a, rect->w, a };
}

Rect add_bottom(const Rect* rect, int a) {
    return (Rect){ rect->x, rect->y + rect->h, rect->w, a };
}

RectCut rectcut(Rect* rect, RectCutSide side) {
    return (RectCut) { rect, side };
}

Rect rectcut_cut(RectCut rectcut, int a) {
    switch (rectcut.side) {
        case RectCut_Left:   return cut_left(rectcut.rect, a);
        case RectCut_Right:  return cut_right(rectcut.rect, a);
        case RectCut_Top:    return cut_top(rectcut.rect, a);
        case RectCut_Bottom: return cut_bottom(rectcut.rect, a);
        case RectAdd_Top: return add_top(rectcut.rect, a);
        default: abort();
    }
}

struct UITheme
{
	Color button_color = {111,105,129,0};
    Color hot_color = {255,255,255,100};
    Color flash_color = {255,255,255,180};
    Color text_color = {255,255,255,255};
    Color panel_color = {27,27,28,255};
    Color label_bg_color = {180,24,20,255};
    Color slider_handle_color = {82,65,90,180};
    Color slider_color = {255,255,255,14};
    int padding = 8;
};

/*
    hook your drawing functions like this

    UIRenderer renderer = {
        sdl2_renderer,  // userdata
        sdl2_draw_rect,
        sdl2_draw_string,
        sdl2_get_text_width,
    };

    and then pass that to UICore
*/

// this is graphics API agnostic library you have to implement these functions
struct UIRenderer
{
    void* userdata;
    void (*draw_rect)(const Rect rect, const Color color, void* userdata);
    void (*draw_box)(Rect rect, Color color, void* userdata);
    void (*draw_string)(const char* string, Vec2 pos, Color color, void* userdata);
    int (*get_text_width)(const char* string, void* userdata);
    // int (*get_text_height)(const char* string, void* userdata);
    void (*render_clip)(void* userdata, Rect *rect);
};

/*
much help
https://www.youtube.com/watch?v=V_phHKr0yRY
https://solhsa.com/imgui/
*/

#include <vector>

struct UICore
{
    Vec2 mouse_pos = {0,0};
    bool mouse_down = 0;
    float scroll_delta = 0;
    float scroll_input = 0;
    float target_scroll_delta = 0;
    UIId id_counter = 0;
    UIId hover = 0;
    UIId next_hover = 0; // this is needed so we can detect a wiget above another one
    UIId mouse_down_hover = 0;
    UIId clicked = 0;
    UIId active = 0;

    UIRenderer renderer;
    UITheme theme;

    // SDL_Renderer* renderer;
    // TTF_Font *font;

    void init(UIRenderer renderer)
    {
        this->renderer = renderer;
    }

    int get_text_width(const char *string) 
    {
        return renderer.get_text_width(string, renderer.userdata);
    }

    int get_text_height(const char *string) 
    {
        // int height;
        // if (TTF_SizeText(font, string, nullptr, &height) != 0) {
        //     std::cerr << "TTF_SizeText error: " << TTF_GetError() << std::endl;
        //     return -1; // Return an error indicator
        // }
        // return height;
        return 16;
    }

    void draw_string(const char *string, Vec2 pos, Color color) 
    {
        renderer.draw_string(string, pos, color, renderer.userdata);
    }

    void draw_rect(Rect rect, Color color)
    {
        renderer.draw_rect(rect, color, renderer.userdata);
    }

    void draw_box(Rect rect, Color color)
    {
        renderer.draw_box(rect, color, renderer.userdata);
    }

    UIId next_id()
    {
        UIId id = id_counter;
        id_counter += 1;
        return id;
    }

    // Check whether current mouse position is within a rectangle
    int region_hit(Rect rect)
    {
        if (mouse_pos.x < rect.x ||
            mouse_pos.y < rect.y ||
            mouse_pos.x >= rect.x + rect.w ||
            mouse_pos.y >= rect.y + rect.h)
            return 0;
        return 1;
    };

    void end_frame()
    {

    }

    void new_frame()
    {

        overflow_vert = false;
        // reset counts
        id_counter = 1;
        hover = next_hover;
        next_hover = 0;
        // reset clicked
        clicked = 0;

        if(!mouse_down) // if mouse up
        {
            // this runs one frame behind
            // this is so if the user pulls away the mouse they can exit the click
            if(mouse_down_hover == hover)
            {
                clicked = mouse_down_hover;
            }

            mouse_down_hover = 0;
            active = 0;  // Reset active when mouse is released
        }

        if(mouse_down) // if mouse down
        {
            if(hover != 0) // if something is being hovered
            {
                mouse_down_hover = hover;
            }

            if(active == 0 && hover != 0)
            {
                active = hover;
            }
        }

    }

    bool button_rect(Rect rect)
    {
        int id = next_id();
        draw_rect(rect, theme.button_color);

        // if(active != 0) // 
        // {
        //     return 0;
        // }

        if(region_hit(rect))
        {
            next_hover = id;
        }

        if(hover == id) // make lighter on hover
        {
            draw_rect(rect, theme.hot_color);
        }

        if(mouse_down_hover == id) // flash white on actual click
        {
            draw_rect(rect, theme.flash_color);
        }

        // hover = -1;
        return (clicked == id);
    }

    bool combo_box(RectCut layout, const char* label, const char** options, int option_count, int& selected_index, bool& open)
    {
        Rect rect = rectcut_cut(layout, get_text_width(label) + (theme.padding * 2));
        int id = next_id();

        // Draw the combo box button
        bool button_clicked = button_rect(rect);
        
        // Draw the selected option text
        draw_string(options[selected_index], {rect.x + theme.padding, rect.y + int(rect.h/2) - int(get_text_height(options[selected_index])/2)}, theme.text_color);

        // Draw the dropdown arrow
        // Rect arrow_rect = {rect.x + rect.w - 20, rect.y, 20, rect.h};
        // draw_rect(arrow_rect, theme.button_color);
        // draw_string("V", {arrow_rect.x + 5, arrow_rect.y + int(arrow_rect.h/2) - int(get_text_height("▼")/2)}, theme.text_color);

        // Check if the button was clicked to toggle the combo box open/close state
        if (button_clicked)
        {
            open = !open;
        }

        bool option_clicked = false;

        if (open)
        {
            // Render the options in the dropdown
            for (int i = 0; i < option_count; ++i)
            {
                Rect option_rect = { rect.x, rect.y + rect.h * (i + 1), rect.w, rect.h };

                // Check if an option is hovered or clicked
                bool option_button_clicked = button_rect(option_rect);
                draw_string(options[i], {option_rect.x + theme.padding, option_rect.y + int(option_rect.h/2) - int(get_text_height(options[i])/2)}, theme.text_color);

                if (option_button_clicked)
                {
                    selected_index = i; // Set the selected index
                    open = false; // Close the combo box after selection
                    option_clicked = true;
                    break;
                }
            }
        }

        return option_clicked;
    }

    bool button(RectCut layout, const char* label)
    {
        float size = get_text_width(label) + (theme.padding*2);
        float text_height = get_text_height(label);
        // Rect rect = cut_left(layout, size);
        Rect rect = rectcut_cut(layout, size);
        
        bool clicked = button_rect(rect);
        draw_string(label, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);
        return clicked;
    }

    // void label(RectCut layout, const char* label)
    // {
    //     float size = get_text_width(label) + (theme.padding*2);
    //     float text_height = get_text_height(label);
    //     // Rect rect = cut_left(layout, size);
    //     Rect rect = rectcut_cut(layout, size);

    //     // draw_box(rect, {255,0,255,255});
        
    //     // bool clicked = button_rect(rect);
    //     draw_string(label, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);
    // }

    void label(RectCut layout, const char* format, ...)
    {
        char label[256];
        va_list args;
        va_start(args, format);
        vsprintf(label, format, args);
        va_end(args);

        float size = get_text_width(label) + (theme.padding * 2);
        float text_height = get_text_height(label);
        Rect rect = rectcut_cut(layout, size);

        // draw_box(rect, {255, 0, 255, 255});
        
        // bool clicked = button_rect(rect);
        draw_string(label, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);
    }

    void label_rect(Rect rect, const char* format, ...)
    {
        char label[256];
        va_list args;
        va_start(args, format);
        vsprintf(label, format, args);
        va_end(args);

        float size = get_text_width(label) + (theme.padding * 2);
        float text_height = get_text_height(label);
        // Rect rect = rectcut_cut(layout, size);

        // draw_box(rect, {255, 0, 255, 255});
        
        // bool clicked = button_rect(rect);
        draw_string(label, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);
    }

    bool vslider_rect(Rect rect, float &value, float min_value, float max_value, bool flip = false) {
        int id = next_id();
        draw_rect(rect, theme.slider_color);

        if (region_hit(rect)) {
            next_hover = id;
        }

        if (hover == id) {
            draw_rect(rect, theme.hot_color);

            // Convert float value to string
            char value_str[16];
            snprintf(value_str, sizeof(value_str), "%.2f", value);

            // Calculate text dimensions
            int text_width = get_text_width(value_str);
            int text_height = get_text_height(value_str);

            // Calculate text position to be in the middle of the slider
            Vec2 text_pos = { rect.x + (rect.w - text_width) / 2, rect.y + (rect.h - text_height) / 2 };

            // Draw the slider value
            // draw_string(value_str, text_pos, theme.text_color);
        }

        if (active == id && mouse_down) { // might not need mouse down
            // Update value based on mouse position relative to the slider track
            float mouse_ratio = (float)(mouse_pos.y - rect.y) / (float)rect.h;
            if (flip) {
                mouse_ratio = 1.0f - mouse_ratio; // Invert the ratio if flipped
            }
            value = mouse_ratio * (max_value - min_value) + min_value;
            // Clamp value within the specified range
            value = std::max(min_value, std::min(max_value, value));
            // next_hover = id; // Hold the hover state until release

            draw_rect(rect, theme.flash_color);
        }

        // Calculate handle position based on current value
        int handle_height = 20; // Height of the handle
        float value_ratio = (value - min_value) / (max_value - min_value);
        if (flip) {
            value_ratio = 1.0f - value_ratio; // Invert the ratio if flipped
        }
        int handle_y = rect.y + (int)(value_ratio * (rect.h - handle_height));
        Rect handle_rect = { rect.x, handle_y, rect.w, handle_height }; // Handle rect within slider track

        draw_rect(handle_rect, theme.slider_handle_color); // Draw the slider handle
        // draw_box(rect, {255,255,255,100});

        return (active == id && mouse_down);
    }

    bool slider_rect(Rect rect, float *value, float min_value, float max_value) {
        int id = next_id();
        draw_rect(rect, theme.slider_color);
        
        if (region_hit(rect)) {
            next_hover = id;
        }
        
        if (hover == id) {
            draw_rect(rect, theme.hot_color);
        }
        
        if (active == id && mouse_down) {
            // Update value based on mouse position relative to the slider track
            *value = (float)(mouse_pos.x - rect.x) / (float)rect.w * (max_value - min_value) + min_value;
            // Clamp value within the specified range
            *value = std::max(min_value, std::min(max_value, *value));
            next_hover = id; // Hold the hover state until release
            
            draw_rect(rect, theme.flash_color);
        }
        
        // Calculate handle position based on current value
        int handle_width = 20; // Width of the handle
        int handle_x = rect.x + (int)((*value - min_value) / (max_value - min_value) * (rect.w - handle_width));
        Rect handle_rect = { handle_x, rect.y, handle_width, rect.h }; // Handle rect within slider track
        
        draw_rect(handle_rect, theme.slider_handle_color); // Draw the slider handle
        
        // Convert float value to string
        char value_str[16];
        snprintf(value_str, sizeof(value_str), "%.2f", *value);
        
        // Calculate text dimensions
        int text_width = get_text_width(value_str);
        int text_height = get_text_height(value_str);
        
        // Calculate text position to be in the middle of the slider
        Vec2 text_pos = { rect.x + (rect.w - text_width) / 2, rect.y + (rect.h - text_height) / 2 };
        
        // Draw the slider value
        draw_string(value_str, text_pos, theme.text_color);
        
        // return (clicked == id); // if you want it to return just one event
        return (active == id && mouse_down);
    }
};

static UICore ui_core;

// Rect resizable_panel(Rect* layout, int default_width, bool* hide_sidepanel) {
//     static int panel_width = default_width;
//     static bool is_resizing = false;
//     static int resize_start_x = 0;

//     if (*hide_sidepanel) {
//         return Rect{0, 0, 0, 0}; // Return empty rect if panel is hidden
//     }

//     Rect panel_left = cut_left(layout, panel_width);

//     // resize panel right
//     Rect resize_handle = cut_right(&panel_left, 8); // 4 pixels wide resize handle

//     // Rect resize_handle = cut_top(&panel_left, 8); // 4 pixels wide resize handle

//     int id = ui_core.next_id();

//     ui_core.draw_rect(resize_handle, {100, 100, 100, 255}); 

//     if (ui_core.region_hit(resize_handle)) {
//         ui_core.next_hover = id;
//         ui_core.draw_rect(resize_handle, {100, 100, 100, 255}); 
//         if (!is_resizing) {
//             resize_start_x = ui_core.mouse_pos.x;
//             is_resizing = true;
//             ui_core.draw_rect(resize_handle, {0, 100, 0, 255}); 
//         }
//     }

//     if (ui_core.active == id && is_resizing) {
//         ui_core.next_hover = id;
//         int delta = ui_core.mouse_pos.x - resize_start_x;
//         panel_width = std::max(panel_width + delta, 100); // Minimum width of 100
//         resize_start_x = ui_core.mouse_pos.x;

//         // Adjust layout width
//         layout->x += delta;
//         layout->w -= delta;
//     }

//     if (!ui_core.mouse_down) {
//         is_resizing = false;
//     }

//     // Draw resize handle

//     return panel_left;
// }

// Rect resizable_panel(RectCut layout, int* panel_size, int* resize_start, bool* hide_sidepanel) {
//     // static int panel_size = default_width;
//     static bool is_resizing = false;
//     // static int resize_start = 0;
//     // static int resize_start_y = 0;

//     if (*hide_sidepanel) {
//         return Rect{0, 0, 0, 0}; // Return empty rect if panel is hidden
//     }

//     // Rect panel_left = cut_left(layout, panel_size);
//     Rect panel_left = rectcut_cut(layout, *panel_size);

//     // printf("cuttype %i panel_size %i\n", layout.side, *panel_size);
//     printf("cuttype %i panel_size %i resize_start %i\n", layout.side, *panel_size, *resize_start);

//     // resize panel right
//     // Rect resize_handle = cut_right(&panel_left, 8); // 4 pixels wide resize handle
//     Rect resize_handle = rectcut_cut(layout, 8);

//     // Rect resize_handle = cut_top(&panel_left, 8); // 4 pixels wide resize handle

//     UIId id = ui_core.next_id();

//     ui_core.draw_rect(resize_handle, {150, 100, 100, 255}); 

//     if (ui_core.region_hit(resize_handle)) {
//         ui_core.next_hover = id;
//         ui_core.draw_rect(resize_handle, {100, 100, 100, 255}); 
//         // printf("hey %i\n", is_resizing);
//         if (!is_resizing) {
//             *resize_start = ui_core.mouse_pos.x;
//             is_resizing = true;
//             ui_core.draw_rect(resize_handle, {0, 100, 0, 255}); 
//         }
//     }

//     if (ui_core.active == id && is_resizing) {
//         ui_core.next_hover = id;

//         if(layout.side == RectCut_Left)
//         {
//             int delta = ui_core.mouse_pos.x - *resize_start;
//             *panel_size = std::max(*panel_size + delta, 100); // Minimum width of 100
//             *resize_start = ui_core.mouse_pos.x;
//             // printf("RectCut_Left %i resize start %i\n", layout.side, *resize_start);
//         }

//         if(layout.side == RectCut_Bottom)
//         {
//             int delta = ui_core.mouse_pos.y - *resize_start;
//             *panel_size = std::max(*panel_size - delta, 100); // Minimum width of 100
//             *resize_start = ui_core.mouse_pos.y;
//             // printf("RectCut_Bottom %i resize start %i\n", layout.side, *resize_start);
//         }

//         // // // Adjust layout width
//         // layout.rect->x += delta;
//         // layout.rect->w -= delta;
//     }

//     if (!ui_core.mouse_down) {
//         is_resizing = false;
//     }

//     // Draw resize handle

//     return panel_left;
// }

Rect resizable_panel(RectCut layout, int* panel_size, bool* hide_sidepanel) {
    static bool is_resizing = false;
    static int last_mouse_pos = 0;

    if (*hide_sidepanel) {
        return Rect{0, 0, 0, 0}; // Return empty rect if panel is hidden
    }

    Rect panel_left = rectcut_cut(layout, *panel_size);

    // Print debug info
    // printf("cuttype %i panel_size %i\n", layout.side, *panel_size);

    // Create resize handle
    Rect resize_handle = rectcut_cut(layout, 8);

    UIId id = ui_core.next_id();

    // Draw the resize handle
    ui_core.draw_rect(resize_handle, {100, 100, 100, 255});

    if (ui_core.region_hit(resize_handle)) {
        ui_core.next_hover = id;
        // ui_core.draw_rect(resize_handle, {100, 0, 100, 255});

        if (!is_resizing) {
            last_mouse_pos = ((layout.side == RectCut_Left) || (layout.side == RectCut_Right)) ? ui_core.mouse_pos.x : ui_core.mouse_pos.y;
            is_resizing = true;
            // ui_core.draw_rect(resize_handle, {180, 180, 180, 255});
            ui_core.draw_box(resize_handle, {180, 180, 180, 255});
        }
    }

    if (ui_core.active == id && is_resizing) {
        ui_core.next_hover = id;

        int current_mouse_pos = ((layout.side == RectCut_Left) || (layout.side == RectCut_Right)) ? ui_core.mouse_pos.x : ui_core.mouse_pos.y;
        int delta = current_mouse_pos - last_mouse_pos;

        if (layout.side == RectCut_Left) {
            *panel_size = std::max(*panel_size + delta, 100); // Minimum width of 100
        } else if (layout.side == (RectCut_Bottom)) {
            *panel_size = std::max(*panel_size - delta, 100); // Minimum height of 100
        } else if (layout.side == (RectCut_Top)) {
            *panel_size = std::max(*panel_size + delta, 100); // Minimum height of 100
        } else if (layout.side == (RectCut_Right)) {
            *panel_size = std::max(*panel_size - delta, 100); // Minimum width of 100
        }

        last_mouse_pos = current_mouse_pos;
    }

    if (!ui_core.mouse_down) {
        is_resizing = false;
    }

    return panel_left;
}


static Rect scrollbar_vert_rect = {0,0,0,0};
static Rect scrollbar_horz_rect = {0,0,0,0};

void begin_scroll_area(float *scroll_x, float *scroll_y, Rect* area)
{
    float scroll_speed = 5.5; // Adjust this to control scroll speed
    float lerp_factor = 0.9f; // Adjust for smoother/more responsive scrolling
    float friction = 0.7; // Adjust for faster/slower deceleration

    // Convert raw input to target scroll delta
    if (std::abs(ui_core.scroll_input) > 0)
    {
        ui_core.target_scroll_delta += ui_core.scroll_input * scroll_speed;
        ui_core.scroll_input = 0; // Reset the input
    }

    // Apply lerp to smooth the scrolling
    ui_core.scroll_delta += (ui_core.target_scroll_delta - ui_core.scroll_delta) * lerp_factor;

    // Apply friction to gradually slow down
    ui_core.target_scroll_delta *= friction;

    // Stop scrolling if speed is very low
    if (std::abs(ui_core.target_scroll_delta) < 0.1f)
    {
        ui_core.target_scroll_delta = 0;
    }

    // If actual scroll is very close to target and target is very small, reset both
    if (std::abs(ui_core.scroll_delta - ui_core.target_scroll_delta) < 0.1f && std::abs(ui_core.target_scroll_delta) < 0.1f)
    {
        ui_core.scroll_delta = 0;
        ui_core.target_scroll_delta = 0;
    }
        

    ui_core.renderer.render_clip(ui_core.renderer.userdata, area);
    if(scroll_y)
    {
        overflow_vert = true;
        scrollbar_vert_rect = cut_right(area, 18);
    }

    if(scroll_x)
    {
        overflow_horz = true;
        scrollbar_horz_rect = cut_bottom(area, 18);
    }
    // Rect resize_handle = add_right(&scrollbar_vert_rect, 18);


    if(scroll_y)
    {
        // finger drag scroll
        if(ui_core.region_hit(*area))
        {
            // printf("yo\n");
            // print_rect(*area);
            ui_core.draw_box(*area,{255,0,255,255});
            // ui_core.draw_box(resize_handle,{255,0,255,255});
            // ui_core.scroll_delta = 0;
            *scroll_y -= ui_core.scroll_delta;
        }
        area->y += *scroll_y *- 1;
    }

    if(scroll_x)
    {
        area->x += *scroll_x *- 1;
    }
}

void end_scroll_area(float *scroll_x, float *scroll_y, Rect* area) {

    if(scroll_y)
    {
        int scrollmax_vert = 0;
        // printf("area->h %i\n", area->h);
        if(area->h < 0)
        {
            scrollmax_vert = area->h * -1;
            
            // Calculate the target scroll position
            float target_scroll = std::min(std::max(*scroll_y, 0.0f), (float)scrollmax_vert);
            
            // Lerp the scroll position
            float lerp_factor = 0.1f; // Adjust this value to control the speed of the lerp
            *scroll_y = *scroll_y + (target_scroll - *scroll_y) * lerp_factor;
            
            // print_rect(*area);
            ui_core.vslider_rect(scrollbar_vert_rect, *scroll_y, 0, scrollmax_vert);
        } else {
            // If there's no scrolling needed, lerp back to 0
            float lerp_factor = 0.1f;
            *scroll_y = *scroll_y + (0 - *scroll_y) * lerp_factor;
        }
    }

    if(scroll_x)
    {
        // printf("area->w %i\n", area->w);
        // print_rect(*area);
        int scrollmax_horz = 0;
        if(area->w < 0)
        {
            scrollmax_horz = (area->w * -1);
            
            // Calculate the target scroll position
            float target_scroll = std::min(std::max(*scroll_x, 0.0f), (float)scrollmax_horz);
            
            // Lerp the scroll position
            float lerp_factor = 0.1f; // Adjust this value to control the speed of the lerp
            // *scroll_x = *scroll_x + (target_scroll - *scroll_x) * lerp_factor;
            *scroll_x = target_scroll;

            // print_rect(*area);
            ui_core.slider_rect(scrollbar_horz_rect, scroll_x, 0, scrollmax_horz);
        } else {
            // If there's no scrolling needed, lerp back to 0
            // float lerp_factor = 0.1f;
            // *scroll_x = *scroll_x + (0 - *scroll_x) * lerp_factor;
            *scroll_x = 0;
        }
    }

    ui_core.renderer.render_clip(ui_core.renderer.userdata, NULL);
    overflow_vert = false;
    overflow_horz = false;
    ui_core.scroll_delta = 0;
}