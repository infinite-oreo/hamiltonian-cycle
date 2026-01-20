# HAMILTONIAN CYCLE COUNTER

## TDZDD-BASED COUNTING TOOL

This project counts Hamiltonian cycles using TDZDD and optionally prints
each cycle. It also outputs a ZDD in DOT format for visualization.

## BUILD

```sh
make
```

## RUN

```sh
./hamilton grid12x12.grh
make run
./hamilton grid4x4.grh --all
```

## OUTPUT

The program prints the number of Hamiltonian cycles to STDOUT.
With `--all`, it prints every Hamiltonian cycle as a vertex sequence.
It also writes ZDD in DOT format to `zdd.dot`.

## GRAPHVIZ

```sh
dot -Tpng zdd.dot -o zdd.png
```

## NOTES

- TdZdd is expected at `./tdzdd` (relative to this directory).
  If it is elsewhere, override the path:

  ```sh
  make TDZDD_HOME=/path/to/tdzdd
  ```

- The input format is:

  ```
  s t
  u1 v1
  u2 v2
  ...
  ```

  The first line (`s t`) is ignored for Hamiltonian cycles.
