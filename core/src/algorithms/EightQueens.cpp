//! @copyright 2022-2026 Ali Ibrahim. Licensed under the MIT License (see LICENSE).
//! @author Ali Ibrahim
//!
//! N-queens by backtracking: one queen per row, columns tried left to right.
//! The board is drawn as a "board" view whose vertex r * N + c is the square in row r, column c.

#include <cstdlib>

#include "../Common.h"
#include "algolab/Algorithms.h"

namespace algolab
{
	namespace
	{
		const char* const queenSymbol = "\xE2\x99\x9B"; // U+265B black chess queen

		struct QueensRun
		{
			const int n;
			Tracer& tr;
			std::vector<int> column; //!< column of the queen in each row, -1 while empty
			long placements{ 0 };
			long backtracks{ 0 };

			QueensRun(int size, Tracer& tracer) : n(size), tr(tracer), column(size, -1) {}

			int square(int row, int col) const { return row * n + col; }

			//! Chess-style name: files a, b, c... from the left, ranks counted from the bottom.
			std::string name(int row, int col) const
			{
				return std::string(1, static_cast<char>('a' + col)) + std::to_string(n - row);
			}

			//! Row of a queen (above `row`) that attacks (row, col), or -1.
			int attacker(int row, int col) const
			{
				for (int r = 0; r < row; r++)
				{
					if (column[r] == col || std::abs(column[r] - col) == row - r) return r;
				}
				return -1;
			}

			//! Shade every square attacked by the queens placed so far and draw the queens.
			void drawBoard()
			{
				tr.resetStates();
				for (int sq = 0; sq < n * n; sq++) tr.nodeLabel(sq, "");
				for (int r = 0; r < n; r++)
				{
					if (column[r] < 0) continue;
					for (int row = 0; row < n; row++)
					{
						for (int col = 0; col < n; col++)
						{
							if (row == r || col == column[r] || std::abs(col - column[r]) == std::abs(row - r))
							{
								tr.node(square(row, col), NodeState::Visited);
							}
						}
					}
				}
				std::vector<std::string> queens;
				for (int r = 0; r < n; r++)
				{
					if (column[r] < 0) continue;
					tr.node(square(r, column[r]), NodeState::Path);
					tr.nodeLabel(square(r, column[r]), queenSymbol);
					queens.push_back(name(r, column[r]));
				}
				tr.panel("Queens placed", queens);
				tr.panel("Placements tried", { std::to_string(placements) });
				tr.panel("Backtracks", { std::to_string(backtracks) });
			}

			bool place(int row)
			{
				if (row == n)
				{
					return true;
				}
				for (int col = 0; col < n; col++)
				{
					placements++;
					const int blocker = attacker(row, col);
					drawBoard();
					if (blocker >= 0)
					{
						const std::string why = column[blocker] == col ? "same column" : "same diagonal";
						tr.node(square(row, col), NodeState::Cycle);
						tr.node(square(blocker, column[blocker]), NodeState::Cycle);
						tr.step(3, name(row, col) + " is attacked by the queen on " + name(blocker, column[blocker]) + " (" + why + "). Try the next column.");
						continue;
					}

					column[row] = col;
					drawBoard();
					tr.node(square(row, col), NodeState::Current);
					tr.step(4, name(row, col) + " is safe: place a queen there and move on to the next row.");
					if (place(row + 1))
					{
						return true;
					}

					column[row] = -1;
					backtracks++;
					drawBoard();
					tr.node(square(row, col), NodeState::Current);
					tr.step(6, "The rows below cannot be completed with a queen on " + name(row, col) + ": every attempt ran into a dead end. Remove it and try further along this row.");
				}
				drawBoard();
				tr.step(7, "Every square in row " + std::to_string(n - row) + " is either attacked or leads to a dead end. Go back to the row above.");
				return false;
			}
		};
	}

	void runEightQueens(const Request& request, Tracer& tr)
	{
		const int n = request.size;
		Graph board;
		board.directed = false;
		QueensRun run{ n, tr };
		for (int row = 0; row < n; row++)
		{
			for (int col = 0; col < n; col++)
			{
				board.addNode(run.name(row, col), col, row);
			}
		}

		tr.setPseudocode({
			"PlaceQueen(row):",
			"  if every row has a queen: solved",
			"  for each column c of this row:",
			"    if a queen above attacks (row, c): skip c",
			"    place a queen on (row, c)",
			"    if PlaceQueen(next row) succeeds: solved",
			"    remove the queen (backtrack)",
			"  no column leads to a solution: return failure",
			"PlaceQueen(first row) failed: no solution exists" });
		tr.addView(std::to_string(n) + " \xC3\x97 " + std::to_string(n) + " board", board, -1, -1, false, "board");
		tr.nodeLegend("path", "Queen");
		tr.nodeLegend("current", "Queen just placed or removed");
		tr.nodeLegend("cycle", "Conflict");
		tr.nodeLegend("visited", "Attacked square");

		run.drawBoard();
		tr.step(0, "Place " + std::to_string(n) + " queens so that none can capture another. Work row by row from the top, trying columns from left to right.");

		const bool solved = run.place(0);

		tr.result("placements", std::to_string(run.placements));
		tr.result("backtracks", std::to_string(run.backtracks));
		run.drawBoard();
		if (solved)
		{
			std::string squares, columns;
			for (int r = 0; r < n; r++)
			{
				squares += (r ? ", " : "") + run.name(r, run.column[r]);
				columns += (r ? " " : "") + std::to_string(run.column[r] + 1);
			}
			tr.step(1, "Every row has a queen and none attacks another: solved after trying " + std::to_string(run.placements)
				+ " squares and backtracking " + std::to_string(run.backtracks) + " times.");
			tr.setSummary("Solution: " + squares + ". Found after " + std::to_string(run.placements) + " placements and "
				+ std::to_string(run.backtracks) + " backtracks.");
			tr.result("solved", "true");
			tr.result("columns", columns);
		}
		else
		{
			tr.step(8, "Every possibility for the first row has failed: " + std::to_string(n) + " queens cannot be placed on this board.");
			tr.setSummary("No solution exists on a " + std::to_string(n) + " \xC3\x97 " + std::to_string(n) + " board. The search tried "
				+ std::to_string(run.placements) + " squares.");
			tr.result("solved", "false");
		}
	}
}
