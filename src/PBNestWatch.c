#include <pebble.h>

static Window *window;

static TextLayer *temperature_layer;
static TextLayer *humidity_layer;
static TextLayer *heat_layer;
static TextLayer *ac_layer;

static char temperature[16];
static char humidity[16];
static char heat[16];
static char ac[16];

static AppSync sync;
static uint8_t sync_buffer[128];

enum HouseKeys {
    TEMPERATURE_KEY = 0x1,
    HUMIDITY_KEY = 0x2,
    HEAT_KEY = 0x3,
    AC_KEY = 0x4,
};

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Dictionary Error: %d", dict_error);

}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
    switch (key) {
        case TEMPERATURE_KEY:
            // App Sync keeps new_tuple in sync_buffer, so we may use it directly
            text_layer_set_text(temperature_layer, new_tuple->value->cstring);
            break;
        case HUMIDITY_KEY:
            text_layer_set_text(humidity_layer, new_tuple->value->cstring);
            break;
        case HEAT_KEY:
            text_layer_set_text(heat_layer, new_tuple->value->cstring);
            break;
        case AC_KEY:
            text_layer_set_text(ac_layer, new_tuple->value->cstring);
            break;
    }
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    //temperature layer
    temperature_layer = text_layer_create(GRect(0, 30, 144, 68));
    text_layer_set_text_color(temperature_layer, GColorWhite);
    text_layer_set_background_color(temperature_layer, GColorClear);
    text_layer_set_font(temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(temperature_layer, GTextAlignmentCenter);
    text_layer_set_text(temperature_layer, temperature);
    //humidity layer
    humidity_layer = text_layer_create(GRect(0, 60, 144, 68));
    text_layer_set_text_color(humidity_layer, GColorWhite);
    text_layer_set_background_color(humidity_layer, GColorClear);
    text_layer_set_font(humidity_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(humidity_layer, GTextAlignmentCenter);
    text_layer_set_text(humidity_layer, humidity);
    //heat layer
    heat_layer = text_layer_create(GRect(10, 100, 144, 68));
    text_layer_set_text_color(heat_layer, GColorWhite);
    text_layer_set_background_color(heat_layer, GColorClear);
    text_layer_set_font(heat_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(humidity_layer, GTextAlignmentCenter);
    text_layer_set_text(heat_layer, heat);
    //AC layer
    ac_layer = text_layer_create(GRect(75, 100, 144, 68));
    text_layer_set_text_color(ac_layer, GColorWhite);
    text_layer_set_background_color(ac_layer, GColorClear);
    text_layer_set_font(ac_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(humidity_layer, GTextAlignmentCenter);
    text_layer_set_text(ac_layer, ac);
    //initial values to set the text as
    Tuplet initial_values[] = {
        TupletCString(TEMPERATURE_KEY, "Open App"),
        TupletCString(HUMIDITY_KEY,"..."),
        TupletCString(HEAT_KEY,"Heat: N/A"),
        TupletCString(AC_KEY,"AC: N/A"),
    };
    app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                  sync_tuple_changed_callback, sync_error_callback, NULL);
    
    layer_add_child(window_layer, text_layer_get_layer(temperature_layer));
    layer_add_child(window_layer, text_layer_get_layer(humidity_layer));
    layer_add_child(window_layer, text_layer_get_layer(heat_layer));
    layer_add_child(window_layer, text_layer_get_layer(ac_layer));

}

static void window_unload(Window *window) {
    app_sync_deinit(&sync);
    text_layer_destroy(temperature_layer);
    text_layer_destroy(humidity_layer);
    text_layer_destroy(heat_layer);
    text_layer_destroy(ac_layer);

}

static void init() {
    window = window_create();
    window_set_background_color(window, GColorBlack);
    window_set_fullscreen(window, true);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });
    
    const int inbound_size = 128;
    const int outbound_size = 16;
    app_message_open(inbound_size, outbound_size);
    
    const bool animated = true;
    window_stack_push(window, animated);
}

static void deinit() {
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
