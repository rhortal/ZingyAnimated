#include <pebble.h>
#include <math.h>

static Window *window;
static TextLayer *time_layer;
static BitmapLayer *zingy_layer;
static int current_zingy;
static AppTimer *timer;
static Layer *window_layer;
static GRect bounds;


uint32_t zingy_names[2] = {RESOURCE_ID_ZINGY1, RESOURCE_ID_ZINGY2};
GBitmap* zingys[2];

static void window_load(Window *window) {
  zingy_layer = bitmap_layer_create((GRect) { .origin = { 6, 6 }, .size = { bounds.size.w, 224 } });
  bitmap_layer_set_bitmap(zingy_layer, gbitmap_create_with_resource(RESOURCE_ID_ZINGY1));
  layer_add_child(window_layer, bitmap_layer_get_layer(zingy_layer));

  current_zingy = 0;

  time_layer = text_layer_create((GRect) { .origin = {50, 134}, .size = { 104, 40 } });
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_text_color(time_layer, GColorBlack);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  /* text_layer_set_text(time_layer, "USA"); */
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
}
void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  static char time_text[] = " 0:00";
  char *time_format;
  if (!clock_is_24h_style()){ 
    time_format = "%l:%M";
  }else{
    time_format = "%H:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);
  text_layer_set_text(time_layer, time_text);
}

static void timer_callback(void *data) {
  bitmap_layer_set_bitmap(zingy_layer, zingys[current_zingy]);

  current_zingy++;
  if(current_zingy > 1){
    current_zingy = 0;
  }
  timer = app_timer_register(100, timer_callback, NULL);
}
void start_animation(void){
  timer = app_timer_register(100, timer_callback, NULL);
}
static void window_unload(Window *window) {
  text_layer_destroy(time_layer);
  bitmap_layer_destroy(zingy_layer);
}

static void init(void) {
  window = window_create();
  window_layer = window_get_root_layer(window);
  bounds = layer_get_frame(window_layer);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  for(int i = 0; i < 2; i++){
      zingys[i] = gbitmap_create_with_resource(zingy_names[i]);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "loaded bitmap %u", i);
  }

  start_animation();
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init end");
}
static void deinit(void) {
  tick_timer_service_unsubscribe();
  layer_destroy(window_layer);
  window_destroy(window);
}
int main(void) {
  init();
  app_event_loop();
  deinit();
}