// Example graphs. This file is data only: every algorithm runs in the C++ core.
//
// nodes: [label, x, y] on a 900 x 520 canvas      edges: [from, to, weight]
// "geometric: true" derives each weight from the edge's length on the canvas, which makes the
// straight-line heuristic of A* admissible.
window.ALGOLAB_SAMPLES = [
  {
    id: 'basic',
    name: 'Small undirected graph',
    directed: false,
    source: 0, target: 7,
    nodes: [['A', 110, 260], ['B', 270, 120], ['C', 270, 400], ['D', 450, 120], ['E', 450, 260], ['F', 450, 400], ['G', 630, 190], ['H', 790, 280]],
    edges: [[0, 1, 1], [0, 2, 1], [1, 3, 1], [1, 4, 1], [2, 4, 1], [2, 5, 1], [3, 6, 1], [4, 6, 1], [5, 7, 1], [6, 7, 1]],
  },
  {
    id: 'cycle',
    name: 'Directed graph with a cycle',
    directed: true,
    source: 0, target: -1,
    nodes: [['A', 120, 150], ['B', 320, 100], ['C', 530, 140], ['D', 640, 320], ['E', 440, 410], ['F', 240, 350], ['G', 790, 170]],
    edges: [[0, 1, 1], [1, 2, 1], [2, 3, 1], [3, 4, 1], [4, 5, 1], [5, 1, 1], [2, 6, 1], [0, 5, 1]],
  },
  {
    id: 'dag',
    name: 'Directed acyclic graph',
    directed: true,
    source: 0, target: 6,
    nodes: [['A', 100, 260], ['B', 270, 130], ['C', 270, 390], ['D', 460, 200], ['E', 460, 420], ['F', 650, 300], ['G', 810, 300]],
    edges: [[0, 1, 1], [0, 2, 1], [1, 3, 1], [2, 3, 1], [2, 4, 1], [3, 5, 1], [4, 5, 1], [5, 6, 1]],
  },
  {
    id: 'map',
    name: 'Road map (weights = distances)',
    directed: false,
    geometric: true,
    source: 0, target: 8,
    nodes: [['A', 90, 280], ['B', 240, 130], ['C', 250, 410], ['D', 400, 260], ['E', 420, 90], ['F', 560, 410], ['G', 590, 190], ['H', 740, 320], ['I', 830, 140]],
    edges: [[0, 1], [0, 2], [0, 3], [1, 4], [1, 3], [2, 3], [2, 5], [3, 6], [3, 5], [4, 6], [5, 7], [6, 7], [6, 8], [7, 8]],
  },
  {
    id: 'trap',
    name: 'Greedy trap (tempting but expensive road)',
    directed: false,
    source: 0, target: 3,
    nodes: [['A', 120, 230], ['B', 320, 230], ['C', 520, 230], ['D', 760, 230], ['E', 300, 400], ['F', 560, 410]],
    edges: [[0, 1, 4], [1, 2, 4], [2, 3, 14], [0, 4, 4], [4, 5, 6], [5, 3, 6]],
  },
  {
    id: 'flow',
    name: 'Flow network (textbook example)',
    directed: true,
    source: 0, target: 5,
    nodes: [['S', 100, 260], ['A', 310, 120], ['B', 310, 400], ['C', 580, 120], ['D', 580, 400], ['T', 790, 260]],
    edges: [[0, 1, 16], [0, 2, 13], [1, 3, 12], [2, 1, 4], [2, 4, 14], [3, 2, 9], [3, 5, 20], [4, 3, 7], [4, 5, 4]],
  },
  {
    id: 'cancel',
    name: 'Flow that must be rerouted (try DFS)',
    directed: true,
    source: 0, target: 3,
    nodes: [['S', 140, 260], ['A', 440, 110], ['B', 440, 410], ['T', 740, 260]],
    edges: [[0, 1, 1], [0, 2, 1], [1, 2, 1], [1, 3, 1], [2, 3, 1]],
  },
  {
    id: 'hub',
    name: 'Three roads, one shared junction',
    directed: false,
    source: 0, target: 9,
    nodes: [['S', 90, 260], ['A', 250, 120], ['B', 250, 260], ['C', 250, 410], ['D', 450, 190], ['E', 450, 410], ['F', 650, 110], ['G', 650, 260], ['H', 650, 410], ['T', 820, 260]],
    edges: [[0, 1, 1], [0, 2, 1], [0, 3, 1], [1, 4, 1], [2, 4, 1], [3, 5, 1], [4, 6, 1], [4, 7, 1], [5, 8, 1], [6, 9, 1], [7, 9, 1], [8, 9, 1]],
  },
  {
    id: 'grid',
    name: '4 x 4 grid',
    directed: false,
    geometric: true,
    source: 0, target: 15,
    nodes: Array.from({ length: 16 }, (_, i) => [String.fromCharCode(65 + i), 180 + (i % 4) * 180, 70 + Math.floor(i / 4) * 125]),
    edges: Array.from({ length: 16 }, (_, i) => i).flatMap(i => [
      ...(i % 4 < 3 ? [[i, i + 1]] : []),
      ...(i < 12 ? [[i, i + 4]] : []),
    ]),
  },
];

// The example that shows each algorithm at its most instructive.
window.ALGOLAB_DEFAULT_SAMPLE = {
  bfs: 'basic', dfs: 'basic', cycle: 'cycle', dijkstra: 'map', astar: 'map', greedy: 'trap', maxflow: 'flow', vdp: 'hub',
};
