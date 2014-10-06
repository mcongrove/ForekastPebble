var API_URL = "https://forekast.com/api/events/eventsByDate.json?subkasts%5B%5D=TV&subkasts%5B%5D=TVM&subkasts%5B%5D=SE&subkasts%5B%5D=ST&subkasts%5B%5D=TE&subkasts%5B%5D=HAW&subkasts%5B%5D=PRP&subkasts%5B%5D=HA&subkasts%5B%5D=EDU&subkasts%5B%5D=MA&subkasts%5B%5D=ART&subkasts%5B%5D=GM&subkasts%5B%5D=OTH&datetime=2014-10-10+00%3A00%3A00&zone_offset=360&country=US";

var EVENTS = [];

Pebble.addEventListener("ready", function(_event) {
	getEvents();
});

Pebble.addEventListener("appmessage", function(_event) {
	console.log(JSON.stringify(_event.payload["0"]));
	
	if(_event.payload["0"]) {
		retrieveEvent(_event.payload["0"] - 1);
	}
});

function getEvents() {
	var http = new XMLHttpRequest();
	
	http.open(
		"GET",
		API_URL,
		true
	);
	
	http.onload = function(_event) {
		if(http.readyState == 4) {
			if(http.status == 200) {
				var response = JSON.parse(http.responseText);
				
				EVENTS = response;
				
				retrieveEvent(0);
			} else {
				console.log("HTTP Request Error");
			}
		}
	};
	
	http.send(null);
}

function retrieveEvent(_skip) {
	var event = EVENTS[_skip];
	
	console.log(JSON.stringify({
		"0": EVENTS.length,
		"1": event.name.substring(0, 90),
		"2": "All Day",
		"3": event.upvotes
	}));
	
	Pebble.sendAppMessage({
		"0": EVENTS.length,
		"1": event.name.substring(0, 90),
		"2": "All Day",
		"3": event.upvotes
	});
}