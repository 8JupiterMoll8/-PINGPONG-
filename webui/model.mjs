const number = (value, fallback = 0) => Number.isFinite(Number(value)) ? Number(value) : fallback

export function createEmptyFrame() {
  return {
    v: 1,
    ms: 0,
    seq: 0,
    status: 0,
    radio: {
      leftPackets: 0,
      rightPackets: 0,
      leftAge: 0xffffffff,
      rightAge: 0xffffffff,
      leftFresh: 0,
      rightFresh: 0,
    },
    left: emptyRacket(),
    right: emptyRacket(),
    table: { leftHit: 0, rightHit: 0, netHit: 0, leftPeak: 0, rightPeak: 0, netPeak: 0 },
    game: { phase: 0, expected: 0, rally: 0, level: 0, progress: 0 },
    outputs: { motorTarget: 0, lightPulse: 0, ledMode: 0, soundMode: 0 },
  }
}

function emptyRacket() {
  return {
    gx: 0,
    gy: 0,
    gz: 0,
    ax: 0,
    ay: 0,
    az: 0,
    piezo: 0,
    pressure: 0,
    roll: 0,
    pitch: 0,
    yaw: 0,
    speed: 0,
    hit: 0,
    peak: 0,
  }
}

function normalizeFrame(raw) {
  const frame = createEmptyFrame()
  const left = { ...frame.left, ...(raw.left ?? {}) }
  const right = { ...frame.right, ...(raw.right ?? {}) }

  return {
    ...frame,
    ...raw,
    v: number(raw.v, 0),
    ms: number(raw.ms),
    seq: number(raw.seq),
    status: number(raw.status),
    radio: { ...frame.radio, ...(raw.radio ?? {}) },
    left,
    right,
    table: { ...frame.table, ...(raw.table ?? {}) },
    game: { ...frame.game, ...(raw.game ?? {}) },
    outputs: { ...frame.outputs, ...(raw.outputs ?? {}) },
  }
}

export function parseTelemetryLine(line) {
  const trimmed = line.trim()
  if (!trimmed.startsWith('PP:')) return null

  try {
    const parsed = JSON.parse(trimmed.slice(3))
    return parsed && typeof parsed === 'object' ? normalizeFrame(parsed) : null
  } catch {
    return null
  }
}

export function counterDelta(current, previous) {
  return (number(current) - number(previous) + 256) % 256
}

export function collectHitEvents(previous, current) {
  if (!previous || !current) return []
  if (current.ms < previous.ms) return []

  const sources = [
    ['leftRacket', current.left.hit, previous.left.hit],
    ['rightRacket', current.right.hit, previous.right.hit],
    ['leftTable', current.table.leftHit, previous.table.leftHit],
    ['rightTable', current.table.rightHit, previous.table.rightHit],
    ['netTable', current.table.netHit, previous.table.netHit],
  ]

  return sources.flatMap(([source, now, before]) => {
    const count = counterDelta(now, before)
    return count ? [{ source, count, at: current.ms }] : []
  })
}

/**
 * Detect game-level events from two consecutive frames:
 *   'fault' — phase just became 3 (DOPPEL_FEHLER)
 *   'rally' — rallyCount incremented (successful exchange)
 */
export function collectGameEvents(previous, current) {
  if (!previous || !current) return []
  if (current.ms < previous.ms) return []

  const events = []
  // Fault: phase just transitioned to 3
  if (previous.game.phase !== 3 && current.game.phase === 3) {
    events.push({ source: 'fault', count: 1, at: current.ms })
  }
  // Rally counted: wrap-safe delta (uint16 → use 65536 wrap)
  const rallyDelta = (number(current.game.rally) - number(previous.game.rally) + 65536) % 65536
  if (rallyDelta > 0 && rallyDelta < 50) {
    events.push({ source: 'rally', count: rallyDelta, at: current.ms })
  }
  return events
}

export function createDemoFrame(timestampMs) {
  const t = timestampMs / 1000
  const wave = (offset = 0) => Math.sin(t * 1.7 + offset)
  const leftHit = Math.floor(timestampMs / 1800) % 256
  const rightHit = Math.floor((timestampMs + 900) / 1800) % 256
  const leftTableHit = Math.floor((timestampMs + 520) / 1800) % 256
  const rightTableHit = Math.floor((timestampMs + 1380) / 1800) % 256
  const leftRoll = wave() * 150
  const rightRoll = wave(Math.PI) * 150
  const level = Math.min(5, Math.floor(timestampMs / 8000))
  const phase = timestampMs < 2000 ? 0 : 2

  return normalizeFrame({
    v: 1,
    ms: Math.floor(timestampMs),
    seq: Math.floor(timestampMs / 10) % 65536,
    status: 1,
    radio: {
      leftPackets: Math.floor(timestampMs * 0.42),
      rightPackets: Math.floor(timestampMs * 0.4),
      leftAge: 2,
      rightAge: 3,
      leftFresh: 1,
      rightFresh: 1,
    },
    left: {
      gx: wave() * 140,
      gy: wave(1) * 90,
      gz: wave(2) * 110,
      ax: wave(0.3),
      ay: wave(1.3),
      az: 1 + wave(2.3) * 0.15,
      piezo: leftHit % 2 ? 28 : 2,
      pressure: Math.round((wave(0.8) + 1) * 340),
      roll: leftRoll,
      pitch: wave(0.5) * 55,
      yaw: (timestampMs / 35) % 360,
      speed: 170 + Math.abs(wave()) * 260,
      hit: leftHit,
      peak: 78,
    },
    right: {
      gx: wave(Math.PI) * 145,
      gy: wave(2.2) * 100,
      gz: wave(0.5) * 105,
      ax: wave(Math.PI + 0.3),
      ay: wave(2.6),
      az: 1 + wave(1.7) * 0.18,
      piezo: rightHit % 2 ? 34 : 3,
      pressure: Math.round((wave(2.4) + 1) * 360),
      roll: rightRoll,
      pitch: wave(2.1) * 50,
      yaw: (360 - timestampMs / 38) % 360,
      speed: 160 + Math.abs(wave(Math.PI)) * 280,
      hit: rightHit,
      peak: 84,
    },
    table: {
      leftHit: leftTableHit,
      rightHit: rightTableHit,
      netHit: 0,
      leftPeak: 66,
      rightPeak: 71,
      netPeak: 0,
    },
    game: {
      phase,
      expected: phase ? (Math.floor(timestampMs / 450) % 4) + 1 : 0,
      rally: phase ? Math.floor(timestampMs / 1800) % 24 : 0,
      level,
      progress: Math.floor((timestampMs % 8000) / 80),
    },
    outputs: {
      motorTarget: Math.round(((leftRoll + 180) / 360) * 9000),
      lightPulse: rightHit,
      ledMode: level,
      soundMode: level,
    },
  })
}
