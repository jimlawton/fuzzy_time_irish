#include <pebble.h>
#include <time.h>
#include "num2words.h"

static Window *window;
static TextLayer *textLayer;

#define BUFFER_SIZE 86
static char sdata[BUFFER_SIZE];

static void update_time(struct tm *t) {
    fuzzy_time_to_words(t->tm_hour, t->tm_min, sdata, BUFFER_SIZE);
    text_layer_set_text(textLayer, sdata);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
    APP_LOG(APP_LOG_DEBUG, "Tick!");
    update_time(tick_time);
}

static void window_load(Window *window) {
    Layer *windowLayer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(windowLayer);

    textLayer = text_layer_create((GRect) {
        .origin = { 0, 72 },
        .size = { bounds.size.w, 20 }
    });
    text_layer_set_text(textLayer, "Press a button");
    text_layer_set_text_alignment(textLayer, GTextAlignmentCenter);
    layer_add_child(windowLayer, text_layer_get_layer(textLayer));
}

static void window_unload(Window *window) {
    text_layer_destroy(text_layer);
}

static void init(void) {
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
    tick_timer_service_unsubscribe();
    text_layer_destroy(textLayer);
    window_destroy(window);
}

int main(void) {
    init();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
    app_event_loop();
    deinit();
}
