#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer, *s_emoji_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // tm structureを取得
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // 今の時と分をbufferに書き込む
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  
  // TextLayerに時間を表示する
  text_layer_set_text(s_time_layer, s_buffer);
  
}

static void main_window_load(Window *window) {
  // Windowの情報を取得する
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // GBitmapを作成
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_KOMBU_BACKGROUND);
    
  // GBitmapを表示させるBitmapLayerを作成
  s_background_layer = bitmap_layer_create(
   GRect(0, 0, 180, 180)
  );
  
  // Bitmapをレイヤーにセットしてwindowに追加する
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  // テキストレイヤーを作成
  s_time_layer = text_layer_create(
    GRect(0, 30, bounds.size.w, 50));

  s_emoji_layer = text_layer_create(
    GRect(50, 75, 180, 180)
  );
  
  // watchfaceっぽくする
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text(s_emoji_layer, "\U0001F618");

  // emoji layer
  text_layer_set_background_color(s_emoji_layer, GColorClear);
  text_layer_set_text_color(s_emoji_layer, GColorWhite);
  
  // GFontを作成
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_30));
  
  // Textlayerにアプライ
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_emoji_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  // Windowのroot layerに子レイヤーとして追加する
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_emoji_layer));
  
}

static void main_window_unload(Window *window) {
  // テキストレイヤーをDESTROY!!!
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_emoji_layer);
  
  // GBitmapを DESTORY!
  gbitmap_destroy(s_background_bitmap);
  
  // Bitmap LayerをDESTROY!!
  bitmap_layer_destroy(s_background_layer);
  
  // GFontをunload
  fonts_unload_custom_font(s_time_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // main Window要素を作ってポインターをアサインする
  s_main_window = window_create();

  // Window内の要素を操作するためのハンドラーをセット
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // pebbleにウィンドウを表示する
  window_stack_push(s_main_window, true);
  
  // 背景色を指定
  window_set_background_color(s_main_window, GColorBlack);
  
  // TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // 最初から表示されるように
  update_time();
}

static void deinit() {
  // Windowをこわす
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}