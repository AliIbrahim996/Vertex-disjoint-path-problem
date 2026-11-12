# AlgoLab

Draw a graph, pick an algorithm, and watch it run one step at a time. Every step highlights the
vertices and edges involved, the pseudocode line being executed, the queue / stack / open set, and
a sentence explaining what just happened and why.

The algorithms are written in **C++17** and compiled to **WebAssembly**, so the tool runs in any
browser with nothing to install. The page is only an editor and a player: it sends the graph to
the C++ core and plays back the steps the core returns.

This repository started as a solver for the vertex-disjoint path problem. That solver is still
here, now as one of ten simulations.

## Algorithms

| Algorithm | What the simulation shows |
| --- | --- |
| Breadth-first search | the queue, distance layers, the BFS tree, the fewest-edges path |
| Depth-first search | the call stack, discovery/finish times, tree / back / forward / cross edges |
| Cycle detection | white-grey-black colouring, the cycle found, or a topological order if there is none |
| Dijkstra | the priority queue, each relaxation, the shortest-path tree |
| A\* | `g + h = f` on every vertex, and whether the heuristic is admissible on your graph |
| Greedy best-first (heuristic search) | how following `h` alone can miss the optimum, with the true optimum for comparison |
| Maximum flow | augmenting paths (BFS, DFS or widest path), bottlenecks, rerouted flow, the minimum cut |
| Vertex-disjoint paths | the vertex-splitting reduction to max-flow, then the resulting paths on the original graph |
| Eight queens (N-queens, 3–8) | backtracking on a chessboard: attacked squares, conflicts, and every queen that gets taken back |
| Travelling salesman | exact branch and bound (with pruning by a lower bound), nearest neighbour, and 2-opt repair, compared with the optimum |

The heuristics of A\* and greedy search are measured on the canvas (straight-line or Manhattan
distance to the target), so dragging a vertex changes the heuristic. "Weights = lengths" sets every
edge weight to its length on the canvas, which makes the straight-line heuristic admissible.

## Using it

1. Choose an algorithm. A fitting example graph is loaded.
2. Change the graph with the tools above the canvas: move, add vertex, add edge, delete. Click an
   edge to edit its weight or capacity.
3. Choose the source and target, then press **Run simulation**.
4. Step with the transport buttons, the slider, or the keys `←` `→` (step) and `space` (play/pause).

## Building

You need [Emscripten](https://emscripten.org/docs/getting_started/downloads.html) and CMake 3.16+.

```bash
emcmake cmake -S . -B build/wasm
cmake --build build/wasm
ctest --test-dir build/wasm --output-on-failure
```

This writes `web/wasm/algolab.js` (the WebAssembly module is embedded in that one file). Then serve
the `web` folder and open http://localhost:8000:

```bash
python -m http.server 8000 --directory web
```

The core has no dependencies and also builds natively, which is the quickest way to run the tests
while working on an algorithm:

```bash
cmake -S . -B build/native
cmake --build build/native
ctest --test-dir build/native --output-on-failure
```

The workflow in `.github/workflows/build.yml` builds, tests, and publishes `web/` to GitHub Pages
on every push to `master` (enable it once under *Settings → Pages → Source: GitHub Actions*).

## Layout

```
core/include/algolab/   public headers: Graph, Tracer, FlowNetwork, Algorithms
core/src/               implementation; one file per algorithm in core/src/algorithms/
wasm/api.cpp            the two functions exported to JavaScript
tests/tests.cpp         dependency-free tests of the algorithm results
web/                    the page: index.html, style.css, js/app.js, js/samples.js
```

### How the two halves talk

The page calls two exported functions:

- `algolab_catalog()` returns the list of algorithms as JSON: name, description, complexity,
  whether a source / target is needed, and the options to offer.
- `algolab_run(request)` takes a request in a small line-based format and returns the trace as JSON.

```
algorithm maxflow
directed 1
source 0
target 3
strategy bfs
node 140 260 S
node 440 110 A
node 440 410 B
node 740 260 T
edge 0 1 3
edge 0 2 2
edge 1 3 2
edge 2 3 3
```

A trace contains the pseudocode, one or more *views* (graphs to draw) and a list of *steps*. Each
step is a full snapshot: the state of every vertex and edge, their labels, the side-panel lists,
the pseudocode line and the explanation. Full snapshots are what make stepping backwards trivial.

### Adding an algorithm

1. Write `void runMyAlgorithm(const Request&, Tracer&)` in `core/src/algorithms/`. Describe what
   happens through the tracer: `tracer.node(v, NodeState::Current)`, `tracer.edge(e, EdgeState::Tree)`,
   `tracer.panel("Queue", items)`, then `tracer.step(line, "explanation")` to take a snapshot.
2. Declare it in `Algorithms.h` and add an entry to the catalogue in `core/src/Registry.cpp`.
3. Add the source file to `CMakeLists.txt` and a test to `tests/tests.cpp`.

The page picks the new algorithm up from the catalogue; no JavaScript changes are needed.
