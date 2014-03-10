#include <pebble.h>
#include <time.h>
#include "num2words.h"

static Window *window;
static TextLayer *textLayer;

#define BUFFER_SIZE 86
static char strbuf[BUFFER_SIZE];

static void update_time(struct tm *t) {
    fuzzy_time_to_words(t->tm_hour, t->tm_min, strbuf, BUFFER_SIZE);
    text_layer_set_text(textLayer, strbuf);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick");
    update_time(tick_time);
}

static void window_load(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Window load");
    Layer *windowLayer = window_get_root_layer(window);
    GRect frame = layer_get_frame(windowLayer);
    GFont font = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);

    window_set_background_color(window, GColorBlack);

    textLayer = text_layer_create(GRect(0, 20, frame.size.w, frame.size.h - 20));
    text_layer_set_text_alignment(textLayer, GTextAlignmentLeft);
    text_layer_set_background_color(textLayer, GColorBlack);
    text_layer_set_text_color(textLayer, GColorWhite);
    text_layer_set_font(textLayer, font);
    layer_add_child(windowLayer, text_layer_get_layer(textLayer));

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    update_time(t);
}

static void window_unload(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Window unload");
    text_layer_destroy(textLayer);
}

static void init(void) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Init");
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    window_stack_push(window, animated);
    tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Deinit");
    tick_timer_service_unsubscribe();
    text_layer_destroy(textLayer);
    window_destroy(window);
}

#if 0
// v1 stuff.

#define MY_UUID { 0xD4, 0xED, 0x05, 0xFF, 0x75, 0x8B, 0x46, 0x6F, 0x80, 0xBF, 0x55, 0xFA, 0xD9, 0xCB, 0xAA, 0x3C }

static void handle_init(AppContextRef ctx) {
    (void) ctx;

    window_init(&s_data.window, "My Fuzzy Time");
    const bool animated = true;
    window_stack_push(&s_data.window, animated);

    window_set_background_color(&s_data.window, GColorBlack);
    GFont gotham = fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD);

    text_layer_init(&s_data.label, GRect(0, 20, s_data.window.layer.frame.size.w, s_data.window.layer.frame.size.h - 20) );
    text_layer_set_background_color(&s_data.label, GColorBlack);
    text_layer_set_text_color(&s_data.label, GColorWhite);
    text_layer_set_font(&s_data.label, gotham);
    layer_add_child(&s_data.window.layer, &s_data.label.layer);

    PblTm t;
    get_time(&t);
    update_time(&t);
}
#endif

int main(void) {
    init();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
    app_event_loop();
    deinit();
}
