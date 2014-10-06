#include "main.h"
#include <pebble.h>
#include "config.c"

static Window *w_main;
static GFont r_gothic_24_bold;
static GFont r_gothic_18;
static GBitmap *r_icon_upvote;
static TextLayer *t_name;
static TextLayer *t_time;
static TextLayer *t_upvote;
static BitmapLayer *i_upvote;
static InverterLayer *l_inverter;

static char upvoteText[4];

int EVENT_COUNT = 0;
int SKIP = 1;

/** App Comms **/

void send_connect_event_to_phone() {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if(iter == NULL) {
		return;
	}
	
	static char message[10] = "CONNECT";
	
	dict_write_cstring(iter, 0, message);
	dict_write_end(iter);
	
	app_message_outbox_send();
}

void send_event_request_to_phone() {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if(iter == NULL) {
		return;
	}
	
	dict_write_int8(iter, 0, SKIP);
	dict_write_end(iter);
	
	app_message_outbox_send();
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Message Received:Success");
	
	Tuple *event_count_tuple = dict_find(iter, KEY_EVENT_COUNT);
	Tuple *name_tuple = dict_find(iter, KEY_NAME);
	Tuple *time_tuple = dict_find(iter, KEY_TIME);
	Tuple *upvote_tuple = dict_find(iter, KEY_UPVOTE);
	
	if(event_count_tuple) {
		EVENT_COUNT = event_count_tuple->value->uint8;
	}
	
	if(name_tuple) {
		set_event_name(name_tuple->value->cstring);
	}
	
	if(time_tuple) {
		set_event_time(time_tuple->value->cstring);
	}
	
	if(upvote_tuple) {
		set_event_upvote(upvote_tuple->value->uint8);
	}
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message Received:Failure");
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Message Send:Success");
}

void out_failed_handler(DictionaryIterator *send, AppMessageResult reason, void *context) {
	char* message;
	
	switch(reason) {
		case APP_MSG_OK:
			message = "APP_MSG_OK";
			break;
		case APP_MSG_SEND_TIMEOUT:
			message = "APP_MSG_SEND_TIMEOUT";
			break;
		case APP_MSG_SEND_REJECTED:
			message = "APP_MSG_SEND_REJECTED";
			break;
		case APP_MSG_NOT_CONNECTED:
			message = "APP_MSG_NOT_CONNECTED";
			break;
		case APP_MSG_APP_NOT_RUNNING:
			message = "APP_MSG_APP_NOT_RUNNING";
			break;
		case APP_MSG_INVALID_ARGS:
			message = "APP_MSG_INVALID_ARGS";
			break;
		case APP_MSG_BUFFER_OVERFLOW:
			message = "APP_MSG_BUFFER_OVERFLOW";
			break;
		case APP_MSG_ALREADY_RELEASED:
			message = "APP_MSG_ALREADY_RELEASED";
			break;
		case APP_MSG_CALLBACK_ALREADY_REGISTERED:
			message = "APP_MSG_CALLBACK_ALREADY_REGISTERED";
			break;
		case APP_MSG_CALLBACK_NOT_REGISTERED:
			message = "APP_MSG_CALLBACK_NOT_REGSITERED";
			break;
		case APP_MSG_OUT_OF_MEMORY:
			message = "APP_MSG_OUT_OF_MEMORY";
			break;
		case APP_MSG_CLOSED:
			message = "APP_MSG_CLOSED";
			break;
		case APP_MSG_INTERNAL_ERROR:
			message = "APP_MSG_INTERNAL_ERROR";
			break;
		default:
			message = "OTHER";
	}
	
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message Send:Failure");
	APP_LOG(APP_LOG_LEVEL_ERROR, message);
}

/** UI **/

static void initialise_ui(void) {
	w_main = window_create();
	window_set_background_color(w_main, GColorBlack);
	window_set_fullscreen(w_main, false);
	
	r_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
	r_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
	r_icon_upvote = gbitmap_create_with_resource(RESOURCE_ID_ICON_UPVOTE_WHITE);
	
	t_name = text_layer_create(GRect(5, 0, 134, 110));
	text_layer_set_background_color(t_name, GColorBlack);
	text_layer_set_text_color(t_name, GColorWhite);
	text_layer_set_text(t_name, "");
	text_layer_set_font(t_name, r_gothic_24_bold);
	layer_add_child(window_get_root_layer(w_main), (Layer *)t_name);
	
	t_time = text_layer_create(GRect(5, 127, 67, 20));
	text_layer_set_background_color(t_time, GColorBlack);
	text_layer_set_text_color(t_time, GColorWhite);
	text_layer_set_text(t_time, "");
	text_layer_set_font(t_time, r_gothic_18);
	layer_add_child(window_get_root_layer(w_main), (Layer *)t_time);
	
	t_upvote = text_layer_create(GRect(72, 127, 52, 20));
	text_layer_set_background_color(t_upvote, GColorBlack);
	text_layer_set_text_color(t_upvote, GColorWhite);
	text_layer_set_text(t_upvote, "0");
	text_layer_set_text_alignment(t_upvote, GTextAlignmentRight);
	text_layer_set_font(t_upvote, r_gothic_18);
	layer_add_child(window_get_root_layer(w_main), (Layer *)t_upvote);
	
	i_upvote = bitmap_layer_create(GRect(129, 136, 10, 8));
	bitmap_layer_set_bitmap(i_upvote, r_icon_upvote);
	bitmap_layer_set_background_color(i_upvote, GColorBlack);
	layer_add_child(window_get_root_layer(w_main), (Layer *)i_upvote);
	
	l_inverter = inverter_layer_create(GRect(0, 0, 144, 152));
	layer_add_child(window_get_root_layer(w_main), (Layer *)l_inverter);
}

/** Public Methods **/

void set_event_name(char * _name) {
	text_layer_set_text(t_name, _name);
}

void set_event_time(char * _time) {
	text_layer_set_text(t_time, _time);
}

void set_event_upvote(int _upvote) {
	snprintf(upvoteText, 4, "%d", _upvote);
	
	text_layer_set_text(t_upvote, upvoteText);
}

static void destroy_ui(void) {
	window_destroy(w_main);
	text_layer_destroy(t_name);
	text_layer_destroy(t_time);
	text_layer_destroy(t_upvote);
	bitmap_layer_destroy(i_upvote);
	gbitmap_destroy(r_icon_upvote);
}

static void handle_window_unload(Window* window) {
	destroy_ui();
}

/** Event Handlers **/

static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	if(SKIP > 1) {
		SKIP--;
		
		send_event_request_to_phone();
	}
}

static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	if(SKIP < EVENT_COUNT) {
		SKIP++;
		
		send_event_request_to_phone();
	}
}

/** Window Management **/

static void config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
}

void show_main(void) {
	initialise_ui();
	
	window_set_window_handlers(w_main, (WindowHandlers) {
		.unload = handle_window_unload,
	});
	
	window_set_click_config_provider(w_main, config_provider);
	
	window_stack_push(w_main, true);
	
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
	
	send_connect_event_to_phone();
}

void hide_main(void) {
	window_stack_remove(w_main, true);
}