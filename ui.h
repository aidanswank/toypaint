#include <algorithm>

typedef uint64_t UIId;

struct Vec2 { int x, y; };
struct Rect { int x, y, w, h; };
struct Color { unsigned char r, g, b, a; };

// https://web.archive.org/web/20210306102303/https://halt.software/dead-simple-layouts/

typedef enum {
    RectCut_Left,
    RectCut_Right,
    RectCut_Top,
    RectCut_Bottom
} RectCutSide;

typedef struct {
    Rect* rect;
    RectCutSide side;
} RectCut;

Rect cut_left(Rect* rect, int a) {
    int x = rect->x;
    int w = std::min(a, rect->w);
    rect->x += w;
    rect->w -= w;
    return {x, rect->y, w, rect->h};
}

Rect cut_right(Rect* rect, int a) {
    int w = std::min(a, rect->w);
    rect->w -= w;
    return {rect->x + rect->w, rect->y, w, rect->h};
}

Rect cut_top(Rect* rect, int a) {
    int y = rect->y;
    int h = std::min(a, rect->h);
    rect->y += h;
    rect->h -= h;
    return {rect->x, y, rect->w, h};
}

Rect cut_bottom(Rect* rect, int a) {
    int h = std::min(a, rect->h);
    rect->h -= h;
    return {rect->x, rect->y + rect->h, rect->w, h};
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
        default: abort();
    }
}

struct UITheme
{
	Color button_color = {111,105,129,220};
    Color hot_color = {255,255,255,100};
    Color flash_color = {255,255,255,255};
    Color text_color = {255,255,255,255};
    Color panel_color = {27,27,28,255};
    Color label_bg_color = {180,24,20,255};
    Color slider_handle_color = {82,65,90,124};
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
    void (*draw_string)(const char* string, Vec2 pos, Color color, void* userdata);
    int (*get_text_width)(const char* string, void* userdata);
    int (*get_text_height)(const char* string, void* userdata);
};

/*
much help
https://www.youtube.com/watch?v=V_phHKr0yRY
https://solhsa.com/imgui/
*/

struct UICore
{
    Vec2 mouse_pos = {0,0};
    bool mouse_down = 0;
    // int mouse_up_once = 0;
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

    void label(RectCut layout, const char* label)
    {
        float size = get_text_width(label) + (theme.padding*2);
        float text_height = get_text_height(label);
        // Rect rect = cut_left(layout, size);
        Rect rect = rectcut_cut(layout, size);

        // draw_rect(rect, theme.label_bg_color);
        
        // bool clicked = button_rect(rect);
        draw_string(label, {rect.x + theme.padding, rect.y + int(rect.h/2) - int(text_height/2)}, theme.text_color);
    }

    bool vslider_rect(Rect rect, float &value, float min_value, float max_value, bool flip = false) {
        int id = next_id();
        draw_rect(rect, {200, 200, 200, 255}); // Draw the slider track

        if (region_hit(rect)) {
            next_hover = id;
        }

        if (hover == id) {
            draw_rect(rect, {255, 255, 255, 100});
        }

        if (active == id && mouse_down) {
            // Update value based on mouse position relative to the slider track
            float mouse_ratio = (float)(mouse_pos.y - rect.y) / (float)rect.h;
            if (flip) {
                mouse_ratio = 1.0f - mouse_ratio; // Invert the ratio if flipped
            }
            value = mouse_ratio * (max_value - min_value) + min_value;
            // Clamp value within the specified range
            value = std::max(min_value, std::min(max_value, value));
            next_hover = id; // Hold the hover state until release

            draw_rect(rect, {0, 255, 0, 100});
        }

        // Calculate handle position based on current value
        int handle_height = 20; // Height of the handle
        float value_ratio = (value - min_value) / (max_value - min_value);
        if (flip) {
            value_ratio = 1.0f - value_ratio; // Invert the ratio if flipped
        }
        int handle_y = rect.y + (int)(value_ratio * (rect.h - handle_height));
        Rect handle_rect = { rect.x, handle_y, rect.w, handle_height }; // Handle rect within slider track

        draw_rect(handle_rect, {255, 0, 0, 100}); // Draw the slider handle

        // Convert float value to string
        char value_str[16];
        snprintf(value_str, sizeof(value_str), "%.2f", value);

        // Calculate text dimensions
        int text_width = get_text_width(value_str);
        int text_height = get_text_height(value_str);

        // Calculate text position to be in the middle of the slider
        Vec2 text_pos = { rect.x + (rect.w - text_width) / 2, rect.y + (rect.h - text_height) / 2 };

        // Draw the slider value
        draw_string(value_str, text_pos, theme.text_color);

        return (active == id && mouse_down);
    }

    bool slider_rect(Rect rect, float *value, float min_value, float max_value) {
        int id = next_id();
        draw_rect(rect, {200, 200, 200, 100}); // Draw the slider track
        
        if (region_hit(rect)) {
            next_hover = id;
        }
        
        if (hover == id) {
            draw_rect(rect, {255, 255, 255, 100});
        }
        
        if (active == id && mouse_down) {
            // Update value based on mouse position relative to the slider track
            *value = (float)(mouse_pos.x - rect.x) / (float)rect.w * (max_value - min_value) + min_value;
            // Clamp value within the specified range
            *value = std::max(min_value, std::min(max_value, *value));
            next_hover = id; // Hold the hover state until release
            
            draw_rect(rect, {0, 255, 0, 100});
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