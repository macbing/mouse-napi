# mouse-napi

[中文](README.zh-CN.md)

Mouse tracking for macOS and Windows. Receive the screen position of mouse events, including while another application is in the foreground. Events are observed only and are not consumed.

The native addon uses [Node-API](https://nodejs.org/api/n-api.html) (`node-addon-api`) and requires **Node.js 14** or later. `npm install` compiles it for the system and Node.js version you are running.

	npm install mouse-napi

macOS requires **10.15** or later and the Xcode command line tools. Windows requires a C++ build environment that can compile Node.js native addons (the Visual Studio Desktop development with C++ workload).

# Usage

The module returns an event emitter.

```javascript
var mouse = require('mouse-napi')()

mouse.on('move', function (x, y) {
  console.log(x, y)
})
```

The program will not terminate as long as a mouse listener is active. To allow the program to exit, either call `mouse.unref()` (works as `unref` / `ref` on a TCP server) or `mouse.destroy()`.

The events emitted are: `move`, `left-down`, `left-up`, `left-drag`, `right-down`, `right-up`, and `right-drag`. For each event the screen coordinates are passed to the handler function.

# Platform notes

Both systems emit the same event names. The native code is compiled separately:

- **macOS** uses a listen-only `CGEventTap`. The system reports `left-drag` and `right-drag` directly.
- **Windows** uses the `WH_MOUSE_LL` low-level mouse hook. Movement while a button is held is reported from native code as `left-drag` or `right-drag`. If both buttons are down, the event is `left-drag`.

From macOS Mojave onward, mouse events are delivered only after the process is allowed under **Accessibility**. Input Monitoring is not required.

When running from Terminal:

1. Open `System Settings > Privacy & Security > Accessibility`
2. Add *Terminal* (or the app that launches Node) to the list and turn it on

On macOS Catalina and earlier, the same list is under `System Preferences > Security & Privacy > Privacy > Accessibility`. Restart the process after changing the permission.

# Credits

- Windows implementation based on [win-mouse](https://github.com/kapetan/win-mouse)
- macOS implementation based on [osx-mouse-napi](https://github.com/macbing/osx-mouse-napi)
