var assert = require('assert')
var mouse = require('../')

var tracker = mouse()

tracker.on('move', function () {})
tracker.ref()
tracker.unref()
tracker.destroy()
tracker.destroy()

assert.strictEqual(typeof tracker.ref, 'function')
assert.strictEqual(typeof tracker.unref, 'function')
assert.strictEqual(typeof tracker.destroy, 'function')

console.log('smoke ok')
