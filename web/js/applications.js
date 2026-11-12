// Real-world uses of the algorithms, each with a demo that runs in the simulator.
// This file is data only: every demo runs the real C++ algorithm in the WebAssembly core.
//
// Keyed by catalogue id. Each use has:
//   field, title, summary   the card on the "Real-world uses" tab
//   mapping                 how the real problem becomes a graph: [term, meaning] pairs
//   story                   shown beside the canvas while the demo runs
//   key                     what the vertex labels stand for: [label, meaning] pairs
//   tryThis                 an experiment to make after the first run
//   options                 algorithm options to use (optional)
//   graph                   same format as samples.js: nodes [label, x, y], edges [from, to, weight]
//   explain(outcome)        HTML that reads the finished run in real-world terms
//
// `outcome` is what ALGOLAB_OUTCOMES[algorithm](trace, run, options) reads from the finished trace;
// `run(overrides)` runs the same graph again (for example with { algorithm: 'dijkstra' }) and
// `options` are the options the trace ran with. For max flow:
//   { maxFlow, minCut, source, target,
//     nodes: [{ label, sourceSide }],                       sourceSide: on the source side of the min cut
//     edges: [{ u, v, from, to, flow, capacity, cut }] }     u -> v is the way the flow runs; cut: in the minimum cut
// For A*:
//   { found, path: [labels], cost, optimalCost, optimal, admissible, expanded, vertices,
//     explored: [labels expanded or on the path], heuristic: "euclidean" | "manhattan" | "zero",
//     dijkstraExpanded (computed on first use) }
//
// tests/demos.js runs every demo through the core and checks its answer; update it with the demos.
(function () {
  'use strict';

  // How to read each algorithm's finished trace into an `outcome`. Add an entry when adding demos
  // for another algorithm.
  window.ALGOLAB_OUTCOMES = {
    // From the last step: its edge labels read "flow/capacity" (on an undirected edge with flow,
    // followed by the direction, "3/5 A→B"), and the minimum cut is drawn as shaded vertices and
    // "cut" edges.
    maxflow(trace) {
      const view = trace.views[0];
      const last = trace.steps[trace.steps.length - 1];
      const nodes = view.nodes.map((n, i) => ({ label: n.label, sourceSide: trace.nodeStates[last.ns[i]] === 'cutside' }));
      const edges = view.edges.map((e, i) => {
        const match = /^(-?[\d.]+)\/([\d.]+)(?: (.+)→(.+))?$/.exec(last.el[i] || '');
        let { u, v } = e;
        if (!view.directed) {
          // Point the edge the way its flow runs; with no flow, away from the source / into the sink.
          const backwards = match && match[3] !== undefined
            ? match[3] === nodes[e.v].label && match[4] === nodes[e.u].label
            : e.v === view.source || e.u === view.target;
          if (backwards) [u, v] = [v, u];
        }
        return {
          u, v, from: nodes[u].label, to: nodes[v].label,
          flow: match ? Number(match[1]) : 0,
          capacity: match ? Number(match[2]) : Number(e.label),
          cut: trace.edgeStates[last.es[i]] === 'cut',
        };
      });
      return { maxFlow: Number(trace.result.maxFlow), minCut: Number(trace.result.minCut), source: view.source, target: view.target, nodes, edges };
    },

    // From the results and the last step (expanded vertices stay "visited", the path is "path").
    // The Dijkstra comparison runs only when an explanation asks for it.
    astar(trace, run, options = {}) {
      const r = trace.result;
      const view = trace.views[0];
      const last = trace.steps[trace.steps.length - 1];
      let dijkstraExpanded;
      return {
        found: r.found === 'true',
        path: r.path ? r.path.split(' → ') : [],
        cost: Number(r.cost),
        optimalCost: Number(r.optimalCost),
        optimal: r.optimal === 'true',
        admissible: r.admissible === 'true',
        expanded: Number(r.expanded),
        vertices: view.nodes.length,
        explored: view.nodes.filter((n, i) => ['visited', 'path'].includes(trace.nodeStates[last.ns[i]])).map(n => n.label),
        heuristic: options.heuristic || 'euclidean',
        get dijkstraExpanded() {
          if (dijkstraExpanded === undefined) {
            const dijkstra = run ? run({ algorithm: 'dijkstra' }) : null;
            dijkstraExpanded = dijkstra && !dijkstra.error ? Number(dijkstra.result.expanded) : NaN;
          }
          return dijkstraExpanded;
        },
      };
    },
  };

  const esc = text => String(text).replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
  const list = items => items.length ? items.map(esc).join(', ') : 'none';
  const plural = (n, word) => `${n} ${word}${n === 1 ? '' : 's'}`;

  // Edges that leave the source / enter the sink, and edges between two inner vertices.
  const fromSource = o => o.edges.filter(e => e.u === o.source);
  const intoSink = o => o.edges.filter(e => e.v === o.target);
  const inner = o => o.edges.filter(e => e.u !== o.source && e.v !== o.target);
  const sum = (edges, field) => edges.reduce((total, e) => total + e[field], 0);

  // Bipartite assignment (people to jobs, crews to flights): who got what, and what was left over.
  function assignment(o) {
    const pairs = inner(o).filter(e => e.flow > 0);
    const byPerson = new Map();
    for (const e of pairs) byPerson.set(e.from, [...(byPerson.get(e.from) || []), e.to]);
    const lines = [...byPerson].map(([person, got]) => `<li><b>${esc(person)}</b> → ${list(got)}</li>`).join('');
    const unserved = intoSink(o).filter(e => e.flow < e.capacity).map(e => e.from);
    const idle = fromSource(o).filter(e => e.flow < e.capacity).map(e => e.to);
    return { pairs, lines, unserved, idle };
  }

  // Brightness strip for the segmentation demo: one square per pixel.
  const PIXELS = [9, 4, 8, 2, 6, 1];
  function strip(values, mask) {
    return '<div class="pixels">' + values.map((b, i) => {
      const grey = Math.round(255 * (1 - b / 10));
      const tag = mask ? (mask[i] ? 'fg' : 'bg') : '';
      return `<span class="pixel ${tag}" style="background:rgb(${grey},${grey},${grey})"><i>P${i + 1}</i><b>${mask ? (mask[i] ? 'object' : 'back') : b}</b></span>`;
    }).join('') + '</div>';
  }

  // A grid of square cells named a1, b1, ... (column letter, row number), joined to the cells
  // left, right, above and below. `blocked` cells are left out; `closed` lists streets ("d2-e2")
  // to leave out; `weight(a, b)` gives the cost of a step between two named cells.
  function grid({ columns, rows, left, top, step, blocked = [], closed = [], weight = () => 1 }) {
    const name = (c, r) => String.fromCharCode(97 + c) + (r + 1);
    const nodes = [], index = new Map();
    for (let r = 0; r < rows; r++) {
      for (let c = 0; c < columns; c++) {
        if (blocked.includes(name(c, r))) continue;
        index.set(name(c, r), nodes.length);
        nodes.push([name(c, r), left + c * step, top + r * step]);
      }
    }
    const edges = [];
    const join = (a, b) => {
      if (index.has(a) && index.has(b) && !closed.includes(a + '-' + b)) edges.push([index.get(a), index.get(b), weight(a, b)]);
    };
    for (let r = 0; r < rows; r++) {
      for (let c = 0; c < columns; c++) {
        if (c + 1 < columns) join(name(c, r), name(c + 1, r));
        if (r + 1 < rows) join(name(c, r), name(c, r + 1));
      }
    }
    return { nodes, edges, at: label => index.get(label) };
  }

  // The part every A* explanation shares: how much work the heuristic saved.
  function savings(o, places) {
    if (!Number.isFinite(o.dijkstraExpanded)) return `<p>A* expanded ${plural(o.expanded, places)} out of ${o.vertices}.</p>`;
    if (o.heuristic === 'zero') {
      return `<p>With no heuristic, A* expanded <b>${o.expanded}</b> of the ${o.vertices} ${places}s: the same work as Dijkstra `
        + `(${o.dijkstraExpanded}), because without an estimate it has no sense of direction either.</p>`;
    }
    const saved = o.dijkstraExpanded - o.expanded;
    return `<p>A* expanded <b>${o.expanded}</b> of the ${o.vertices} ${places}s. Dijkstra, which has no sense of direction, expands `
      + `<b>${o.dijkstraExpanded}</b> on the same map`
      + (saved > 0 ? `: the heuristic saved ${Math.round(saved / o.dijkstraExpanded * 100)}% of the work.</p>` : '.</p>')
      + (saved < o.dijkstraExpanded / 4
        ? '<p>The saving is small here: on a small map, or when walls force a detour, there is little to skip. On a map with millions of places '
          + 'A* typically skips almost all of them.</p>'
        : '');
  }
  const route = o => o.path.map(esc).join(' → ');

  // Whether the answer is guaranteed to be the best one, for the heuristic that actually ran.
  const HEURISTIC_NAMES = { euclidean: 'The straight-line distance', manhattan: 'Manhattan distance' };
  function guarantee(o, cost) {
    if (o.heuristic === 'zero') return `<p>Like Dijkstra, it is still guaranteed to find the best ${cost}.</p>`;
    if (!o.admissible) return '<p>The heuristic overestimates somewhere on this map, so the answer is not guaranteed to be the best.</p>';
    return `<p>${HEURISTIC_NAMES[o.heuristic] || 'The heuristic'} never overestimates the ${cost} here, so A* is guaranteed to return the best one.</p>`;
  }

  const GAME = grid({ columns: 8, rows: 5, left: 100, top: 60, step: 100, blocked: ['d1', 'd2', 'd3', 'd4', 'f2', 'f3', 'f4', 'f5'] });
  const CONGESTED = ['b3-c3', 'c3-d3'];
  const WAREHOUSE = grid({
    columns: 8, rows: 5, left: 100, top: 60, step: 100,
    blocked: ['b2', 'c2', 'd2', 'f2', 'g2', 'b4', 'c4', 'd4', 'f4', 'g4'],
    weight: (a, b) => (CONGESTED.includes(a + '-' + b) ? 4 : 1),
  });
  const TAXI = grid({
    columns: 8, rows: 5, left: 100, top: 60, step: 100,
    closed: ['d2-e2', 'd3-e3', 'd4-e4', 'd5-e5', 'f1-f2', 'g1-g2'],
  });

  // Sports elimination: wins so far, and the best record PHI can still reach.
  const WINS = { ATL: 83, NY: 78, MTL: 77 };
  const PHI_BEST = 83;

  window.ALGOLAB_APPLICATIONS = {
    maxflow: {
      intro: 'Maximum flow answers "how much can get from here to there at most?" when every link has a limit. '
        + 'The same calculation, and the minimum cut that comes with it, solves problems that look nothing like pipes: '
        + 'matching people to jobs, separating an object from its background, or proving a team can no longer win.',
      uses: [
        {
          id: 'logistics',
          field: 'Transportation & logistics',
          title: 'Freight through a road network',
          summary: 'How many trucks per hour can travel from the port to the distribution hub, and which roads hold the traffic back?',
          mapping: [
            ['Source', 'the port where the goods arrive'],
            ['Sink', 'the distribution hub'],
            ['Capacity', 'trucks per hour a road can carry'],
            ['Max flow', 'the most trucks per hour the network can deliver'],
            ['Min cut', 'the bottleneck roads: widening any other road changes nothing'],
          ],
          story: 'Trucks leave the port and drive through four junctions to the hub. Each road carries a limited number of trucks per hour. '
            + 'Max flow finds the best way to spread the traffic over the roads, and the minimum cut shows the bottleneck.',
          key: [['Port', 'container port (source)'], ['J1–J4', 'road junctions'], ['Hub', 'distribution hub (sink)']],
          tryThis: 'Click the label of a road outside the cut, raise its capacity and run again: the answer does not change. Now raise a cut road.',
          graph: {
            directed: true, source: 0, target: 5,
            nodes: [['Port', 90, 260], ['J1', 310, 120], ['J2', 310, 400], ['J3', 580, 120], ['J4', 580, 400], ['Hub', 810, 260]],
            edges: [[0, 1, 12], [0, 2, 10], [1, 3, 8], [1, 2, 4], [2, 4, 6], [4, 3, 3], [3, 5, 10], [4, 5, 7]],
          },
          explain(o) {
            const roads = o.edges.filter(e => e.cut).map(e => `${e.from}→${e.to}`);
            return `<p>The network delivers at most <b>${o.maxFlow} trucks per hour</b> to the hub.</p>`
              + `<p>Bottleneck roads (the minimum cut): <b>${list(roads)}</b>. Together they carry ${o.minCut} trucks per hour and are completely full. `
              + 'Any route from the port to the hub uses one of them, so investing in other roads cannot raise the throughput.</p>';
          },
        },
        {
          id: 'matching',
          field: 'Bipartite matching',
          title: 'Applicants to open positions',
          summary: 'Four applicants, four jobs, and each applicant is qualified for only some of them. How many jobs can be filled?',
          mapping: [
            ['Source', 'connected to every applicant with capacity 1 (one job each)'],
            ['Sink', 'connected from every job with capacity 1 (one person each)'],
            ['Middle edges', '"is qualified for", capacity 1'],
            ['Max flow', 'the largest number of applicant–job pairs; edges carrying flow are the pairs'],
          ],
          story: 'Ann can do Dev or QA, Bob only Dev, Cat QA or Ops, and Dan Ops or UX. Giving Ann the Dev job first leaves Bob without work. '
            + 'Watch how max flow undoes that choice: an augmenting path walks backwards along Ann→Dev and moves Ann to QA.',
          key: [['Ann, Bob, Cat, Dan', 'applicants'], ['Dev, QA, Ops, UX', 'open positions'], ['S / T', 'added source and sink']],
          tryThis: 'Delete the edge Cat→Ops and run again: one job must now stay empty, and the result says which one.',
          graph: {
            directed: true, source: 0, target: 9,
            nodes: [['S', 80, 260], ['Ann', 320, 80], ['Bob', 320, 200], ['Cat', 320, 320], ['Dan', 320, 440],
              ['Dev', 600, 80], ['QA', 600, 200], ['Ops', 600, 320], ['UX', 600, 440], ['T', 830, 260]],
            edges: [[0, 1, 1], [0, 2, 1], [0, 3, 1], [0, 4, 1],
              [1, 5, 1], [1, 6, 1], [2, 5, 1], [3, 6, 1], [3, 7, 1], [4, 7, 1], [4, 8, 1],
              [5, 9, 1], [6, 9, 1], [7, 9, 1], [8, 9, 1]],
          },
          explain(o) {
            const a = assignment(o);
            return `<p><b>${plural(a.pairs.length, 'job')}</b> can be filled at the same time:</p><ul>${a.lines}</ul>`
              + (a.unserved.length ? `<p>No qualified applicant is left for: <b>${list(a.unserved)}</b>.</p>` : '<p>Every position is filled.</p>')
              + (a.idle.length ? `<p>Without a job: <b>${list(a.idle)}</b>.</p>` : '');
          },
        },
        {
          id: 'segmentation',
          field: 'Image segmentation',
          title: 'Separating an object from its background',
          summary: 'Label each pixel of an image as object or background, trusting brightness but preferring neighbours to agree.',
          mapping: [
            ['Source', '"object": an edge to each pixel, as strong as the pixel looks like the object (its brightness)'],
            ['Sink', '"background": an edge from each pixel, as strong as it looks like background (10 − brightness)'],
            ['Neighbour edges', 'capacity 3: the price of cutting two touching pixels apart'],
            ['Min cut', 'the cheapest labelling: pixels on the source side are the object'],
          ],
          story: 'A row of six pixels, brightness 0–10. A simple threshold would call P1, P3 and P5 the object. But P2 is surrounded by bright pixels, '
            + 'and P5 is a bright speck in a dark area. The minimum cut weighs both kinds of evidence at once.',
          key: [['P1–P6', 'pixels, left to right'], ['Obj', 'object terminal (source)'], ['Bg', 'background terminal (sink)']],
          keyHtml: strip(PIXELS),
          tryThis: 'Lower the neighbour capacities between P1, P2 and P3 to 1 and run again: P2 is now labelled by its own brightness.',
          graph: {
            directed: true, source: 0, target: 7,
            nodes: [['Obj', 450, 55], ...PIXELS.map((_, i) => [`P${i + 1}`, 140 + i * 124, 260]), ['Bg', 450, 465]],
            edges: [
              ...PIXELS.map((b, i) => [0, i + 1, b]),
              ...PIXELS.map((b, i) => [i + 1, 7, 10 - b]),
              ...PIXELS.slice(1).flatMap((_, i) => [[i + 1, i + 2, 3], [i + 2, i + 1, 3]]),
            ],
          },
          explain(o) {
            const mask = PIXELS.map((_, i) => {
              const node = o.nodes.find(n => n.label === `P${i + 1}`);
              return node ? node.sourceSide : false;
            });
            const object = mask.map((m, i) => (m ? `P${i + 1}` : null)).filter(Boolean);
            return strip(PIXELS, mask)
              + `<p>Object: <b>${list(object)}</b>. The labelling costs ${o.minCut}: the sum of the cut edges, the cheapest possible.</p>`
              + (mask[1] && PIXELS[1] < 5 ? '<p>P2 joins the object although it is dark: cutting it away from two bright neighbours would cost more than its darkness saves.</p>' : '')
              + (!mask[4] && PIXELS[4] >= 5 ? '<p>The bright speck P5 is treated as noise and left in the background, for the same reason.</p>' : '')
              + '<p>Real tools solve the same cut for millions of pixels, with an edge between every pair of neighbours.</p>';
          },
        },
        {
          id: 'evacuation',
          field: 'Disaster management',
          title: 'Evacuating a building',
          summary: 'How many people per minute can leave a three-storey building through its stairs and exits?',
          mapping: [
            ['Source', 'a "super source" linked to every floor: many starting points become one'],
            ['Sink', 'a "super sink" (Safe) linked from every exit'],
            ['Capacity', 'people per minute a corridor, staircase or door lets through'],
            ['Max flow', 'evacuation rate; the cut shows which stairs or doors to widen first'],
          ],
          story: 'Floors 3 and 2 reach the street by two staircases; floor 1 also has its own door to the south exit. '
            + 'With 400 people inside, the evacuation rate decides how long the building takes to empty.',
          key: [['All', 'everyone in the building (super source)'], ['F1–F3', 'floors'], ['StA, StB', 'staircases'],
            ['ExN, ExS', 'north and south exits'], ['Safe', 'assembly point (super sink)']],
          tryThis: 'Raise the south exit (ExS→Safe) from 15 to 25 and run again. How much faster is the evacuation, and what is the new bottleneck?',
          graph: {
            directed: true, source: 0, target: 8,
            nodes: [['All', 70, 260], ['F3', 250, 90], ['F2', 250, 260], ['F1', 250, 430], ['StA', 480, 150], ['StB', 480, 320],
              ['ExN', 690, 150], ['ExS', 690, 360], ['Safe', 845, 260]],
            edges: [[0, 1, 20], [0, 2, 20], [0, 3, 20], [1, 4, 8], [1, 5, 6], [2, 4, 6], [2, 5, 8], [3, 7, 10],
              [4, 6, 12], [5, 7, 10], [6, 8, 15], [7, 8, 15]],
          },
          explain(o) {
            const people = 400;
            const limits = o.edges.filter(e => e.cut).map(e => `${e.from}→${e.to}`);
            return `<p>At most <b>${o.maxFlow} people per minute</b> reach safety.</p>`
              + (o.maxFlow > 0 ? `<p>${people} people need at least <b>${Math.ceil(people / o.maxFlow)} minutes</b> to get out.</p>` : '')
              + `<p>What limits the rate (the minimum cut): <b>${list(limits)}</b>. These are where wider stairs or extra doors pay off.</p>`;
          },
        },
        {
          id: 'projects',
          field: 'Project selection',
          title: 'Choosing profitable projects',
          summary: 'Projects earn money but need resources that cost money, and resources can be shared. Which projects should we take on?',
          mapping: [
            ['Source → project', 'capacity = the project\'s profit'],
            ['Project → resource', 'capacity "unlimited" (99): taking the project means paying for the resource'],
            ['Resource → sink', 'capacity = the resource\'s cost'],
            ['Min cut', 'projects and resources on the source side are the plan; best profit = total profit − cut'],
          ],
          story: 'The website earns 8 and needs a server (cost 5). The mobile app earns 9 and needs the server and a licence (cost 7). '
            + 'The AI feature earns 6 but needs GPUs (cost 9). Shared costs make the choice hard to see by hand.',
          key: [['Web, App, AI', 'projects'], ['Srv, Lic, GPU', 'resources: server, licence, GPUs'], ['S / T', 'added source and sink'], ['99', 'stands for "unlimited"']],
          tryThis: 'Lower the GPU cost (GPU→T) to 4 and run again: the AI feature joins the plan.',
          graph: {
            directed: true, source: 0, target: 7,
            nodes: [['S', 80, 260], ['Web', 330, 110], ['App', 330, 260], ['AI', 330, 410], ['Srv', 600, 110], ['Lic', 600, 260], ['GPU', 600, 410], ['T', 830, 260]],
            edges: [[0, 1, 8], [0, 2, 9], [0, 3, 6], [1, 4, 99], [2, 4, 99], [2, 5, 99], [3, 6, 99], [4, 7, 5], [5, 7, 7], [6, 7, 9]],
          },
          explain(o) {
            const projects = fromSource(o);
            const resources = intoSink(o);
            const chosen = projects.filter(e => o.nodes[e.v].sourceSide).map(e => e.to);
            const bought = resources.filter(e => o.nodes[e.u].sourceSide).map(e => e.from);
            const profit = sum(projects, 'capacity') - o.minCut;
            return `<p>Take on: <b>${list(chosen)}</b>, and pay for: <b>${list(bought)}</b>.</p>`
              + `<p>Best profit = total profit of all projects (${sum(projects, 'capacity')}) − minimum cut (${o.minCut}) = <b>${profit}</b>.</p>`
              + '<p>A project on the source side forces its resources onto the source side too: cutting an "unlimited" edge is never cheapest. '
              + 'So the cut pays for exactly the resources the chosen projects need, plus the profit of the projects left out.</p>';
          },
        },
        {
          id: 'water',
          field: 'Water & power distribution',
          title: 'Supplying a city with water',
          summary: 'Can the pipe network meet every district\'s demand? If not, which district goes short and which pipe is to blame?',
          mapping: [
            ['Source', 'the reservoir'],
            ['Sink', 'a "super sink" (Use) linked from every district'],
            ['Pipe capacity', 'litres per second a pipe can carry'],
            ['District → sink', 'capacity = the district\'s demand, so a full edge means the demand is met'],
          ],
          story: 'Water flows from the reservoir through two pumping stations and a storage tank to three districts that need 20, 18 and 15 litres per second. '
            + 'The same model plans power grids, with line capacities instead of pipes.',
          key: [['Res', 'reservoir (source)'], ['P1, P2', 'pumping stations'], ['Tnk', 'storage tank'], ['DA, DB, DC', 'districts'], ['Use', 'all consumers (super sink)']],
          tryThis: 'Raise the pipe P2→DC to 15 and run again: does every district get its water now?',
          graph: {
            directed: true, source: 0, target: 7,
            nodes: [['Res', 80, 260], ['P1', 280, 140], ['P2', 280, 380], ['Tnk', 480, 260], ['DA', 680, 110], ['DB', 680, 260], ['DC', 680, 410], ['Use', 850, 260]],
            edges: [[0, 1, 30], [0, 2, 25], [1, 3, 15], [1, 4, 15], [2, 3, 10], [2, 6, 12], [3, 4, 6], [3, 5, 20], [4, 7, 20], [5, 7, 18], [6, 7, 15]],
          },
          explain(o) {
            const districts = intoSink(o);
            const demand = sum(districts, 'capacity');
            const lines = districts.map(e => `<li><b>${esc(e.from)}</b>: ${e.flow} of ${e.capacity} L/s${e.flow < e.capacity ? ` <span class="short">short by ${Math.round((e.capacity - e.flow) * 100) / 100}</span>` : ''}</li>`).join('');
            const pipes = o.edges.filter(e => e.cut && e.v !== o.target).map(e => `${e.from}→${e.to}`);
            return `<p>The network delivers <b>${o.maxFlow} of the ${demand} L/s</b> the city needs.</p><ul>${lines}</ul>`
              + (o.maxFlow < demand
                ? `<p>Pipes that limit the supply (in the minimum cut): <b>${list(pipes)}</b>.</p>`
                : '<p>Every district gets all the water it needs.</p>');
          },
        },
        {
          id: 'elimination',
          field: 'Sports analytics',
          title: 'Is a team mathematically eliminated?',
          summary: 'Philadelphia can reach at most 83 wins. Can the remaining games still end with nobody above them?',
          mapping: [
            ['Source → game', 'capacity = games left between two other teams'],
            ['Game → team', 'unlimited (99): either team can win those games'],
            ['Team → sink', 'capacity = wins the team may still add without passing Philadelphia (83 − its wins)'],
            ['Max flow', 'if every remaining game fits (flow = games left), Philadelphia can still finish first'],
          ],
          story: 'Atlanta has 83 wins, New York 78 and Montreal 77. Philadelphia has 80 wins and 3 games left, so at best 83. '
            + 'Atlanta and New York still play each other 6 times, and Atlanta and Montreal once. Someone has to win those games.',
          key: [['A-N, A-M', 'remaining games: Atlanta–New York (6), Atlanta–Montreal (1)'], ['ATL, NY, MTL', 'the other teams'], ['S / T', 'added source and sink']],
          tryThis: 'Lower the Atlanta–New York games (S→A-N) from 6 to 5 and run again: Philadelphia is alive again.',
          graph: {
            directed: true, source: 0, target: 6,
            nodes: [['S', 80, 260], ['A-N', 310, 170], ['A-M', 310, 350], ['ATL', 570, 260], ['NY', 570, 110], ['MTL', 570, 410], ['T', 830, 260]],
            edges: [[0, 1, 6], [0, 2, 1], [1, 3, 99], [1, 4, 99], [2, 3, 99], [2, 5, 99],
              [3, 6, PHI_BEST - WINS.ATL], [4, 6, PHI_BEST - WINS.NY], [5, 6, PHI_BEST - WINS.MTL]],
          },
          explain(o) {
            const games = sum(fromSource(o), 'capacity');
            if (o.maxFlow >= games) {
              return `<p>All <b>${games} remaining games</b> can be given winners without anyone passing ${PHI_BEST} wins: `
                + '<b>Philadelphia is not eliminated</b> yet.</p>';
            }
            const teams = intoSink(o).filter(e => o.nodes[e.u].sourceSide).map(e => e.from);
            const among = fromSource(o).filter(g => {
              const sides = o.edges.filter(e => e.u === g.v).map(e => e.to);
              return sides.length && sides.every(t => teams.includes(t));
            });
            const wins = teams.reduce((total, t) => total + (WINS[t] || 0), 0) + sum(among, 'capacity');
            const proof = teams.length
              ? `<p>Proof from the minimum cut: ${list(teams)} already have ${teams.reduce((t, x) => t + (WINS[x] || 0), 0)} wins and play each other `
                + `${sum(among, 'capacity')} more times, so together they finish with ${wins} wins, an average of `
                + `<b>${Math.round(wins / teams.length * 100) / 100}</b>. At least one of them must end above ${PHI_BEST}.</p>`
              : '';
            return `<p>Only ${o.maxFlow} of the ${games} remaining games can be given a winner without someone passing ${PHI_BEST} wins: `
              + '<b>Philadelphia is eliminated</b>, even though it is not last in wins.</p>' + proof;
          },
        },
        {
          id: 'crews',
          field: 'Airline scheduling',
          title: 'Assigning crews to flights',
          summary: 'Each crew may fly a limited number of flights a day and only the routes it is certified for. Can every flight be staffed?',
          mapping: [
            ['Source → crew', 'capacity = flights the crew may fly today'],
            ['Crew → flight', '"is certified for", capacity 1'],
            ['Flight → sink', 'capacity 1: each flight needs one crew'],
            ['Max flow', 'the number of flights that can be staffed; flow edges are the roster'],
          ],
          story: 'Ana and Ben may fly two flights each, Cy only one. Ana is certified for F1 and F2, Ben for F1–F3, and Cy for F4 and F5. '
            + 'Unlike one-to-one matching, a crew can take several flights, set by its capacity from the source.',
          key: [['Ana, Ben, Cy', 'crews'], ['F1–F5', 'today\'s flights'], ['S / T', 'added source and sink']],
          tryThis: 'Certify Ben for F5: add an edge Ben→F5 with capacity 1 and run again.',
          graph: {
            directed: true, source: 0, target: 9,
            nodes: [['S', 80, 260], ['Ana', 320, 120], ['Ben', 320, 290], ['Cy', 320, 420],
              ['F1', 590, 60], ['F2', 590, 160], ['F3', 590, 260], ['F4', 590, 360], ['F5', 590, 460], ['T', 830, 260]],
            edges: [[0, 1, 2], [0, 2, 2], [0, 3, 1], [1, 4, 1], [1, 5, 1], [2, 4, 1], [2, 5, 1], [2, 6, 1], [3, 7, 1], [3, 8, 1],
              [4, 9, 1], [5, 9, 1], [6, 9, 1], [7, 9, 1], [8, 9, 1]],
          },
          explain(o) {
            const a = assignment(o);
            const spare = fromSource(o).filter(e => e.flow < e.capacity).map(e => `${e.to} (${e.capacity - e.flow} spare)`);
            return `<p><b>${a.pairs.length} of ${intoSink(o).length} flights</b> can be staffed:</p><ul>${a.lines}</ul>`
              + (a.unserved.length ? `<p>Without a crew: <b>${list(a.unserved)}</b>. ` : '<p>Every flight has a crew. ')
              + (spare.length ? `Crews with time left but no certified flight free: ${list(spare)}.</p>` : '</p>')
              + (a.unserved.length ? '<p>Certifying a crew with spare time for an unstaffed route would fix the roster.</p>' : '');
          },
        },
      ],
    },

    astar: {
      intro: 'A* finds the cheapest route from one place to another, like Dijkstra, but it also estimates how far each place still is from the goal '
        + '(the heuristic) and looks at the promising places first. As long as the estimate never overestimates, the route is still the best one. '
        + 'That is why it sits inside navigation apps, games and robots: they need the best route, and they need it fast.',
      uses: [
        {
          id: 'gps',
          field: 'Navigation apps',
          title: 'Driving directions across town',
          summary: 'A navigation app has to find the shortest route through thousands of junctions in a fraction of a second.',
          mapping: [
            ['Vertex', 'a junction or a place'],
            ['Edge weight', 'road length in km'],
            ['Heuristic', 'straight-line distance to the destination, which no road can beat'],
            ['Result', 'the shortest route, found while ignoring the part of town that leads away from the goal'],
          ],
          story: 'You drive from home to work. A* scores every junction by the distance driven so far plus the straight-line distance still to go, '
            + 'so it explores roads heading towards work first and barely looks at the other side of town.',
          key: [['Home, Work', 'start and destination'], ['Other labels', 'junctions, named after a nearby place (library, station, university, …)'], ['Weights', 'road length in km']],
          tryThis: 'Change the heuristic to "None" (that is Dijkstra) and run again: same route, but look how much more of the town gets explored.',
          options: { heuristic: 'euclidean' },
          graph: {
            directed: false, geometric: true, source: 0, target: 15,
            nodes: [['Home', 330, 290], ['Fort', 190, 220], ['Lake', 170, 440], ['Gym', 280, 100], ['Zoo', 70, 110], ['Farm', 60, 330],
              ['Pier', 330, 470], ['Lib', 480, 200], ['Mall', 500, 370], ['Stn', 560, 70], ['Univ', 650, 190], ['Hosp', 640, 450],
              ['Bank', 730, 330], ['Mkt', 760, 80], ['Park', 420, 470], ['Work', 845, 230]],
            edges: [[0, 1], [0, 2], [0, 3], [1, 4], [1, 5], [3, 4], [5, 2], [2, 6], [0, 6], [1, 3],
              [0, 7], [0, 8], [3, 9], [7, 9], [7, 10], [8, 10], [8, 11], [9, 10], [9, 13], [13, 15], [10, 15], [11, 12], [12, 15], [10, 12], [6, 14], [14, 8], [14, 11]],
          },
          explain(o) {
            if (!o.found) return '<p>Work cannot be reached from home on these roads.</p>';
            return `<p>Shortest route: <b>${route(o)}</b>, <b>${o.cost} km</b>.</p>` + savings(o, 'junction')
              + guarantee(o, 'route')
              + '<p>Real apps add tricks on top (precomputed shortcuts, travel times), but the idea is the same.</p>';
          },
        },
        {
          id: 'game',
          field: 'Video games',
          title: 'A game character finding its way',
          summary: 'When you click on a spot, the character has to walk there around the walls, without wandering off.',
          mapping: [
            ['Vertex', 'a floor tile; walls are simply missing tiles'],
            ['Edge', 'a step to a neighbouring tile, cost 1'],
            ['Heuristic', 'Manhattan distance: tiles left/right plus tiles up/down to the goal'],
            ['Result', 'the shortest walk; games repeat this for many characters every second'],
          ],
          story: 'The hero stands at a3 and the treasure is at h3, behind two walls. The direct way is blocked, so A* has to discover the gaps at the bottom '
            + 'of the first wall and the top of the second.',
          key: [['a1 … h5', 'tiles: column letter, row number'], ['Gaps', 'walls fill columns d (rows 1–4) and f (rows 2–5)'], ['Weights', 'one step = 1']],
          tryThis: 'Delete tile d5 to close the gap in the first wall and run again: the treasure becomes unreachable, and A* has to prove it by searching everything.',
          options: { heuristic: 'manhattan', scale: 100 },
          graph: { directed: false, source: GAME.at('a3'), target: GAME.at('h3'), nodes: GAME.nodes, edges: GAME.edges },
          explain(o) {
            if (!o.found) return '<p>The treasure cannot be reached: A* explored every tile it could get to before giving up. '
              + 'Games often check reachability first to avoid this costly search.</p>';
            return `<p>The hero walks <b>${o.cost} tiles</b>: ${route(o)}.</p>` + savings(o, 'tile')
              + guarantee(o, 'walk');
          },
        },
        {
          id: 'warehouse',
          field: 'Robotics & warehouses',
          title: 'A warehouse robot avoiding a busy aisle',
          summary: 'Robots fetch goods between shelves. The shortest aisle is not the fastest when forklifts are working in it.',
          mapping: [
            ['Vertex', 'a spot on the warehouse floor; shelves are missing spots'],
            ['Edge weight', 'seconds to drive one spot: 1, or 4 in the busy aisle'],
            ['Heuristic', 'Manhattan distance, as if every aisle were free'],
            ['Result', 'the fastest trip; slow aisles only raise the true cost, so the heuristic stays safe'],
          ],
          story: 'The robot at a3 must reach the packing station at h3. The straight middle aisle is shortest, but forklifts slow it down between b3 and d3.',
          key: [['a1 … h5', 'floor spots: column letter, row number'], ['Missing spots', 'shelves in rows 2 and 4'], ['Weight 4', 'busy aisle b3–c3–d3']],
          tryThis: 'Set the two busy edges (b3–c3, c3–d3) back to 1 and run again: the robot takes the middle aisle.',
          options: { heuristic: 'manhattan', scale: 100 },
          graph: { directed: false, source: WAREHOUSE.at('a3'), target: WAREHOUSE.at('h3'), nodes: WAREHOUSE.nodes, edges: WAREHOUSE.edges },
          explain(o) {
            if (!o.found) return '<p>The packing station cannot be reached.</p>';
            const busy = o.path.includes('c3');
            return `<p>Fastest trip: <b>${o.cost} seconds</b> along ${route(o)}.</p>`
              + (busy ? '<p>The robot goes through the busy aisle: the detour would take longer.</p>'
                : '<p>It drives around the busy aisle: the detour is longer in distance but quicker in time.</p>')
              + savings(o, 'spot');
          },
        },
        {
          id: 'maze',
          field: 'Puzzles & mazes',
          title: 'Getting out of a maze',
          summary: 'The corridor that points straight at the exit is a dead end. Does A* fall for it?',
          mapping: [
            ['Vertex', 'a junction or the end of a corridor'],
            ['Edge weight', 'corridor length'],
            ['Heuristic', 'straight-line distance to the exit, ignoring walls'],
            ['Result', 'the shortest way out, even when the heuristic is misleading'],
          ],
          story: 'From the entrance three corridors branch off. The middle one heads straight for the exit and looks perfect to the heuristic, '
            + 'but it ends at a wall just before the exit. The real ways out go around the top or the bottom.',
          key: [['In, Out', 'entrance and exit'], ['A–K', 'junctions'], ['F', 'the dead end next to the exit']],
          tryThis: 'Add an edge from F to Out and run again: the tempting corridor becomes the way out.',
          options: { heuristic: 'euclidean' },
          graph: {
            directed: false, geometric: true, source: 0, target: 12,
            nodes: [['In', 70, 260], ['A', 200, 260], ['B', 200, 100], ['C', 200, 420], ['D', 380, 260], ['E', 540, 260], ['F', 690, 260],
              ['G', 400, 80], ['H', 620, 80], ['I', 780, 140], ['J', 420, 440], ['K', 680, 440], ['Out', 830, 290]],
            edges: [[0, 1], [1, 2], [1, 3], [1, 4], [4, 5], [5, 6], [2, 7], [7, 8], [8, 9], [9, 12], [3, 10], [10, 11], [11, 12]],
          },
          explain(o) {
            if (!o.found) return '<p>There is no way out of this maze.</p>';
            // Only claim the detour if A* really went down the dead end.
            const fooled = o.explored.includes('F') && !o.path.includes('F');
            return `<p>Shortest way out: <b>${route(o)}</b>, length ${o.cost}.</p>`
              + (fooled ? '<p>A* first ran down the corridor towards the exit (D, E, F), because it looked closest. At the dead end its f = g + h values grew, '
                + 'so it went back and tried the other corridors. Unlike greedy search, it does not stop at the first way out it sees: it stops only when '
                + 'no other route could be shorter.</p>' : '')
              + savings(o, 'junction');
          },
        },
        {
          id: 'taxi',
          field: 'Ride hailing & delivery',
          title: 'A taxi in a grid city with roadworks',
          summary: 'In a city of straight streets, the right heuristic is the number of blocks, not the straight line.',
          mapping: [
            ['Vertex', 'a street corner'],
            ['Edge', 'one block, cost 1; streets closed for roadworks are missing'],
            ['Heuristic', 'Manhattan distance: blocks east/west plus blocks north/south'],
            ['Result', 'the shortest drive; a heuristic closer to the true distance means less searching'],
          ],
          story: 'The taxi picks up at a5 and drops off at h3. Roadworks close the crossings between columns d and e except at the top, '
            + 'and two streets near the destination.',
          key: [['a1 … h5', 'corners: avenue letter, street number'], ['Missing edges', 'streets closed for roadworks'], ['Weights', 'one block = 1']],
          tryThis: 'Change the heuristic to "Straight line" and run again: the drive is still 13 blocks, but A* expands more corners, because the straight line underestimates more.',
          options: { heuristic: 'manhattan', scale: 100 },
          graph: { directed: false, source: TAXI.at('a5'), target: TAXI.at('h3'), nodes: TAXI.nodes, edges: TAXI.edges },
          explain(o) {
            if (!o.found) return '<p>The drop-off cannot be reached with these streets closed.</p>';
            return `<p>Shortest drive: <b>${o.cost} blocks</b>: ${route(o)}.</p>` + savings(o, 'corner')
              + (o.heuristic === 'manhattan'
                ? '<p>Manhattan distance is closer to the true distance on a street grid than the straight line, so A* wastes less time on corners that only look promising.</p>'
                : o.heuristic === 'euclidean'
                  ? '<p>The straight line is safe, but on a street grid it underestimates more than Manhattan distance, so A* expands corners it could have skipped.</p>'
                  : guarantee(o, 'drive'));
          },
        },
        {
          id: 'traveltime',
          field: 'Route planning pitfalls',
          title: 'When the heuristic lies: travel time on a motorway',
          summary: 'Measure cost in minutes but estimate in kilometres, and A* can miss the fastest route.',
          mapping: [
            ['Vertex', 'a town or a motorway junction'],
            ['Edge weight', 'driving time in minutes'],
            ['Heuristic', 'straight-line distance in km: wrong units once a road is faster than 1 km per minute'],
            ['Result', 'with an overestimating heuristic, A* is no longer guaranteed the best route'],
          ],
          story: 'From the farm to the city there is a country road through town (8 + 8 minutes) and a longer motorway that is much faster (5 + 3 + 5 minutes). '
            + 'The heuristic counts kilometres, and on the motorway you cover more than a kilometre per minute.',
          key: [['Farm, City', 'start and destination'], ['Town', 'country road through town'], ['On, Off', 'motorway junctions'], ['Weights', 'minutes']],
          tryThis: 'Change the heuristic to "None" (Dijkstra) and run again: the motorway route is found. A heuristic must never overestimate the real cost.',
          options: { heuristic: 'euclidean' },
          graph: {
            directed: false, source: 0, target: 2,
            nodes: [['Farm', 100, 260], ['Town', 450, 190], ['City', 800, 260], ['On', 160, 450], ['Off', 740, 450]],
            edges: [[0, 1, 8], [1, 2, 8], [0, 3, 5], [3, 4, 3], [4, 2, 5]],
          },
          explain(o) {
            if (!o.found) return '<p>The city cannot be reached.</p>';
            let html = `<p>A* returned <b>${route(o)}</b>, taking <b>${o.cost} minutes</b>.</p>`;
            if (!o.optimal) {
              html += `<p>But the fastest route takes <b>${o.optimalCost} minutes</b>. A* never tried it: the motorway junctions are far from the city `
                + 'in kilometres, so the heuristic made them look worse than they are, and A* reached the city by the country road first.</p>';
            } else {
              html += '<p>This is the fastest route.</p>';
            }
            html += o.admissible
              ? '<p>The heuristic never overestimates on this map, so the answer is guaranteed to be optimal.</p>'
              : '<p>The heuristic overestimates the remaining time somewhere on this map (it is not admissible). The fix is to estimate in the same unit as the cost: '
                + 'distance divided by the top speed gives minutes that can never be too high.</p>';
            return html;
          },
        },
      ],
    },
  };
})();
