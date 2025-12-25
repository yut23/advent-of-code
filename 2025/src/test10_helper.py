#!/usr/bin/env python3
# mypy: disable-error-code=no-untyped-call
import functools
import sys

import sympy

it = iter(sys.stdin)
lineno = sys.argv[1]
n_rows = int(next(it, "0"))
input_mtx = sympy.Matrix(
    [[int(x) for x in next(it).rstrip("\n").split()] for _ in range(n_rows)]
)
actual_mtx = sympy.Matrix(
    [[int(x) for x in next(it).rstrip("\n").split()] for _ in range(n_rows)]
)
expected_mtx, pivots = input_mtx.rref()  # type: ignore[no-untyped-call]
# make fraction-free
for row in range(expected_mtx.rows):
    gcd = functools.reduce(sympy.gcd, expected_mtx.row(row))
    if gcd != 0:
        expected_mtx = expected_mtx.elementary_row_op(row=row, k=1 / gcd)
diff = expected_mtx - actual_mtx
if not diff.is_zero_matrix:
    print(f"\nError on line {lineno}")
    print("input:")
    print(sympy.matrix2numpy(input_mtx, dtype=int))
    print("\nexpected:")
    print(sympy.matrix2numpy(expected_mtx, dtype=int))
    print("\nactual:")
    print(sympy.matrix2numpy(actual_mtx, dtype=int))
    print("\npivots:")
    print(pivots, flush=True)
    sys.exit(1)
