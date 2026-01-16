Hamiltonian Cycle Counter (TdZdd)

Build:
  make

Run:
  ./hamilton grid12x12.grh
  make run
  ./hamilton grid4x4.grh --all

Output:
  The program prints the number of Hamiltonian cycles to STDOUT.
  With --all, it prints every Hamiltonian cycle as a vertex sequence.
  It also writes ZDD in DOT format to zdd.dot.

Graphviz:
  dot -Tpng zdd.dot -o zdd.png

Notes:
  - TdZdd is expected at ./tdzdd (relative to this directory).
    If it is elsewhere, override the path:
      make TDZDD_HOME=/path/to/tdzdd
  - The input format is:
      s t
      u1 v1
      u2 v2
      ...
    The first line (s t) is ignored for Hamiltonian cycles.
