#include <pebble.h>
#define DATE_FORMAT 0
#define BG_COLOR 1

// Constants
  
const char* kDateFormat[] = { "%a %D", "%a %d/%m/%y"};

// Variables
  
static Window *s_main_window;  

static TextLayer *s_time_layer;
static TextLayer *s_ampm_layer;
static TextLayer *s_date_layer;
  
static GFont s_time_font;
static GFont s_ampm_font;
static GFont s_date_font;

static BitmapLayer *s_background_layer, *s_bt_icon_layer;
static GBitmap *s_background_bitmap, *s_bt_icon_bitmap;

static GBitmap *s_heart_full;
static GBitmap *s_heart_1;
static GBitmap *s_heart_2;
static GBitmap *s_heart_3;
static GBitmap *s_heart_4;
static Layer *s_hearts_layer;

static int date;

const uint32_t colors[2] = {RESOURCE_ID_IMAGE_TRIFORCE_BG_GREEN,
                            RESOURCE_ID_IMAGE_TRIFORCE_BG_RED};

// --- Time and Date Functions ---

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof("00:00"), "%l:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  
  // Display AM/PM on the TextLayer
  if(!clock_is_24h_style()) {
    int hours = tick_time->tm_hour;
    if(hours < 12) {
      text_layer_set_text(s_ampm_layer, "am");
    } else {
      text_layer_set_text(s_ampm_layer, "pm");
    }
  }
}

static void update_date() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "DDD dd/mm/yy";

  // Write the current date into the buffer
  date = persist_read_int(DATE_FORMAT);
  strftime(buffer, sizeof("DDD dd/mm/yy"), kDateFormat[date], tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_date_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_date();
}



// --- App Connection Handlers ---

static void in_recv_handler(DictionaryIterator *iter, void *context)
{ 
  // Read Background Color preference
  Tuple *color_t = dict_find(iter, MESSAGE_KEY_BG_COLOR);
  if(color_t) {
    switch(color_t->key) {
    case BG_COLOR:
      // Check for the BG_COLOR key
      if(strcmp(color_t->value->cstring, "green") == 0)
      { 
        persist_write_int(BG_COLOR, 0);
      }
      else if(strcmp(color_t->value->cstring, "red") == 0)
      { 
        persist_write_int(BG_COLOR, 1);
      }
      else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Error: in_recv_handler failed to recognize color_t.");
        APP_LOG(APP_LOG_LEVEL_DEBUG, color_t->value->cstring);
      }
      s_background_bitmap = gbitmap_create_with_resource(colors[persist_read_int(BG_COLOR)]);
      bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    }
  }
  
  // Read Date Format preference
  Tuple *date_t = dict_find(iter, MESSAGE_KEY_DATE_FORMAT);
  if(date_t) {
    switch(date_t->key) {
    case DATE_FORMAT:
      // Check for the DATE_FORMAT key
      if(strcmp(date_t->value->cstring, "usa") == 0)
      {
        // Set and save as MM/DD
        persist_write_int(DATE_FORMAT, 0);
      }
      else if(strcmp(date_t->value->cstring, "global") == 0)
      {
        // Set and save as DD/MM
        persist_write_int(DATE_FORMAT, 1);
      }
      else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Error: in_recv_handler failed to recognize date_t.");
        APP_LOG(APP_LOG_LEVEL_DEBUG, date_t->value->cstring);
      }
      update_date();
    }
  }
}

void prv_init(void) {
  // Open AppMessage connection
  app_message_register_inbox_received(in_recv_handler);
  app_message_open(128, 128);
}


// === Fill the Battery Meter ===
// I really gotta make this better.
// Like ASAP.

static void battery_level_draw(Layer *layer, GContext *ctx) {  
  // Check the battery level
  BatteryChargeState state = battery_state_service_peek();
  
  if (state.charge_percent < 36) {
    // Select 1st heart image
    if (state.charge_percent > 28) {
      graphics_draw_bitmap_in_rect(ctx, s_heart_full, GRect(0, 0, 18, 16));
    }
    else if (state.charge_percent > 20) {
      graphics_draw_bitmap_in_rect(ctx, s_heart_1, GRect(0, 0, 18, 16));
    }
    else if (state.charge_percent > 12) {
      graphics_draw_bitmap_in_rect(ctx, s_heart_2, GRect(0, 0, 18, 16));
    }
    else {
      graphics_draw_bitmap_in_rect(ctx, s_heart_3, GRect(0, 0, 18, 16));
    }
    // Display remaining 2 hearts as empty
    graphics_draw_bitmap_in_rect(ctx, s_heart_4, GRect(20, 0, 18, 16));
    graphics_draw_bitmap_in_rect(ctx, s_heart_4, GRect(40, 0, 18, 16));
  } 
  else if (state.charge_percent < 68) {
    // Draw 1st heart as full
    graphics_draw_bitmap_in_rect(ctx, s_heart_full, GRect(0, 0, 18, 16));
    // Select 2nd heart image
    if (state.charge_percent > 60) {
      graphics_draw_bitmap_in_rect(ctx, s_heart_full, GRect(20, 0, 18, 16));
    }
    else if (state.charge_percent > 52) {
      graphics_draw_bitmap_in_rect(ctx, s_heart_1, GRect(20, 0, 18, 16));
    }
    else if (state.charge_percent > 44) {
      graphics_draw_bitmap_in_rect(ctx, s_heart_2, GRect(20, 0, 18, 16));
    }
    else {
      graphics_draw_bitmap_in_rect(ctx, s_heart_3, GRect(20, 0, 18, 16));
    }
    // Draw last heart as empty
    graphics_draw_bitmap_in_rect(ctx, s_heart_4, GRect(40, 0, 18, 16));
  } 
  else {
    // Draw first 2 hearts as full
    graphics_draw_bitmap_in_rect(ctx, s_heart_full, GRect(0, 0, 18, 16));
    graphics_draw_bitmap_in_rect(ctx, s_heart_full, GRect(20, 0, 18, 16));
    // Select last heart image
    if (state.charge_percent > 92) {
      graphics_draw_bitmap_in_rect(ctx, s_heart_full, GRect(40, 0, 18, 16));
    }
    else if (state.charge_percent > 84) {
      graphics_draw_bitmap_in_rect(ctx, s_heart_1, GRect(40, 0, 18, 16));
    }
    else if (state.charge_percent > 76) {
      graphics_draw_bitmap_in_rect(ctx, s_heart_2, GRect(40, 0, 18, 16));
    }
    else {
      graphics_draw_bitmap_in_rect(ctx, s_heart_3, GRect(40, 0, 18, 16));
    }
  }
}

// --- Subscribe to Bluetooth connection ---

static void bluetooth_callback(bool connected) {
  // Show icon if disconnected
  layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), connected);

  if(!connected) {
    // Issue a vibrating alert
    vibes_double_pulse();
  }
}


// --- Establishing the Main Window ---

static void main_window_load(Window *window) {  
  // Create background layer using selected background
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  s_background_bitmap = gbitmap_create_with_resource(colors[persist_read_int(BG_COLOR)]);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  // Load heart images
  s_heart_full = gbitmap_create_with_resource(RESOURCE_ID_HEART_FULL);
  s_heart_1 = gbitmap_create_with_resource(RESOURCE_ID_HEART_1);
  s_heart_2 = gbitmap_create_with_resource(RESOURCE_ID_HEART_2);
  s_heart_3 = gbitmap_create_with_resource(RESOURCE_ID_HEART_3);
  s_heart_4 = gbitmap_create_with_resource(RESOURCE_ID_HEART_4);
  
  // Create time TextLayer
  if(clock_is_24h_style()) {
    s_time_layer = text_layer_create(GRect(0, -5, 140, 45));
  } else {
    s_time_layer = text_layer_create(GRect(0, -5, 123, 45));
  }
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  
  // Create AM/PM TextLayer
  s_ampm_layer = text_layer_create(GRect(124, 21, 30, 20));
  text_layer_set_background_color(s_ampm_layer, GColorClear);
  text_layer_set_text_color(s_ampm_layer, GColorWhite);

  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(0, 128, 144, 20));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  
  // Create GFonts
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TRIFORCE_36));
  s_ampm_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TRIFORCE_16));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TRIFORCE_16));

  // Improve the layout of the watchface
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
  text_layer_set_font(s_ampm_layer, s_ampm_font);
  text_layer_set_text_alignment(s_ampm_layer, GTextAlignmentLeft);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Add child layers to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_ampm_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
  // Draw hearts on BitmapLayer
  s_hearts_layer = layer_create(GRect(43, 152, 60, 16));
  layer_set_update_proc(s_hearts_layer, battery_level_draw);
  layer_add_child(window_get_root_layer(window), s_hearts_layer);
  
  // Create the Bluetooth icon GBitmap
  s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BT_ICON);

  // Create the BitmapLayer to display the GBitmap
  s_bt_icon_layer = bitmap_layer_create(GRect(0, 5, 30, 30));
  bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_icon_layer));

  // Show the correct state of the BT connection from the start
  bluetooth_callback(bluetooth_connection_service_peek());
}

static void main_window_unload(Window *window) {
  //Unsubscribe from battery state
  battery_state_service_unsubscribe();
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_ampm_layer);
  text_layer_destroy(s_date_layer);
  
  // Destroy hearts
  layer_destroy(s_hearts_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_ampm_font);
  fonts_unload_custom_font(s_date_font); 
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy Bluetooth layer and bitmap
  gbitmap_destroy(s_bt_icon_bitmap);
  bitmap_layer_destroy(s_bt_icon_layer);
}


// --- Load and Unload Functions ---

static void init() {
  // Open AppMessage to communicate with the phone
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register for Bluetooth connection updates
  bluetooth_connection_service_subscribe(bluetooth_callback);
  
  // Make sure the time is displayed from the start
  update_time();
  update_date();
}

static void deinit() {
  // Deregister from TickTimerService
  tick_timer_service_unsubscribe();
  
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

