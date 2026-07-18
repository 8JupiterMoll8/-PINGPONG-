import {
  collectHitEvents,
  collectGameEvents,
  createDemoFrame,
  parseTelemetryLine,
} from './model.mjs'

const $ = (id) => document.getElementById(id)
const clamp = (value, minimum, maximum) => Math.min(maximum, Math.max(minimum, Number(value) || 0))
const format = (value, digits = 1) => (Number(value) || 0).toFixed(digits)

const phaseNames = ['Sensor test', 'Serve', 'Rally', 'Fault', 'Technical pause']
const expectedNames = ['—', 'Left racket', 'Left table', 'Right table', 'Right racket']
const levelNames = ['Distance', 'Recognition', 'Listening', 'Dialogue', 'Reunion', 'Freedom']
const sourceNames = {
  leftRacket: 'Left racket',
  rightRacket: 'Right racket',
  leftTable: 'Left table',
  rightTable: 'Right table',
  netTable: 'Net',
  fault: '⚡ Fault',
  rally: '✓ Rally',
}

let activeMode = 'idle'
let serialPort = null
let serialReader = null
let serialReadTask = null
let serialBuffer = ''
let demoTimer = null
let previousFrame = null
let lastFrameReceivedAt = 0
let eventHistory = []
let chartHistory = []
let packetRateFrame = null
let packetRates = { left: 0, right: 0 }

function setConnection(mode, title, detail) {
  activeMode = mode
  $('connection-label').textContent = title
  $('frame-age').textContent = detail
  $('connection-dot').className = `status-dot ${mode === 'live' ? 'live' : mode === 'demo' ? 'demo' : mode === 'error' ? 'error' : ''}`
  $('mode-badge').textContent = mode === 'live'
    ? 'LIVE / READ-ONLY'
    : mode === 'demo'
      ? 'DEMO / SIMULATED'
      : 'READ-ONLY'
  $('connect-btn').disabled = mode === 'live' || mode === 'demo'
  $('demo-btn').disabled = mode === 'demo' || mode === 'live'
  $('disconnect-btn').disabled = mode === 'idle'
}

function resetSession() {
  previousFrame = null
  lastFrameReceivedAt = 0
  chartHistory = []
  eventHistory = []
  packetRateFrame = null
  packetRates = { left: 0, right: 0 }
  renderEvents()
}

async function connectSerial() {
  if (!('serial' in navigator)) {
    setConnection('error', 'Web Serial unavailable', 'Use desktop Chrome or Edge on localhost')
    return
  }

  await stopActiveConnection()

  try {
    serialPort = await navigator.serial.requestPort()
    await serialPort.open({ baudRate: 115200, bufferSize: 8192 })
    serialBuffer = ''
    resetSession()
    setConnection('live', 'Master connected', 'Waiting for PINGPONG telemetry')
    serialReadTask = readSerialLoop()
  } catch (error) {
    serialPort = null
    if (error.name === 'NotFoundError') {
      setConnection('idle', 'Not connected', 'Connection chooser was cancelled')
    } else {
      setConnection('error', 'Connection failed', error.message)
    }
  }
}

async function readSerialLoop() {
  try {
    while (serialPort?.readable && activeMode === 'live') {
      serialReader = serialPort.readable.getReader()
      try {
        while (activeMode === 'live') {
          const { value, done } = await serialReader.read()
          if (done) break
          serialBuffer += new TextDecoder().decode(value, { stream: true })
          processSerialBuffer()
        }
      } finally {
        serialReader.releaseLock()
        serialReader = null
      }
    }
  } catch (error) {
    if (activeMode === 'live') {
      setConnection('error', 'Serial connection lost', error.message)
    }
  }
}

function processSerialBuffer() {
  const lines = serialBuffer.split(/\r?\n/)
  serialBuffer = lines.pop() ?? ''
  if (serialBuffer.length > 8192) serialBuffer = serialBuffer.slice(-8192)

  for (const line of lines) {
    const frame = parseTelemetryLine(line)
    if (!frame) continue
    if (frame.v !== 1) {
      setConnection('error', `Unsupported telemetry v${frame.v}`, 'Update the dashboard and firmware together')
      continue
    }
    renderFrame(frame)
  }
}

async function startDemo() {
  await stopActiveConnection()
  resetSession()
  setConnection('demo', 'Demo running', 'Simulated sensors—no hardware connected')
  const startedAt = performance.now()
  demoTimer = window.setInterval(() => {
    const elapsed = (performance.now() - startedAt) * 1.7
    renderFrame(createDemoFrame(elapsed))
  }, 50)
}

async function stopActiveConnection() {
  activeMode = 'stopping'

  if (demoTimer !== null) {
    clearInterval(demoTimer)
    demoTimer = null
  }

  const reader = serialReader
  if (reader) await reader.cancel().catch(() => {})
  if (serialReadTask) await serialReadTask.catch(() => {})
  serialReadTask = null

  const port = serialPort
  serialPort = null
  if (port?.readable || port?.writable) await port.close().catch(() => {})

  setConnection('idle', 'Not connected', 'Waiting for a frame')
}

function renderFrame(frame) {
  const receivedAt = Date.now()
  const hitEvents  = collectHitEvents(previousFrame, frame)
  const gameEvents = collectGameEvents(previousFrame, frame)
  const events     = [...hitEvents, ...gameEvents]

  const rates = updatePacketRates(frame)

  lastFrameReceivedAt = receivedAt
  $('sequence-value').textContent = `#${frame.seq}`
  $('protocol-value').textContent = `Telemetry protocol v${frame.v}`
  $('frame-age').textContent = activeMode === 'demo' ? 'Simulated at 20 frames/s' : 'Live frame received now'

  renderRadio('left', frame.radio.leftFresh, frame.radio.leftAge, rates.left)
  renderRadio('right', frame.radio.rightFresh, frame.radio.rightAge, rates.right)
  renderRacket('left', frame.left)
  renderRacket('right', frame.right)
  renderCourt(frame, hitEvents)   // court only flashes on physical hits
  renderGame(frame.game)
  renderOutputs(frame, hitEvents)

  chartHistory.push({ left: frame.left.speed, right: frame.right.speed })
  if (chartHistory.length > 120) chartHistory.shift()
  drawChart()

  for (const event of events) addEvent(event)
  for (const event of hitEvents) onLedHitEvent(event)
  previousFrame = frame
}

function updatePacketRates(current) {
  if (!packetRateFrame || current.ms < packetRateFrame.ms) {
    packetRateFrame = current
    packetRates = { left: 0, right: 0 }
    return packetRates
  }

  const elapsed = current.ms - packetRateFrame.ms
  if (elapsed < 500) return packetRates

  const delta = (now, before) => Number(now) >= Number(before)
    ? Number(now) - Number(before)
    : Number(now)
  packetRates = {
    left: delta(current.radio.leftPackets, packetRateFrame.radio.leftPackets) * 1000 / elapsed,
    right: delta(current.radio.rightPackets, packetRateFrame.radio.rightPackets) * 1000 / elapsed,
  }
  packetRateFrame = current
  return packetRates
}

function renderRadio(side, fresh, age, packetsPerSecond) {
  const healthy = Boolean(Number(fresh))
  const ageLabel = Number(age) >= 0xffffffff ? '—' : `${Math.round(age)} ms`

  for (const id of [`${side}-radio`, `${side}-radio-pill`]) {
    const element = $(id)
    element.classList.toggle('healthy', healthy)
    element.classList.toggle('unhealthy', !healthy)
    element.textContent = `${side === 'left' ? 'L' : 'R'} racket ${healthy ? 'live' : 'stale'}`
  }

  $(`${side}-pps`).textContent = Math.round(packetsPerSecond)
  $(`${side}-age`).textContent = ageLabel
}

function renderRacket(side, racket) {
  for (const axis of ['gx', 'gy', 'gz', 'ax', 'ay', 'az']) {
    $(`${side}-${axis}`).textContent = format(racket[axis], 2)
  }

  $(`${side}-roll`).textContent = `${format(racket.roll)}°`
  $(`${side}-pitch`).textContent = `${format(racket.pitch)}°`
  $(`${side}-yaw`).textContent = `${format(racket.yaw)}°`
  $(`${side}-speed`).textContent = format(racket.speed)
  $(`${side}-pressure`).textContent = Math.round(racket.pressure)
  $(`${side}-piezo`).textContent = Math.round(racket.piezo)
  $(`${side}-peak`).textContent = Math.round(racket.peak)

  $(`${side}-pressure-bar`).style.width = `${clamp(racket.pressure / 10.23, 0, 100)}%`
  $(`${side}-speed-bar`).style.width = `${clamp(racket.speed / 6, 0, 100)}%`

  const visual = $(`${side}-racket-visual`)
  visual.style.setProperty('--roll', `${clamp(racket.roll, -180, 180)}deg`)
  visual.style.setProperty('--pitch', `${clamp(racket.pitch, -75, 75)}deg`)
  visual.style.setProperty('--yaw', `${clamp(normalizeAngle(racket.yaw) * 0.35, -65, 65)}deg`)

  const player = $(`${side}-player`)
  player.style.setProperty('--roll', `${clamp(racket.roll, -70, 70)}deg`)
  player.style.setProperty('--lift', `${-clamp(racket.speed / 35, 0, 12)}px`)
}

function renderCourt(frame, events) {
  $('left-racket-hits').textContent = frame.left.hit
  $('right-racket-hits').textContent = frame.right.hit
  $('left-table-hits').textContent = frame.table.leftHit
  $('right-table-hits').textContent = frame.table.rightHit
  $('net-table-hits').textContent = frame.table.netHit
  $('net-table-peak').textContent = frame.table.netPeak > 0 ? `peak ${frame.table.netPeak}` : 'peak —'

  const core = $('relationship-core')
  core.style.setProperty('--orbit-duration', `${0.18 + clamp(frame.game.level, 0, 5) * 0.7}s`)
  core.style.boxShadow = `0 0 ${18 + frame.game.level * 7}px rgba(233, 185, 110, ${0.08 + frame.game.level * 0.025})`

  // Highlight the sensor the game engine expects next (pulse glow)
  const expectedSources = [null, 'leftRacket', 'leftTable', 'rightTable', 'rightRacket']
  document.querySelectorAll('.hit-pad.expected').forEach(el => el.classList.remove('expected'))
  const expectedSource = expectedSources[frame.game.expected]
  if (expectedSource) {
    const el = document.querySelector(`[data-source="${expectedSource}"]`)
    if (el) el.classList.add('expected')
  }

  const positions = {
    0: [50, 50],
    1: [10, 78],
    2: [28, 24],
    3: [72, 76],
    4: [90, 22],
  }
  const [left, top] = positions[frame.game.expected] ?? positions[0]
  $('ball').style.left = `${left}%`
  $('ball').style.top = `${top}%`
  $('ball').style.opacity = frame.game.phase === 0 ? '0.25' : '1'

  for (const event of events) flashSource(event.source)
}

function renderGame(game) {
  const level = clamp(Math.round(game.level), 0, 5)
  const progress = clamp(game.progress, 0, 100)
  const phase = phaseNames[game.phase] ?? `Unknown (${game.phase})`

  $('phase-value').textContent = phase
  $('expected-value').textContent = expectedNames[game.expected] ?? `Input ${game.expected}`
  $('rally-value').textContent = game.rally
  $('level-value').textContent = level
  $('level-copy').textContent = levelNames[level]
  $('level-fill').style.width = `${(level / 5) * 100}%`
  $('progress-value').textContent = `${Math.round(progress)}%`
  $('progress-fill').style.width = `${progress}%`

  // Flash engine badge red on fault
  const active = game.phase !== 0
  const isFault = game.phase === 3
  $('engine-badge').classList.toggle('pending', !active && !isFault)
  $('engine-badge').classList.toggle('active', active && !isFault)
  $('engine-badge').classList.toggle('fault', isFault)
  $('engine-badge').textContent = activeMode === 'demo' ? 'SIMULATED'
    : isFault ? 'FAULT'
    : active  ? 'ACTIVE'
    : 'PENDING'
  $('engine-note').textContent = activeMode === 'demo'
    ? 'Demo mode imagines the future referee and relationship engine.'
    : isFault
      ? 'Fault detected — sequence broken. Resetting rally in 600 ms.'
      : active
        ? 'GameEngine is authoritative. The dashboard is observing its decisions.'
        : 'The new referee is not active yet. Live sensor truth is shown below.'
}

function renderOutputs(frame, events) {
  const motor = clamp(frame.outputs.motorTarget, 0, 9000)
  const angle = -130 + (motor / 9000) * 260
  $('motor-value').textContent = Math.round(motor)
  $('motor-dial').style.setProperty('--motor-angle', `${angle}deg`)
  $('sound-value').textContent = frame.outputs.soundMode > 0 ? `Level ${frame.outputs.soundMode} simulation` : 'Legacy MIDI'

  if (events.some((event) => event.source === 'rightRacket')) {
    const orb = $('light-orb')
    orb.classList.add('pulse')
    $('light-value').textContent = 'Pulse'
    window.setTimeout(() => {
      orb.classList.remove('pulse')
      $('light-value').textContent = 'Ready'
    }, 120)
  } else if ($('light-value').textContent === 'Waiting') {
    $('light-value').textContent = 'Ready'
  }
}

function normalizeAngle(value) {
  return ((Number(value) + 180) % 360 + 360) % 360 - 180
}

function flashSource(source) {
  const element = document.querySelector(`[data-source="${source}"]`)
  if (!element) return
  element.classList.remove('flash')
  requestAnimationFrame(() => element.classList.add('flash'))
  window.setTimeout(() => element.classList.remove('flash'), 180)
}

function addEvent(event) {
  eventHistory.unshift(event)
  if (eventHistory.length > 40) eventHistory.length = 40
  renderEvents()
}

function renderEvents() {
  const list = $('event-list')
  list.replaceChildren()

  if (eventHistory.length === 0) {
    const empty = document.createElement('li')
    empty.className = 'event-empty'
    empty.textContent = 'Hits will appear here.'
    list.append(empty)
    return
  }

  for (const event of eventHistory) {
    const item = document.createElement('li')
    item.className =
      event.source === 'fault'              ? 'fault-event'
      : event.source === 'rally'            ? 'rally-event'
      : event.source.startsWith('left')     ? 'left-event'
      : event.source.startsWith('right')    ? 'right-event'
      : 'net-event'
    const label = document.createElement('span')
    label.textContent = `${sourceNames[event.source] ?? event.source}${event.count > 1 ? ` × ${event.count}` : ''}`
    const time = document.createElement('time')
    time.textContent = `${(event.at / 1000).toFixed(2)} s`
    item.append(label, time)
    list.append(item)
  }
}

function drawChart() {
  const canvas = $('signal-chart')
  const bounds = canvas.getBoundingClientRect()
  if (bounds.width === 0 || bounds.height === 0) return
  const ratio = window.devicePixelRatio || 1
  const width = Math.round(bounds.width * ratio)
  const height = Math.round(bounds.height * ratio)
  if (canvas.width !== width || canvas.height !== height) {
    canvas.width = width
    canvas.height = height
  }

  const context = canvas.getContext('2d')
  context.setTransform(ratio, 0, 0, ratio, 0, 0)
  context.clearRect(0, 0, bounds.width, bounds.height)

  context.strokeStyle = 'rgba(255,255,255,0.065)'
  context.lineWidth = 1
  for (let row = 1; row < 4; row += 1) {
    const y = bounds.height * row / 4
    context.beginPath()
    context.moveTo(0, y)
    context.lineTo(bounds.width, y)
    context.stroke()
  }

  const maxValue = Math.max(600, ...chartHistory.flatMap((point) => [point.left, point.right]))
  drawSeries(context, bounds, maxValue, 'left', '#ef5a71')
  drawSeries(context, bounds, maxValue, 'right', '#54d6c6')
}

function drawSeries(context, bounds, maxValue, key, color) {
  if (chartHistory.length < 2) return
  context.beginPath()
  context.strokeStyle = color
  context.lineWidth = 1.7
  context.shadowColor = color
  context.shadowBlur = 7

  chartHistory.forEach((point, index) => {
    const x = index / 119 * bounds.width
    const y = bounds.height - clamp(point[key] / maxValue, 0, 1) * (bounds.height - 8) - 4
    if (index === 0) context.moveTo(x, y)
    else context.lineTo(x, y)
  })
  context.stroke()
  context.shadowBlur = 0
}

$('connect-btn').addEventListener('click', connectSerial)
$('demo-btn').addEventListener('click', startDemo)
$('disconnect-btn').addEventListener('click', stopActiveConnection)
$('clear-events-btn').addEventListener('click', () => {
  eventHistory = []
  renderEvents()
})
window.addEventListener('resize', drawChart)

if ('serial' in navigator) {
  navigator.serial.addEventListener('disconnect', () => {
    if (activeMode === 'live') setConnection('error', 'Master disconnected', 'Reconnect the USB cable and choose Connect Master')
  })
} else {
  $('connect-btn').title = 'Web Serial requires desktop Chrome or Edge on localhost'
}

window.setInterval(() => {
  if (activeMode !== 'live' || !lastFrameReceivedAt) return
  const age = Date.now() - lastFrameReceivedAt
  $('frame-age').textContent = age < 500 ? `${age} ms since last frame` : `Telemetry stale — ${age} ms`
  $('connection-dot').classList.toggle('error', age >= 500)
}, 250)

setConnection('idle', 'Not connected', 'Connect TeensyMaster or explore the demo')
drawChart()

// ─── LED Strip Simulator ──────────────────────────────────────────────────────
// Mirrors CometRaw.h logic (58 LEDs, 10 ms update cycle, speed=1.25, accel=0.001, size=7)
// Driven entirely by telemetry hit events — no firmware changes needed.

const LED_COUNT = 58
// Each LED: Float32Array [r, g, b] for smooth fade arithmetic
const ledState = Array.from({ length: LED_COUNT }, () => new Float32Array(3))

class CometSim {
  constructor(startPos, direction, color) {
    this.startPos = startPos   // 0 (right) or 57 (left)
    this.direction = direction // +1 or -1
    this.color = color         // [r, g, b] 0-255
    this.pos = startPos
    this.speed = 1.25
    this.active = false
    this.lastMs = 0
  }

  start() {
    this.pos = this.startPos
    this.speed = 1.25
    this.active = true
    this.lastMs = performance.now()
  }

  tick(now) {
    if (!this.active) return
    if (now - this.lastMs < 10) return  // 10 ms tick — same as Teensy

    this.lastMs = now
    this.speed += 0.001               // acceleration
    this.pos += this.direction * this.speed

    // Draw head (size = 7), clamped to strip bounds
    for (let i = 0; i < 7; i++) {
      const idx = Math.round(this.pos) + i * this.direction
      if (idx >= 0 && idx < LED_COUNT) {
        ledState[idx][0] = Math.min(255, ledState[idx][0] + this.color[0])
        ledState[idx][1] = Math.min(255, ledState[idx][1] + this.color[1])
        ledState[idx][2] = Math.min(255, ledState[idx][2] + this.color[2])
      }
    }

    // Random fade tail (fadeAmt = 200 / 255 ≈ 78 % fade per tick, 50 % chance)
    for (let j = 0; j < LED_COUNT; j++) {
      if (Math.random() > 0.5) {
        ledState[j][0] *= 0.22
        ledState[j][1] *= 0.22
        ledState[j][2] *= 0.22
      }
    }

    // Hit boundary → stop and clear
    if (this.pos > LED_COUNT - 7 || this.pos < 0) {
      this.active = false
      for (let i = 0; i < LED_COUNT; i++) ledState[i].fill(0)
    }
  }
}

// Left-racket comet: starts at LED 57, moves toward 0 (matches lr_cometRaw in main.cpp)
const leftComet  = new CometSim(LED_COUNT - 1, -1, [239, 90, 113])
// Right-racket comet: starts at LED 0, moves toward 57
const rightComet = new CometSim(0, 1, [84, 214, 198])

function flashNetLeds() {
  for (let i = 0; i < LED_COUNT; i++) {
    ledState[i][0] = 200
    ledState[i][1] = 200
    ledState[i][2] = 255
  }
  window.setTimeout(() => {
    for (let i = 0; i < LED_COUNT; i++) ledState[i].fill(0)
  }, 160)
}

// Called from renderFrame for every hit event
function onLedHitEvent(event) {
  if (event.source === 'leftRacket')  leftComet.start()
  else if (event.source === 'rightRacket') rightComet.start()
  else if (event.source === 'netTable') flashNetLeds()
}

function drawLedStrip() {
  const canvas = $('led-strip-canvas')
  if (!canvas) return
  const bounds = canvas.getBoundingClientRect()
  if (bounds.width === 0 || bounds.height === 0) return

  const ratio = window.devicePixelRatio || 1
  const pw = Math.round(bounds.width * ratio)
  const ph = Math.round(bounds.height * ratio)
  if (canvas.width !== pw || canvas.height !== ph) {
    canvas.width = pw
    canvas.height = ph
  }

  const ctx = canvas.getContext('2d')
  ctx.setTransform(ratio, 0, 0, ratio, 0, 0)
  ctx.clearRect(0, 0, bounds.width, bounds.height)

  const padX = 10
  const step = (bounds.width - padX * 2) / LED_COUNT
  const dotR = Math.max(2.5, step * 0.42)
  const cy = bounds.height / 2

  for (let i = 0; i < LED_COUNT; i++) {
    const r = Math.round(ledState[i][0])
    const g = Math.round(ledState[i][1])
    const b = Math.round(ledState[i][2])
    const brightness = (r + g + b) / (255 * 3)
    const cx = padX + (i + 0.5) * step
    const color = `rgb(${r},${g},${b})`

    // Glow when lit
    ctx.shadowColor = color
    ctx.shadowBlur = brightness > 0.02 ? 10 + brightness * 22 : 0

    ctx.beginPath()
    ctx.arc(cx, cy, dotR, 0, Math.PI * 2)
    ctx.fillStyle = brightness > 0.02 ? color : '#10151c'
    ctx.fill()
  }
  ctx.shadowBlur = 0
}

// Animation loop — independent of telemetry frame rate
;(function ledLoop(now) {
  leftComet.tick(now)
  rightComet.tick(now)
  drawLedStrip()
  requestAnimationFrame(ledLoop)
})()

