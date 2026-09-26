#ifndef _MOUSE_H
#define _MOUSE_H

#include <atomic>
#include <napi.h>

#include "mouse_hook.h"

struct MouseEvent {
	LONG x;
	LONG y;
	const char* name;
};

const unsigned int MAX_QUEUE_SIZE = 10;

class Mouse : public Napi::ObjectWrap<Mouse> {
	public:
		static Napi::Object Init(Napi::Env env, Napi::Object exports);
		explicit Mouse(const Napi::CallbackInfo& info);
		~Mouse();

		void Stop();
		void HandleEvent(WPARAM type, POINT point);
		void Finalize(Napi::Env env);

	private:
		// Outlives this object: queued callbacks may run after Stop(), and the
		// thread-safe function finalizer deletes it once those callbacks finish.
		struct State {
			std::atomic<bool> stopped;
			bool left;
			bool right;
			State() : stopped(false), left(false), right(false) {}
		};

		static Napi::FunctionReference constructor;
		static const char* EventName(WPARAM type);

		Napi::Value Destroy(const Napi::CallbackInfo& info);
		Napi::Value AddRef(const Napi::CallbackInfo& info);
		Napi::Value RemoveRef(const Napi::CallbackInfo& info);

		MouseHookRef hook_ref;
		Napi::ThreadSafeFunction tsfn;
		State* state;
};

#endif
