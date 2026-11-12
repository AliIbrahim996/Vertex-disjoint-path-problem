// Building requests for the C++ core. Shared by the page (app.js) and tests/demos.js, so the tests
// send exactly what the page sends.
(function () {
  'use strict';

  const DEFAULT_SCALE = 50; // canvas units per unit of edge weight

  // An edge weight equal to the edge's length on the canvas, so the straight-line heuristic of A*
  // (distance / scale) never overestimates it.
  function lengthWeight(a, b, scale = DEFAULT_SCALE) {
    return Math.max(1, Math.ceil(Math.hypot(a.x - b.x, a.y - b.y) / scale));
  }

  // A graph in the samples.js format ({ directed, geometric, nodes: [label, x, y], edges: [u, v, w] })
  // as the page edits it: { directed, nodes: [{ label, x, y }], edges: [{ u, v, w }] }.
  function graphFromSample(sample, scale = DEFAULT_SCALE) {
    const nodes = sample.nodes.map(([label, x, y]) => ({ label, x, y }));
    return {
      directed: sample.directed,
      nodes,
      edges: sample.edges.map(([u, v, w]) => ({ u, v, w: sample.geometric ? lengthWeight(nodes[u], nodes[v], scale) : w })),
    };
  }

  // The line-based request text (see the README). `options` may hold a "scale" that replaces the
  // default heuristic scale, because the core uses the last value it reads.
  function requestText({ algorithm, graph, source, target, options = {}, usesGraph = true }) {
    const lines = [
      'algorithm ' + algorithm,
      'directed ' + (graph.directed ? 1 : 0),
      'source ' + source,
      'target ' + target,
      'scale ' + DEFAULT_SCALE,
    ];
    for (const [key, value] of Object.entries(options)) lines.push(key + ' ' + value);
    if (usesGraph) {
      for (const n of graph.nodes) lines.push(`node ${Math.round(n.x)} ${Math.round(n.y)} ${n.label}`);
      for (const e of graph.edges) lines.push(`edge ${e.u} ${e.v} ${e.w}`);
    }
    return lines.join('\n') + '\n';
  }

  window.ALGOLAB_REQUEST = { DEFAULT_SCALE, lengthWeight, graphFromSample, requestText };
})();
