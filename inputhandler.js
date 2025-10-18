const SDL_KEYDOWN = 0x300
const SDL_KEYUP   = 0x301


const SDL_EVENT_MOUSE_MOTION    = 0x400 /**< Mouse moved */
const SDL_EVENT_MOUSE_BUTTON_DOWN = 0x401      /**< Mouse button pressed */
const SDL_EVENT_MOUSE_BUTTON_UP =0x402         /**< Mouse button released */

canvas = document.getElementById("canvas")
canvas.addEventListener("wheel", (ev) => {
		console.log(ev);
		
		const event_buf_addr = _get_event_buf();
		evbuf = new Int32Array(wasmMemory.buffer);
		evbuf8 = new Int8Array(wasmMemory.buffer);
		evbuf[event_buf_addr/4] = SDL_KEYDOWN;
		evbuf[event_buf_addr/4+3] = (ev.type=="keydown");
		var keyCode = (ev.deltaY < 0 ? '+' : '-').charCodeAt(0);
		evbuf[event_buf_addr/4+5] = keyCode;
		_event_added();
});
canvas.addEventListener("pointermove", (ev) => {
		const w = canvas.scrollWidth;
		const h = canvas.scrollHeight;
		const event_buf_addr = _get_event_buf();
		evbuf = new Int32Array(wasmMemory.buffer);
		evbuf[event_buf_addr/4] = SDL_EVENT_MOUSE_MOTION;
		evbuf[event_buf_addr/4 + 4] = (ev.buttons & 1) + ((ev.buttons & 2) << 1) + ((ev.buttons & 4) >> 1);
		evbuf[event_buf_addr/4 + 5] = ev.clientX * 1920 / w;
		evbuf[event_buf_addr/4 + 6] = ev.clientY * 1080 / h;
		evbuf[event_buf_addr/4 + 7] = ev.movementX * 1920 / w;
		evbuf[event_buf_addr/4 + 8] = ev.movementY * 1080 / h;

		_event_added();
		
});
canvas.addEventListener("mousedown", (ev) => {
		const w = canvas.scrollWidth;
		const h = canvas.scrollHeight;
		const event_buf_addr = _get_event_buf();
		console.log("ev_addr:", event_buf_addr);
		evbuf = new Int32Array(wasmMemory.buffer);
		evbuf[event_buf_addr/4] = SDL_EVENT_MOUSE_BUTTON_DOWN;
		evbuf[event_buf_addr/4 + 5] = ev.clientX * 1920 / w;
		evbuf[event_buf_addr/4 + 6] = ev.clientY * 1080 / h;
		console.log("mpos:",ev.clientX,ev.clientY);

		_event_added();
});
canvas.addEventListener("mouseup", (ev) => {
		const w = canvas.scrollWidth;
		const h = canvas.scrollHeight;
		const event_buf_addr = _get_event_buf();
		console.log("ev_addr:", event_buf_addr);
		evbuf = new Int32Array(wasmMemory.buffer);
		evbuf[event_buf_addr/4] = SDL_EVENT_MOUSE_BUTTON_UP;
		evbuf[event_buf_addr/4 + 5] = ev.clientX * 1920 / w;
		evbuf[event_buf_addr/4 + 6] = ev.clientY * 1080 / h;
		console.log("mpos:",ev.clientX,ev.clientY);

		_event_added();
});
keylistener = (ev) => {
		const event_buf_addr = _get_event_buf();
		evbuf = new Int32Array(wasmMemory.buffer);
		evbuf8 = new Int8Array(wasmMemory.buffer);
		evbuf[event_buf_addr/4] = SDL_KEYDOWN;
		evbuf[event_buf_addr/4+3] = (ev.type=="keydown");
		var keyCode = ev.keyCode
		if (keyCode == 37)keyCode =      0x40000050 // left
		else if (keyCode == 38)keyCode = 0x40000052 // up
		else if (keyCode == 39)keyCode = 0x4000004f // right
		else if (keyCode == 40)keyCode = 0x40000051 // down 
		else if (keyCode > 31){
			keyCode = ev.key.charCodeAt(0)
		}
		evbuf[event_buf_addr/4+5] = keyCode;
		_event_added();
};
body = document.getElementsByTagName("body")[0];
body.addEventListener("keydown", keylistener);
body.addEventListener("keyup", keylistener);
