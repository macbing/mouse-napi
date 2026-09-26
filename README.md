# mouse-napi

macOS 与 Windows 的鼠标事件监听。可以拿到各类鼠标事件的屏幕坐标，前台是其他应用时也会继续收到事件。事件只监听，不会被吞掉。

原生插件使用 [Node-API](https://nodejs.org/api/n-api.html)（`node-addon-api`），需要 **Node.js 14** 或更高版本。`npm install` 会按当前系统和 Node 版本编译。

	npm install mouse-napi

macOS 需要 **10.15** 或更高版本，以及 Xcode 命令行工具。Windows 需要能编译 Node 原生插件的构建环境（Visual Studio 的 C++ 桌面开发工具）。

# 用法

模块返回一个事件发射器。

```javascript
var mouse = require('mouse-napi')()

mouse.on('move', function (x, y) {
  console.log(x, y)
})
```

只要监听还在，进程就不会退出。想让进程正常结束，调用 `mouse.unref()`（作用和 TCP 服务器上的 `unref` / `ref` 一样），或者调用 `mouse.destroy()`。

事件有：`move`、`left-down`、`left-up`、`left-drag`、`right-down`、`right-up`、`right-drag`。每个事件都会把屏幕坐标传给处理函数。

# 平台差异

两个系统对外的事件名相同，底层实现分开编译：

- **macOS** 使用 `CGEventTap`（只监听）。系统直接上报 `left-drag` 和 `right-drag`。
- **Windows** 使用低级鼠标钩子 `WH_MOUSE_LL`。按键按住期间的移动在原生层记成 `left-drag` 或 `right-drag`。左右键同时按下时优先记成 `left-drag`。

从 macOS Mojave 起，进程需要出现在 **辅助功能**（Accessibility）名单里，鼠标事件才会送达。不需要「输入监听」。

在终端里运行时：

1. 打开 `系统设置 > 隐私与安全性 > 辅助功能`
2. 把 *终端*（或实际启动 Node 的应用）加进列表并打开

改完权限后需要重新启动进程。

# 来源

- Windows 实现来自 [win-mouse](https://github.com/kapetan/win-mouse)
- macOS 实现来自 [osx-mouse-napi](https://github.com/macbing/osx-mouse-napi)
