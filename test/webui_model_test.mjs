import assert from 'node:assert/strict'

import {
  collectHitEvents,
  counterDelta,
  createDemoFrame,
  parseTelemetryLine,
} from '../webui/model.mjs'

assert.equal(parseTelemetryLine('Hallo Ping Pong'), null)
assert.equal(parseTelemetryLine('PP:not-json'), null)

const firstFrame = parseTelemetryLine(
  'PP:{"v":1,"ms":100,"seq":7,"left":{"hit":255},"right":{"hit":2},"table":{"leftHit":9,"rightHit":4}}',
)

assert.equal(firstFrame.v, 1)
assert.equal(firstFrame.seq, 7)
assert.equal(firstFrame.left.hit, 255)
assert.deepEqual(collectHitEvents(null, firstFrame), [])

assert.equal(counterDelta(0, 255), 1)
assert.equal(counterDelta(3, 255), 4)
assert.equal(counterDelta(12, 12), 0)

const secondFrame = structuredClone(firstFrame)
secondFrame.ms = 150
secondFrame.left.hit = 0
secondFrame.table.rightHit = 6

assert.deepEqual(collectHitEvents(firstFrame, secondFrame), [
  { source: 'leftRacket', count: 1, at: 150 },
  { source: 'rightTable', count: 2, at: 150 },
])

const restartedFrame = structuredClone(secondFrame)
restartedFrame.ms = 4
restartedFrame.left.hit = 0
assert.deepEqual(collectHitEvents(secondFrame, restartedFrame), [])

const demo = createDemoFrame(1_000)
assert.equal(demo.v, 1)
assert.equal(demo.game.phase, 0)
assert.ok(Number.isFinite(demo.left.roll))
assert.ok(Number.isFinite(demo.outputs.motorTarget))

console.log('webui model tests passed')
