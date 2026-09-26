#include "mouse.h"

#include <memory>

const char* LEFT_DOWN = "left-down";
const char* LEFT_UP = "left-up";
const char* RIGHT_DOWN = "right-down";
const char* RIGHT_UP = "right-up";
const char* MOVE = "move";

bool IsMouseEvent(WPARAM type) {
	return type == WM_LBUTTONDOWN ||
		type == WM_LBUTTONUP ||
		type == WM_RBUTTONDOWN ||
		type == WM_RBUTTONUP ||
		type == WM_MOUSEMOVE;
}

void OnMouseEvent(WPARAM type, POINT point, void* data) {
	Mouse* mouse = static_cast<Mouse*>(data);
	mouse->HandleEvent(type, point);
}

Napi::FunctionReference Mouse::constructor;

const char* Mouse::EventName(WPARAM type) {
	switch (type) {
		case WM_LBUTTONDOWN: return LEFT_DOWN;
		case WM_LBUTTONUP: return LEFT_UP;
		case WM_RBUTTONDOWN: return RIGHT_DOWN;
		case WM_RBUTTONUP: return RIGHT_UP;
		case WM_MOUSEMOVE: return MOVE;
		default: return nullptr;
	}
}

Napi::Object Mouse::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "Mouse", {
		InstanceMethod("destroy", &Mouse::Destroy),
		InstanceMethod("ref", &Mouse::AddRef),
		InstanceMethod("unref", &Mouse::RemoveRef)
	});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Mouse", func);
	return exports;
}

Mouse::Mouse(const Napi::CallbackInfo& info)
	: Napi::ObjectWrap<Mouse>(info),
	  hook_ref(nullptr),
	  state(nullptr) {
	Napi::Env env = info.Env();

	if (info.Length() < 1 || !info[0].IsFunction()) {
		throw Napi::TypeError::New(env, "Expected a function");
	}

	state = new State();

	try {
		tsfn = Napi::ThreadSafeFunction::New(
			env,
			info[0].As<Napi::Function>(),
			"mouse-napi:Mouse",
			static_cast<size_t>(MAX_QUEUE_SIZE),
			static_cast<size_t>(1),
			[](Napi::Env, State* state) { delete state; },
			state);
	} catch (...) {
		delete state;
		state = nullptr;
		throw;
	}

	hook_ref = MouseHookRegister(OnMouseEvent, this);
}

Mouse::~Mouse() {
	Stop();
}

void Mouse::Finalize(Napi::Env /*env*/) {
	Stop();
}

void Mouse::Stop() {
	if (state == nullptr) return;
	if (state->stopped.exchange(true)) return;

	MouseHookRef ref = hook_ref;
	hook_ref = nullptr;
	if (ref != nullptr) MouseHookUnregister(ref);

	// Callbacks already queued keep their own State*. Release() may run the
	// finalizer, which deletes State, so this object must not touch it after.
	state = nullptr;
	tsfn.Release();
}

void Mouse::HandleEvent(WPARAM type, POINT point) {
	if (!IsMouseEvent(type) || state == nullptr || state->stopped.load()) return;

	MouseEvent* event = new MouseEvent();
	event->x = point.x;
	event->y = point.y;
	event->type = type;

	State* keep = state;
	napi_status status = tsfn.NonBlockingCall(
		event,
		[keep](Napi::Env env, Napi::Function callback, MouseEvent* event) {
			std::unique_ptr<MouseEvent> owned(event);

			if (env == nullptr || callback == nullptr || keep->stopped.load()) return;

			const char* name = EventName(owned->type);
			if (name == nullptr) return;

			callback.Call({
				Napi::String::New(env, name),
				Napi::Number::New(env, static_cast<double>(owned->x)),
				Napi::Number::New(env, static_cast<double>(owned->y))
			});
		});

	if (status != napi_ok) delete event;
}

Napi::Value Mouse::Destroy(const Napi::CallbackInfo& info) {
	Stop();
	return info.Env().Undefined();
}

Napi::Value Mouse::AddRef(const Napi::CallbackInfo& info) {
	if (state != nullptr && !state->stopped.load()) {
		tsfn.Ref(info.Env());
	}

	return info.Env().Undefined();
}

Napi::Value Mouse::RemoveRef(const Napi::CallbackInfo& info) {
	if (state != nullptr && !state->stopped.load()) {
		tsfn.Unref(info.Env());
	}

	return info.Env().Undefined();
}
