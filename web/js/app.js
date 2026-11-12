// AlgoLab front end: graph editor, renderer and step player.
// It contains no algorithm logic. It sends the graph to the C++ core (WebAssembly) as text and
// plays back the list of steps the core returns.
(function () {
  'use strict';

  const WIDTH = 900;
  const HEIGHT = 520;
  const RADIUS = 20;
  const HEURISTIC_SCALE = 50; // canvas units per unit of edge weight
  const SVGNS = 'http://www.w3.org/2000/svg';

  const $ = id => document.getElementById(id);
  const svg = $('canvas');

  const state = {
    core: null,
    catalog: [],
    algo: null,
    graph: { directed: false, nodes: [], edges: [] }, // nodes {label, x, y}, edges {u, v, w}
    source: -1,
    target: -1,
    options: {},
    sampleId: null,
    edited: false,     // the user changed the example graph
    tool: 'move',
    pendingFrom: null, // first endpoint chosen with the "Add edge" tool
    drag: null,
    pointer: null,
    trace: null,
    step: 0,
    timer: null,
  };

  // ---- Small helpers -------------------------------------------------------------------------

  const escapeHtml = text => String(text).replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
  const formatNumber = value => String(Math.round(value * 100) / 100);
  const clamp = (value, low, high) => Math.min(high, Math.max(low, value));
  const distance = (a, b) => Math.hypot(a.x - b.x, a.y - b.y);
  const lengthWeight = (a, b) => Math.max(1, Math.ceil(distance(a, b) / HEURISTIC_SCALE));

  function nextLabel() {
    const used = new Set(state.graph.nodes.map(n => n.label));
    for (let round = 0; ; round++) {
      for (let i = 0; i < 26; i++) {
        const label = String.fromCharCode(65 + i) + (round ? round : '');
        if (!used.has(label)) return label;
      }
    }
  }

  // ---- Talking to the C++ core ---------------------------------------------------------------

  function buildRequest() {
    const g = state.graph;
    const lines = [
      'algorithm ' + state.algo.id,
      'directed ' + (g.directed ? 1 : 0),
      'source ' + state.source,
      'target ' + state.target,
      'scale ' + HEURISTIC_SCALE,
    ];
    for (const [key, value] of Object.entries(state.options)) lines.push(key + ' ' + value);
    if (state.algo.usesGraph) {
      for (const n of g.nodes) lines.push(`node ${Math.round(n.x)} ${Math.round(n.y)} ${n.label}`);
      for (const e of g.edges) lines.push(`edge ${e.u} ${e.v} ${e.w}`);
    }
    return lines.join('\n') + '\n';
  }

  function runSimulation() {
    stopPlaying();
    closeWeightEditor();
    const result = JSON.parse(state.core.ccall('algolab_run', 'string', ['string'], [buildRequest()]));
    if (result.error) {
      showError(result.error);
      return;
    }
    showError('');
    state.trace = result;
    state.step = 0;
    state.pendingFrom = null;
    renderAll();
    if (result.steps.length > 1) startPlaying();
  }

  function clearTrace() {
    stopPlaying();
    state.trace = null;
    state.step = 0;
  }

  function showError(text) {
    $('error').textContent = text;
    $('error').hidden = !text;
  }

  // ---- Editing the graph ---------------------------------------------------------------------

  function loadSample(id) {
    const sample = window.ALGOLAB_SAMPLES.find(s => s.id === id);
    if (!sample) return;
    const nodes = sample.nodes.map(([label, x, y]) => ({ label, x, y }));
    state.graph = {
      directed: sample.directed,
      nodes,
      edges: sample.edges.map(([u, v, w]) => ({ u, v, w: sample.geometric ? lengthWeight(nodes[u], nodes[v]) : w })),
    };
    state.source = sample.source;
    state.target = sample.target;
    state.sampleId = id;
    state.edited = false;
    graphChanged(false);
  }

  function randomGraph() {
    const columns = 4, rows = 3, count = 9 + Math.floor(Math.random() * 3);
    const cells = Array.from({ length: columns * rows }, (_, i) => i).sort(() => Math.random() - 0.5).slice(0, count).sort((a, b) => a - b);
    const cellWidth = (WIDTH - 120) / columns, cellHeight = (HEIGHT - 100) / rows;
    const nodes = cells.map((cell, i) => ({
      label: String.fromCharCode(65 + i),
      x: Math.round(60 + (cell % columns + 0.2 + Math.random() * 0.6) * cellWidth),
      y: Math.round(50 + (Math.floor(cell / columns) + 0.2 + Math.random() * 0.6) * cellHeight),
    }));
    nodes.sort((a, b) => a.x - b.x).forEach((n, i) => { n.label = String.fromCharCode(65 + i); });

    const directed = state.graph.directed;
    const edges = [];
    const has = (u, v) => edges.some(e => (e.u === u && e.v === v) || (e.u === v && e.v === u));
    nodes.forEach((n, u) => {
      const nearest = nodes.map((m, v) => v).filter(v => v !== u).sort((a, b) => distance(n, nodes[a]) - distance(n, nodes[b]));
      for (const v of nearest.slice(0, 2)) {
        if (has(u, v)) continue;
        // Directed edges point left to right, so flows and paths have somewhere to go.
        const [from, to] = directed && nodes[u].x > nodes[v].x ? [v, u] : [u, v];
        edges.push({ u: from, v: to, w: lengthWeight(nodes[u], nodes[v]) });
      }
    });
    state.graph = { directed, nodes, edges };
    state.source = 0;
    state.target = nodes.length - 1;
    state.sampleId = null;
    state.edited = true;
    graphChanged(false);
  }

  // Call after any change to the graph: the old simulation no longer applies.
  function graphChanged(byUser = true) {
    if (byUser) {
      state.edited = true;
      state.sampleId = null;
    }
    clearTrace();
    showError('');
    fixEndpoints();
    renderAll();
  }

  function fixEndpoints() {
    const n = state.graph.nodes.length;
    const algo = state.algo;
    if (state.source >= n) state.source = -1;
    if (state.target >= n) state.target = -1;
    if (!algo) return;
    if (algo.source === 'required' && state.source < 0 && n > 0) state.source = 0;
    if (algo.target === 'required' && state.target < 0 && n > 1) state.target = n - 1;
    if (algo.target === 'required' && state.target === state.source && n > 1) state.target = state.source === n - 1 ? 0 : n - 1;
  }

  function addNode(point) {
    state.graph.nodes.push({ label: nextLabel(), x: Math.round(point.x), y: Math.round(point.y) });
    graphChanged();
  }

  function addEdge(u, v) {
    const g = state.graph;
    if (u === v) return;
    const exists = g.edges.some(e => (e.u === u && e.v === v) || (!g.directed && e.u === v && e.v === u));
    if (exists) return;
    const w = state.algo.usesWeights ? lengthWeight(g.nodes[u], g.nodes[v]) : 1;
    g.edges.push({ u, v, w });
    graphChanged();
  }

  function deleteNode(index) {
    const g = state.graph;
    g.nodes.splice(index, 1);
    g.edges = g.edges.filter(e => e.u !== index && e.v !== index)
      .map(e => ({ ...e, u: e.u > index ? e.u - 1 : e.u, v: e.v > index ? e.v - 1 : e.v }));
    const shift = value => (value === index ? -1 : value > index ? value - 1 : value);
    state.source = shift(state.source);
    state.target = shift(state.target);
    graphChanged();
  }

  function deleteEdge(index) {
    state.graph.edges.splice(index, 1);
    graphChanged();
  }

  function setDirected(directed) {
    const g = state.graph;
    g.directed = directed;
    if (!directed) {
      // Two opposite directed edges collapse into one undirected edge.
      const seen = new Set();
      g.edges = g.edges.filter(e => {
        const key = Math.min(e.u, e.v) + '-' + Math.max(e.u, e.v);
        if (seen.has(key)) return false;
        seen.add(key);
        return true;
      });
    }
    graphChanged();
  }

  let weightEditor = null;

  function closeWeightEditor(commit = false) {
    if (!weightEditor) return;
    const { input, edge } = weightEditor;
    weightEditor = null;
    const value = Number(input.value);
    input.remove();
    if (commit && input.value.trim() !== '' && Number.isFinite(value) && value >= 0 && state.graph.edges[edge]) {
      state.graph.edges[edge].w = Math.round(value * 100) / 100;
      graphChanged();
    }
  }

  function openWeightEditor(edgeIndex) {
    closeWeightEditor(true);
    const label = svg.querySelector(`[data-edge="${edgeIndex}"] text`);
    if (!label) return;
    const wrap = $('canvasWrap').getBoundingClientRect();
    const box = label.getBoundingClientRect();
    const input = document.createElement('input');
    input.type = 'number';
    input.min = '0';
    input.step = 'any';
    input.className = 'weightInput';
    input.value = state.graph.edges[edgeIndex].w;
    input.setAttribute('aria-label', 'Edge ' + (state.algo.weightName || 'weight'));
    input.style.left = (box.left + box.width / 2 - wrap.left) + 'px';
    input.style.top = (box.top + box.height / 2 - wrap.top) + 'px';
    $('canvasWrap').appendChild(input);
    weightEditor = { input, edge: edgeIndex };
    input.focus();
    input.select();
    input.addEventListener('keydown', event => {
      if (event.key === 'Enter') closeWeightEditor(true);
      if (event.key === 'Escape') closeWeightEditor(false);
      event.stopPropagation();
    });
    input.addEventListener('blur', () => closeWeightEditor(true));
  }

  // ---- Pointer interaction on the canvas -----------------------------------------------------

  function toCanvas(event) {
    const point = new DOMPoint(event.clientX, event.clientY).matrixTransform(svg.getScreenCTM().inverse());
    return { x: clamp(point.x, RADIUS + 4, WIDTH - RADIUS - 4), y: clamp(point.y, RADIUS + 4, HEIGHT - RADIUS - 18) };
  }

  function hit(event) {
    const element = document.elementFromPoint(event.clientX, event.clientY);
    const node = element && element.closest && element.closest('[data-node]');
    if (node && svg.contains(node)) return { node: Number(node.dataset.node) };
    const edge = element && element.closest && element.closest('[data-edge]');
    if (edge && svg.contains(edge)) return { edge: Number(edge.dataset.edge), onLabel: element.tagName === 'text' };
    return {};
  }

  svg.addEventListener('pointerdown', event => {
    if (state.trace || event.button !== 0 || !state.algo.usesGraph) return;
    closeWeightEditor(true);
    const target = hit(event);
    const point = toCanvas(event);
    state.pointer = point;

    if (target.node !== undefined) {
      if (state.tool === 'delete') { deleteNode(target.node); return; }
      if (state.tool === 'edge') {
        if (state.pendingFrom !== null && state.pendingFrom !== target.node) {
          const from = state.pendingFrom;
          state.pendingFrom = null;
          addEdge(from, target.node);
          renderAll();
          return;
        }
        state.pendingFrom = target.node;
        svg.setPointerCapture(event.pointerId);
        renderGraph();
        return;
      }
      if (state.tool === 'move') {
        const node = state.graph.nodes[target.node];
        state.drag = { node: target.node, dx: node.x - point.x, dy: node.y - point.y, moved: false };
        svg.setPointerCapture(event.pointerId);
      }
      return;
    }

    if (target.edge !== undefined) {
      if (state.tool === 'delete') { deleteEdge(target.edge); return; }
      if (state.algo.usesWeights) { event.preventDefault(); openWeightEditor(target.edge); }
      return;
    }

    if (state.tool === 'node') { addNode(point); return; }
    if (state.pendingFrom !== null) { state.pendingFrom = null; renderGraph(); }
  });

  svg.addEventListener('pointermove', event => {
    if (state.trace) return;
    const point = toCanvas(event);
    state.pointer = point;
    if (state.drag) {
      const node = state.graph.nodes[state.drag.node];
      node.x = Math.round(clamp(point.x + state.drag.dx, RADIUS + 4, WIDTH - RADIUS - 4));
      node.y = Math.round(clamp(point.y + state.drag.dy, RADIUS + 4, HEIGHT - RADIUS - 18));
      state.drag.moved = true;
      renderGraph();
    } else if (state.tool === 'edge' && state.pendingFrom !== null) {
      renderGraph();
    }
  });

  svg.addEventListener('pointerup', event => {
    if (state.trace) return;
    if (state.drag) {
      const moved = state.drag.moved;
      state.drag = null;
      if (moved) { state.edited = true; state.sampleId = null; renderAll(); }
      return;
    }
    if (state.tool === 'edge' && state.pendingFrom !== null) {
      const target = hit(event);
      if (target.node !== undefined && target.node !== state.pendingFrom) {
        const from = state.pendingFrom;
        state.pendingFrom = null;
        addEdge(from, target.node);
        renderAll();
      }
      // Released on the same vertex: keep it selected so the edge can be finished with a second click.
    }
  });

  // ---- Drawing -------------------------------------------------------------------------------

  // Geometry of one edge: a quadratic curve (straight unless it has a twin running the other way).
  function edgeShape(a, b, radius, directed, bend) {
    const dx = b.x - a.x, dy = b.y - a.y;
    const length = Math.hypot(dx, dy) || 1;
    const px = -dy / length, py = dx / length;
    const control = { x: (a.x + b.x) / 2 + px * bend, y: (a.y + b.y) / 2 + py * bend };
    const toward = (from, to, amount) => {
      const d = Math.hypot(to.x - from.x, to.y - from.y) || 1;
      return { x: from.x + (to.x - from.x) / d * amount, y: from.y + (to.y - from.y) / d * amount };
    };
    const start = toward(a, control, radius);
    const tip = toward(b, control, radius + 1);
    const end = directed ? toward(b, control, radius + 9) : tip;
    let head = '';
    if (directed) {
      const hx = (tip.x - control.x), hy = (tip.y - control.y);
      const hl = Math.hypot(hx, hy) || 1;
      const ux = hx / hl, uy = hy / hl;
      const base = { x: tip.x - ux * 11, y: tip.y - uy * 11 };
      head = `${tip.x},${tip.y} ${base.x - uy * 5.5},${base.y + ux * 5.5} ${base.x + uy * 5.5},${base.y - ux * 5.5}`;
    }
    const mid = { x: 0.25 * start.x + 0.5 * control.x + 0.25 * end.x, y: 0.25 * start.y + 0.5 * control.y + 0.25 * end.y };
    return {
      path: `M${start.x},${start.y} Q${control.x},${control.y} ${end.x},${end.y}`,
      head,
      label: { x: mid.x + px * 13, y: mid.y + py * 13 },
    };
  }

  // What to draw right now: the graph being edited, or one step of the simulation.
  function currentPicture() {
    const g = state.graph;
    if (!state.trace && !state.algo.usesGraph) {
      // An empty board of the chosen size until the puzzle is run.
      const n = Number(state.options.size) || 8;
      return {
        editable: false,
        layout: 'board',
        nodes: Array.from({ length: n * n }, (_, i) => ({ label: '', x: i % n, y: Math.floor(i / n), state: 'idle', sub: '' })),
        edges: [],
        source: -1,
        target: -1,
      };
    }
    if (!state.trace) {
      const algo = state.algo;
      return {
        layout: 'graph',
        editable: true,
        directed: g.directed,
        nodes: g.nodes.map(n => ({ ...n, state: 'idle', sub: '' })),
        edges: g.edges.map(e => ({ u: e.u, v: e.v, state: 'idle', label: algo.usesWeights ? formatNumber(e.w) : '' })),
        source: algo.source !== 'no' ? state.source : -1,
        target: algo.target !== 'no' ? state.target : -1,
      };
    }
    const trace = state.trace;
    const step = trace.steps[state.step];
    const view = trace.views[step.view];
    return {
      editable: false,
      layout: view.layout,
      directed: view.directed,
      nodes: view.nodes.map((n, i) => ({ ...n, state: trace.nodeStates[step.ns[i]], sub: step.nl[i] })),
      edges: view.edges.map((e, i) => ({
        u: e.u, v: e.v, state: trace.edgeStates[step.es[i]],
        label: step.el[i] || (view.showEdgeLabels ? e.label : ''),
      })),
      source: view.source,
      target: view.target,
    };
  }

  function renderBoard(picture) {
    const n = Math.round(Math.sqrt(picture.nodes.length));
    const size = Math.floor((HEIGHT - 60) / n);
    const left = (WIDTH - size * n) / 2;
    const top = (HEIGHT - size * n) / 2 - 8;
    let html = '';
    picture.nodes.forEach(node => {
      const x = left + node.x * size, y = top + node.y * size;
      const shade = (node.x + node.y) % 2 ? 'dark' : 'light';
      html += `<g class="square ${shade} n-${node.state}"><title>${escapeHtml(node.label)}</title>`
        + `<rect x="${x}" y="${y}" width="${size}" height="${size}"/>`
        + (node.sub ? `<text x="${x + size / 2}" y="${y + size / 2}" font-size="${Math.round(size * 0.66)}">${escapeHtml(node.sub)}</text>` : '')
        + '</g>';
    });
    for (let c = 0; c < n; c++) html += `<text class="coord" x="${left + (c + 0.5) * size}" y="${top + n * size + 16}">${String.fromCharCode(97 + c)}</text>`;
    for (let r = 0; r < n; r++) html += `<text class="coord" x="${left - 14}" y="${top + (r + 0.5) * size}">${n - r}</text>`;
    html += `<rect class="frame" x="${left}" y="${top}" width="${size * n}" height="${size * n}"/>`;
    svg.innerHTML = html;
    svg.classList.remove('editable');
  }

  function renderGraph() {
    const picture = currentPicture();
    if (picture.layout === 'board') {
      renderBoard(picture);
      return;
    }
    const { nodes, edges } = picture;

    // Shrink the vertices when they sit close together (the split network of vertex-disjoint paths).
    let nearest = Infinity;
    for (let i = 0; i < nodes.length; i++) {
      for (let j = i + 1; j < nodes.length; j++) nearest = Math.min(nearest, distance(nodes[i], nodes[j]));
    }
    const radius = clamp(Math.floor(nearest / 2 - 9), 13, RADIUS);

    const pairs = new Set(edges.map(e => e.u + '>' + e.v));
    let html = '';

    edges.forEach((e, i) => {
      const twin = picture.directed && pairs.has(e.v + '>' + e.u);
      const shape = edgeShape(nodes[e.u], nodes[e.v], radius, picture.directed, twin ? 26 : 0);
      html += `<g class="edge e-${e.state}" data-edge="${i}">`
        + `<path class="hit" d="${shape.path}"/><path class="line" d="${shape.path}"/>`
        + (shape.head ? `<polygon class="head" points="${shape.head}"/>` : '')
        + (e.label ? `<text x="${shape.label.x}" y="${shape.label.y}">${escapeHtml(e.label)}</text>` : '')
        + '</g>';
    });

    if (picture.editable && state.tool === 'edge' && state.pendingFrom !== null && state.pointer && nodes[state.pendingFrom]) {
      const from = nodes[state.pendingFrom];
      html += `<line class="rubber" x1="${from.x}" y1="${from.y}" x2="${state.pointer.x}" y2="${state.pointer.y}"/>`;
    }

    nodes.forEach((n, i) => {
      const pending = picture.editable && state.pendingFrom === i ? ' pending' : '';
      const badge = i === picture.source ? 'S' : i === picture.target ? 'T' : '';
      const role = badge === 'S' ? ' (source)' : badge === 'T' ? ' (target)' : '';
      html += `<g class="node n-${n.state}${pending}" data-node="${i}" transform="translate(${n.x},${n.y})">`
        + `<title>${escapeHtml(n.label + role)}</title>`
        + `<circle class="body" r="${radius}"/>`
        + `<text class="name" font-size="${radius >= 18 ? 15 : 12}">${escapeHtml(n.label)}</text>`
        + (n.sub ? `<text class="sub" y="${radius + 15}">${escapeHtml(n.sub)}</text>` : '')
        + (badge ? `<g class="badge" transform="translate(${radius * 0.75},${-radius * 0.75})"><circle r="8"/><text>${badge}</text></g>` : '')
        + '</g>';
    });

    if (nodes.length === 0) {
      html += `<text class="emptyNote" x="${WIDTH / 2}" y="${HEIGHT / 2}">The canvas is empty. Choose “Add vertex” and click here, or load an example.</text>`;
    }

    svg.innerHTML = html;
    svg.classList.toggle('editable', picture.editable);
  }

  const HINTS = {
    move: 'Drag a vertex to move it.',
    node: 'Click an empty spot to add a vertex.',
    edge: 'Click or drag from one vertex to another to connect them.',
    delete: 'Click a vertex or an edge to remove it.',
  };

  const NODE_LEGEND = {
    frontier: 'Discovered, waiting', current: 'Being processed', visited: 'Finished', path: 'On the path',
    cycle: 'On the cycle', cutside: 'Source side of the cut',
  };
  const EDGE_LEGEND = {
    considering: 'Being examined', tree: 'Search tree', back: 'Back edge', cross: 'Forward / cross edge',
    path: 'Path', pathreverse: 'Path, walked backwards', flow: 'Carries flow', saturated: 'Full (saturated)',
    cycle: 'Cycle', cut: 'Minimum cut',
  };

  function renderLegend() {
    const trace = state.trace;
    $('legendCard').hidden = !trace;
    if (!trace) return;
    const nodeStates = new Set(), edgeStates = new Set();
    for (const step of trace.steps) {
      step.ns.forEach(s => nodeStates.add(trace.nodeStates[s]));
      step.es.forEach(s => edgeStates.add(trace.edgeStates[s]));
    }
    const words = { node: {}, edge: {}, ...trace.legend };
    const board = trace.views.some(view => view.layout === 'board');
    let html = '';
    for (const [name, text] of Object.entries(NODE_LEGEND)) {
      if (!nodeStates.has(name)) continue;
      const swatch = board
        ? `<g class="square light n-${name}"><rect x="6" y="1" width="14" height="14"/></g>`
        : `<g class="node n-${name}"><circle class="body" cx="13" cy="8" r="6.5"/></g>`;
      html += `<li><svg viewBox="0 0 26 16">${swatch}</svg>${escapeHtml(words.node[name] || text)}</li>`;
    }
    for (const [name, text] of Object.entries(EDGE_LEGEND)) {
      if (edgeStates.has(name)) {
        html += `<li><svg viewBox="0 0 26 16"><g class="edge e-${name}"><path class="line" d="M2,8 L24,8"/></g></svg>${escapeHtml(words.edge[name] || text)}</li>`;
      }
    }
    if ([...edgeStates].some(s => /^path\d$/.test(s))) {
      html += '<li><svg viewBox="0 0 26 16"><g class="edge e-path0"><path class="line" d="M2,4 L24,4"/></g><g class="edge e-path1"><path class="line" d="M2,12 L24,12"/></g></svg>One colour per path</li>';
    }
    $('legend').innerHTML = html;
    $('legendCard').hidden = html === '';
  }

  function renderSide() {
    const algo = state.algo;
    const trace = state.trace;
    const step = trace ? trace.steps[state.step] : null;

    // Pseudocode comes from the core with the trace; before the first run there is none yet.
    const code = trace ? trace.pseudocode : pseudocodeFor(algo);
    $('pseudocode').innerHTML = code.length
      ? code.map((line, i) => `<li class="${step && step.line === i ? 'active' : ''}">${escapeHtml(line)}</li>`).join('')
      : '<li>Run the simulation to see the pseudocode.</li>';

    const panels = step ? step.panels : [];
    $('panelsCard').hidden = panels.length === 0;
    $('panels').innerHTML = panels.map(panel =>
      `<div class="panel"><h3>${escapeHtml(panel.title)}</h3><div class="chips">`
      + (panel.items.length ? panel.items.map(item => `<span class="chip">${escapeHtml(item)}</span>`).join('') : '<span class="empty">empty</span>')
      + '</div></div>').join('');

    const finished = trace && state.step === trace.steps.length - 1;
    $('resultCard').hidden = !finished || !trace.summary;
    $('result').textContent = finished ? trace.summary : '';

    const message = $('message');
    message.classList.toggle('idle', !step);
    message.textContent = step ? step.msg
      : 'Edit the graph or pick an example, then press “Run simulation”. Each step explains what the algorithm just did and why.';

    const view = step ? trace.views[step.view] : null;
    $('viewTitle').hidden = !(trace && trace.views.length > 1);
    $('viewTitle').textContent = view ? view.title : '';
  }

  function renderPlayer() {
    const trace = state.trace;
    const last = trace ? trace.steps.length - 1 : 0;
    for (const id of ['first', 'prev', 'play', 'next', 'last', 'scrub']) $(id).disabled = !trace;
    if (trace) {
      $('first').disabled = $('prev').disabled = state.step === 0;
      $('next').disabled = $('last').disabled = state.step === last;
    }
    $('scrub').max = last;
    $('scrub').value = state.step;
    $('counter').textContent = trace ? `${state.step + 1} / ${last + 1}` : '–';
    $('play').textContent = state.timer ? 'Pause' : (trace && state.step === last ? 'Replay' : 'Play');
  }

  function renderControls() {
    const algo = state.algo;
    const g = state.graph;
    const tracing = !!state.trace;

    $('algoDescription').textContent = algo.description;
    $('algoComplexity').textContent = algo.complexity;
    $('directed').checked = g.directed;
    $('graphCard').hidden = !algo.usesGraph;
    $('sample').value = state.sampleId || '';
    $('geometric').hidden = !algo.usesWeights;

    for (const button of document.querySelectorAll('[data-tool]')) {
      button.classList.toggle('active', !tracing && algo.usesGraph && button.dataset.tool === state.tool);
      button.disabled = tracing || !algo.usesGraph;
    }
    $('edit').hidden = !tracing;
    $('hint').textContent = !algo.usesGraph ? (tracing ? 'Go back to choose another board size.' : 'This puzzle has its own board. Choose a size and press “Run simulation”.')
      : tracing ? 'Simulation running. Go back to editing to change the graph.'
      : HINTS[state.tool] + (algo.usesWeights ? ` Click an edge to change its ${algo.weightName}.` : '');

    // Source / target / option pickers for the chosen algorithm.
    const vertexOptions = (selected, optional) =>
      (optional ? `<option value="-1"${selected < 0 ? ' selected' : ''}>None (explore everything)</option>` : '')
      + g.nodes.map((n, i) => `<option value="${i}"${i === selected ? ' selected' : ''}>${escapeHtml(n.label)}</option>`).join('');
    let html = '';
    if (algo.source !== 'no') {
      html += `<label class="field"><span>Source</span><select data-field="source">${vertexOptions(state.source, false)}</select></label>`;
    }
    if (algo.target !== 'no') {
      html += `<label class="field"><span>Target</span><select data-field="target">${vertexOptions(state.target, algo.target === 'optional')}</select></label>`;
    }
    for (const option of algo.options) {
      html += `<label class="field"><span>${escapeHtml(option.label)}</span><select data-option="${option.key}">`
        + option.choices.map(c => `<option value="${c.id}"${state.options[option.key] === c.id ? ' selected' : ''}>${escapeHtml(c.label)}</option>`).join('')
        + '</select></label>';
    }
    if (!html) html = '<p class="complexity">This algorithm inspects the whole graph, so there is nothing to choose.</p>';
    $('runFields').innerHTML = html;
  }

  function renderAll() {
    renderControls();
    renderGraph();
    renderSide();
    renderPlayer();
    renderLegend();
  }

  // Pseudocode to show before the first run. It can depend on the options (TSP method, max-flow
  // strategy), so ask the core with the current options on a tiny input: a triangle, and a 1 x 1
  // board for the queens. Cached per algorithm and option combination.
  const pseudocodeCache = new Map();
  function pseudocodeFor(algo) {
    const options = Object.entries(state.options).map(([key, value]) => `${key} ${value}\n`).join('');
    const key = algo.id + '|' + options;
    if (!pseudocodeCache.has(key)) {
      const probe = `algorithm ${algo.id}\ndirected 0\nsource 0\ntarget 1\n${options}size 1\n`
        + 'node 0 0 A\nnode 100 0 B\nnode 50 80 C\nedge 0 1 1\nedge 1 2 1\nedge 2 0 1\n';
      const result = JSON.parse(state.core.ccall('algolab_run', 'string', ['string'], [probe]));
      pseudocodeCache.set(key, result.pseudocode || []);
    }
    return pseudocodeCache.get(key);
  }

  // ---- Playback ------------------------------------------------------------------------------

  function goTo(index) {
    if (!state.trace) return;
    state.step = clamp(index, 0, state.trace.steps.length - 1);
    renderGraph();
    renderSide();
    renderPlayer();
  }

  function stopPlaying() {
    clearInterval(state.timer);
    state.timer = null;
  }

  function startPlaying() {
    if (!state.trace) return;
    stopPlaying();
    if (state.step >= state.trace.steps.length - 1) state.step = 0;
    state.timer = setInterval(() => {
      if (state.step >= state.trace.steps.length - 1) {
        stopPlaying();
        renderPlayer();
        return;
      }
      goTo(state.step + 1);
      if (state.step >= state.trace.steps.length - 1) { stopPlaying(); renderPlayer(); }
    }, Number($('speed').value));
    goTo(state.step);
  }

  function togglePlaying() {
    if (state.timer) { stopPlaying(); renderPlayer(); } else startPlaying();
  }

  function manualStep(index) {
    stopPlaying();
    goTo(index);
  }

  // ---- Wiring --------------------------------------------------------------------------------

  function selectAlgorithm(id) {
    state.algo = state.catalog.find(a => a.id === id);
    state.options = {};
    for (const option of state.algo.options) state.options[option.key] = option.choices[0].id;
    if (!state.algo.usesGraph) {
      clearTrace();
      showError('');
      renderAll();
      return;
    }
    // Show the example that suits the algorithm, unless the user is working on a graph of their own.
    if (!state.edited) {
      loadSample(window.ALGOLAB_DEFAULT_SAMPLE[id] || 'basic');
    } else {
      graphChanged(false);
    }
  }

  function bind() {
    $('algo').addEventListener('change', event => selectAlgorithm(event.target.value));
    $('sample').addEventListener('change', event => { if (event.target.value) loadSample(event.target.value); });
    $('directed').addEventListener('change', event => setDirected(event.target.checked));
    $('random').addEventListener('click', randomGraph);
    $('clear').addEventListener('click', () => {
      state.graph = { directed: state.graph.directed, nodes: [], edges: [] };
      state.source = state.target = -1;
      state.tool = 'node';
      graphChanged();
    });
    $('geometric').addEventListener('click', () => {
      const g = state.graph;
      for (const e of g.edges) e.w = lengthWeight(g.nodes[e.u], g.nodes[e.v]);
      graphChanged();
    });
    $('run').addEventListener('click', runSimulation);
    $('edit').addEventListener('click', () => { clearTrace(); renderAll(); });

    $('runFields').addEventListener('change', event => {
      const select = event.target;
      if (select.dataset.field) state[select.dataset.field] = Number(select.value);
      if (select.dataset.option) state.options[select.dataset.option] = select.value;
      clearTrace();
      showError('');
      renderAll();
    });

    for (const button of document.querySelectorAll('[data-tool]')) {
      button.addEventListener('click', () => {
        state.tool = button.dataset.tool;
        state.pendingFrom = null;
        renderControls();
        renderGraph();
      });
    }

    $('first').addEventListener('click', () => manualStep(0));
    $('prev').addEventListener('click', () => manualStep(state.step - 1));
    $('next').addEventListener('click', () => manualStep(state.step + 1));
    $('last').addEventListener('click', () => manualStep(Infinity));
    $('play').addEventListener('click', togglePlaying);
    $('scrub').addEventListener('input', event => manualStep(Number(event.target.value)));
    $('speed').addEventListener('change', () => { if (state.timer) startPlaying(); });

    document.addEventListener('keydown', event => {
      if (!state.trace || event.target.closest('input, select, textarea')) return;
      // A focused button already toggles itself on space; don't toggle playback a second time.
      if (event.key === ' ' && event.target.closest('button')) return;
      if (event.key === 'ArrowRight') { manualStep(state.step + 1); event.preventDefault(); }
      if (event.key === 'ArrowLeft') { manualStep(state.step - 1); event.preventDefault(); }
      if (event.key === ' ') { togglePlaying(); event.preventDefault(); }
    });
  }

  function fatal(html) {
    $('fatal').innerHTML = html;
    $('fatal').hidden = false;
  }

  async function start() {
    if (window.ALGOLAB_MISSING || typeof createAlgoLab !== 'function') {
      fatal('<b>The C++ core has not been built yet.</b> Build it with Emscripten: '
        + '<code>emcmake cmake -S . -B build/wasm</code> then <code>cmake --build build/wasm</code>. '
        + 'That creates <code>web/wasm/algolab.js</code>; reload this page afterwards.');
      return;
    }
    try {
      state.core = await createAlgoLab();
      state.catalog = JSON.parse(state.core.ccall('algolab_catalog', 'string', [], []));
    } catch (error) {
      fatal('<b>The C++ core failed to load.</b> ' + escapeHtml(error && error.message ? error.message : error));
      return;
    }

    const categories = [...new Set(state.catalog.map(a => a.category))];
    $('algo').innerHTML = categories.map(category =>
      `<optgroup label="${escapeHtml(category)}">`
      + state.catalog.filter(a => a.category === category).map(a => `<option value="${a.id}">${escapeHtml(a.name)}</option>`).join('')
      + '</optgroup>').join('');
    $('sample').innerHTML = '<option value="">Custom graph</option>'
      + window.ALGOLAB_SAMPLES.map(s => `<option value="${s.id}">${escapeHtml(s.name)}</option>`).join('');

    bind();
    selectAlgorithm(state.catalog[0].id);
  }

  start();
})();
