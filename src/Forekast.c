#include <pebble.h>
#include "main.h"

/** Pebble **/

static void init() {
	const uint32_t inbound_size = 118;
	const uint32_t outbound_size = 10;
	app_message_open(inbound_size, outbound_size);
	
	show_main();
}

static void deinit() {
	hide_main();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}