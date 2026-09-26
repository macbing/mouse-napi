var events = require('events')
var bindings = require('bindings')

var platform = process.platform

if (platform !== 'darwin' && platform !== 'win32') {
  throw new Error('mouse-napi only supports macOS and Windows')
}

var Mouse = bindings('addon').Mouse

module.exports = function () {
  var that = new events.EventEmitter()
  var mouse = null
  var left = false
  var right = false

  that.once('newListener', function () {
    mouse = new Mouse(function (type, x, y) {
      // Windows 低级钩子只上报 move。按键按住时的移动在这里改成 drag。
      // macOS 的 CGEvent 已经区分 left-drag / right-drag。
      if (platform === 'win32') {
        if (type === 'left-down') left = true
        else if (type === 'left-up') left = false
        else if (type === 'right-down') right = true
        else if (type === 'right-up') right = false

        if (type === 'move' && left) type = 'left-drag'
        else if (type === 'move' && right) type = 'right-drag'
      }

      that.emit(type, x, y)
    })
  })

  that.ref = function () {
    if (mouse) mouse.ref()
  }

  that.unref = function () {
    if (mouse) mouse.unref()
  }

  that.destroy = function () {
    if (mouse) mouse.destroy()
    mouse = null
  }

  return that
}
