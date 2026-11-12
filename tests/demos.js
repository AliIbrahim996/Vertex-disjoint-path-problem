// Runs every real-world demo (web/js/applications.js) through the WebAssembly core and checks
// the answer and its explanation, then a few edited versions of them. Needs the wasm build:
//   node tests/demos.js            (add --print to see every result and explanation)
'use strict';

const path = require('path');
const root = path.join(__dirname, '..');

global.window = {};
require(path.join(root, 'web/js/request.js'));
require(path.join(root, 'web/js/applications.js'));
const createAlgoLab = require(path.join(root, 'web/wasm/algolab.js'));
const { graphFromSample, requestText, DEFAULT_SCALE } = window.ALGOLAB_REQUEST;

// For each demo: values the outcome must have, and text the explanation must contain.
const EXPECTED = {
  astar: {
    gps: { cost: 12, expanded: 5, dijkstraExpanded: 15, says: ['Home → Lib → Univ → Work', '12 km', 'saved 67%', 'straight-line distance never overestimates'] },
    game: { cost: 15, optimal: true, says: ['15 tiles', 'd5 → e5', 'saving is small', 'Manhattan distance never overestimates'] },
    warehouse: { cost: 11, says: ['11 seconds', 'drives around the busy aisle', 'saved 50%'] },
    maze: { cost: 23, says: ['In → A → C → J → K → Out', 'dead end'] },
    taxi: { cost: 13, expanded: 23, dijkstraExpanded: 35, says: ['13 blocks', 'd1 → e1', 'closer to the true distance'] },
    traveltime: { cost: 16, optimalCost: 13, optimal: false, admissible: false, says: ['16 minutes', 'fastest route takes 13 minutes', 'not admissible'] },
  },
  maxflow: {
    logistics: { maxFlow: 14, says: ['14 trucks per hour', 'J1→J3, J2→J4'] },
    matching: { maxFlow: 4, says: ['4 jobs', 'Every position is filled'] },
    segmentation: { maxFlow: 21, says: ['Object: P1, P2, P3.', 'P2 joins the object', 'P5 is treated as noise'] },
    evacuation: { maxFlow: 27, says: ['27 people per minute', '15 minutes', 'StA→ExN, ExS→Safe'] },
    projects: { maxFlow: 18, says: ['Take on: Web, App', 'pay for: Srv, Lic', '= 5'] },
    water: { maxFlow: 50, says: ['50 of the 53 L/s', 'short by 3', 'P2→DC'] },
    elimination: { maxFlow: 6, says: ['Philadelphia is eliminated', 'ATL, NY', '83.5'] },
    crews: { maxFlow: 4, says: ['4 of 5 flights', 'Without a crew: F5', 'Ben (1 spare)'] },
  },
};

// Graph edits for the variants, on the page's graph format.
const byLabel = (graph, label) => graph.nodes.findIndex(n => n.label === label);
function deleteEdge(graph, a, b) {
  const [u, v] = [byLabel(graph, a), byLabel(graph, b)];
  graph.edges = graph.edges.filter(e => !((e.u === u && e.v === v) || (e.u === v && e.v === u)));
}
function deleteNode(graph, label, ends) {
  const index = byLabel(graph, label);
  graph.nodes.splice(index, 1);
  graph.edges = graph.edges.filter(e => e.u !== index && e.v !== index)
    .map(e => ({ ...e, u: e.u > index ? e.u - 1 : e.u, v: e.v > index ? e.v - 1 : e.v }));
  for (const key of ['source', 'target']) if (ends[key] > index) ends[key]--;
}

// Demos as the user changes them, following a "Try this" or poking at the explanation.
const VARIANTS = [
  { algorithm: 'astar', use: 'gps', options: { heuristic: 'zero' }, expect: { cost: 12 },
    says: ['With no heuristic, A* expanded 15', 'Like Dijkstra, it is still guaranteed'], lacks: ['straight-line distance never', 'saving is small'] },
  { algorithm: 'astar', use: 'taxi', options: { heuristic: 'euclidean' }, expect: { cost: 13 },
    says: ['13 blocks', 'underestimates more'], lacks: ['closer to the true distance'] },
  { algorithm: 'astar', use: 'maze', edit: g => deleteEdge(g, 'A', 'D'), expect: { cost: 23 },
    says: ['In → A → C → J → K → Out'], lacks: ['dead end', 'ran down the corridor'] },
  { algorithm: 'astar', use: 'game', edit: (g, ends) => deleteNode(g, 'd5', ends), expect: { found: false },
    says: ['treasure cannot be reached'] },
  { algorithm: 'maxflow', use: 'matching', edit: g => { g.directed = false; }, expect: { maxFlow: 4 },
    says: ['4 jobs', 'Bob → Dev'], lacks: ['Dev →', 'QA →', 'Ops →', 'UX →'] },
];

// The request the page sends for a demo (graph and options as startDemo() sets them up).
function demoRequest(algorithm, use, { options = {}, edit } = {}) {
  const merged = { ...(use.options || {}), ...options };
  const graph = graphFromSample(use.graph, Number(merged.scale) || DEFAULT_SCALE);
  const ends = { source: use.graph.source, target: use.graph.target };
  if (edit) edit(graph, ends);
  return overrides => {
    const { algorithm: run = algorithm, ...all } = { ...merged, ...overrides };
    return { text: requestText({ algorithm: run, graph, source: ends.source, target: ends.target, options: all }), options: all };
  };
}

createAlgoLab().then(core => {
  let checks = 0, failures = 0;
  const check = (ok, what) => {
    checks++;
    if (!ok) { failures++; console.error('FAIL ' + what); }
  };
  const plain = html => html.replace(/<[^>]+>/g, ' ').replace(/\s+/g, ' ').replace(/ ([.,:])/g, '$1').trim();

  // Run one demo (or variant) and check its outcome and explanation.
  function runCase(name, algorithm, use, setup, expect, says = [], lacks = []) {
    const outcome = window.ALGOLAB_OUTCOMES[algorithm];
    const request = demoRequest(algorithm, use, setup);
    const run = overrides => JSON.parse(core.ccall('algolab_run', 'string', ['string'], [request(overrides).text]));
    const trace = run({});
    check(!trace.error, `${name}: ${trace.error}`);
    if (trace.error || !outcome) return;
    const result = outcome(trace, run, request({}).options);
    for (const [key, value] of Object.entries(expect)) check(result[key] === value, `${name}: ${key} = ${result[key]}, expected ${value}`);
    const text = plain(use.explain(result));
    if (process.argv.includes('--print')) console.log(`\n${name}: ${text}`);
    for (const phrase of says) check(text.includes(phrase), `${name}: explanation lacks "${phrase}": ${text}`);
    for (const phrase of lacks) check(!text.includes(phrase), `${name}: explanation should not say "${phrase}": ${text}`);
  }

  for (const [algorithm, group] of Object.entries(window.ALGOLAB_APPLICATIONS)) {
    check(typeof window.ALGOLAB_OUTCOMES[algorithm] === 'function', `${algorithm}: no ALGOLAB_OUTCOMES entry`);
    for (const use of group.uses) {
      const name = `${algorithm}/${use.id}`;
      for (const field of ['field', 'title', 'summary', 'story']) check(!!use[field], `${name}: missing ${field}`);
      const expected = (EXPECTED[algorithm] || {})[use.id];
      check(!!expected, `${name}: no expectation in tests/demos.js`);
      if (!expected) continue;
      const { says, ...values } = expected;
      runCase(name, algorithm, use, {}, values, says);
    }
  }

  for (const variant of VARIANTS) {
    const use = window.ALGOLAB_APPLICATIONS[variant.algorithm].uses.find(u => u.id === variant.use);
    runCase(`${variant.algorithm}/${variant.use} (edited)`, variant.algorithm, use, variant, variant.expect, variant.says, variant.lacks);
  }

  console.log(`${checks - failures} of ${checks} demo checks passed`);
  process.exit(failures ? 1 : 0);
});
