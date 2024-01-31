from __future__ import annotations

import math
from dataclasses import dataclass, field
from functools import cached_property
from numbers import Real
from typing import Any, cast

import numpy as np
import numpy.typing as npt
import z3
from scipy.optimize import check_grad, minimize

np.set_printoptions(linewidth=300)

MIN_X, MAX_X = 200000000000000, 400000000000000
MIN_Y, MAX_Y = MIN_X, MAX_X


IArray = npt.NDArray[np.int64]
DEBUG = True


@dataclass(frozen=True)
class Hailstone:
    px: int
    py: int
    pz: int
    vx: int
    vy: int
    vz: int

    @cached_property
    def pos(self) -> IArray:
        arr = np.array([self.px, self.py, self.pz], dtype=np.int64)
        arr.flags.writeable = False
        return arr

    @cached_property
    def vel(self) -> IArray:
        arr = np.array([self.vx, self.vy, self.vz], dtype=np.int64)
        arr.flags.writeable = False
        return arr


def parse(lines: list[str]) -> list[Hailstone]:
    stones: list[Hailstone] = []
    for line in lines:
        args = [int(x.strip()) for part in line.split(" @ ") for x in part.split(",")]
        stones.append(Hailstone(*args))
    return stones


def check_intersection_xy(a: Hailstone, b: Hailstone) -> tuple[float, float] | None:
    # check if lines are parallel
    if a.vx == 0 and b.vx == 0:
        # lines are both vertical
        return None
    if a.vx != 0 and b.vx != 0 and a.vy / a.vx == b.vy / b.vx:
        # lines are parallel
        return None

    """
    Thanks Mathematica!
    StringRiffle[
     StringReplace[ToString[#,InputForm],{"\""->"","->"->"=("}]<>")"&/@
     Simplify@First@Solve[y-#<>"py"==(#<>"vy"/#<>"vx")(x-#<>"px")&/@{"a.","b."},{x,y}],
    "\n"]
    """
    x = (
        a.py * a.vx * b.vx
        - a.px * a.vy * b.vx
        - a.vx * b.py * b.vx
        + a.vx * b.px * b.vy
    ) / (-(a.vy * b.vx) + a.vx * b.vy)
    y = (
        a.vy * b.py * b.vx
        - a.py * a.vx * b.vy
        + a.px * a.vy * b.vy
        - a.vy * b.px * b.vy
    ) / (a.vy * b.vx - a.vx * b.vy)

    if math.copysign(1.0, x - a.px) != math.copysign(1.0, a.vx) or math.copysign(
        1.0, x - b.px
    ) != math.copysign(1.0, b.vx):
        # intersection occurred in the past for at least one of the hailstones
        return None

    return (x, y)


def part_1(lines: list[str]) -> int:
    stones = parse(lines)

    count = 0
    for i, a in enumerate(stones):
        for b in stones[i + 1 :]:
            result = check_intersection_xy(a, b)
            if result is None:
                continue
            x, y = result
            if MIN_X <= x <= MAX_X and MIN_Y <= y <= MAX_Y:
                count += 1

    return count


FArray = npt.NDArray[np.float64]


def part_2(lines: list[str]) -> int:
    stones = parse(lines)
    sol = part_2_linalg(stones)
    # sol = part_2_z3(stones)
    # sol = part_2_scipy(stones)
    correct_sol = None
    if len(lines) == 5:
        correct_sol = 47
    elif len(lines) == 300:
        correct_sol = 769840447420960
    if correct_sol is not None:
        assert sol == correct_sol, f"{sol} != {correct_sol}"
    return sol


def make_levi_civita() -> IArray:
    eps = np.zeros((3, 3, 3), dtype=int)
    indices = (0, 1, 2)
    for _ in range(3):
        eps[indices] = 1
        eps[indices[::-1]] = -1
        indices = tuple(np.roll(indices, 1))  # type: ignore[assignment]
    return eps


def make_linear_system(
    stones: list[Hailstone], index_pairs: tuple[tuple[int, int], tuple[int, int]]
) -> tuple[IArray, IArray]:
    """Returns (A, b), suitable for np.linalg.solve(A, b)."""
    eps = make_levi_civita()

    assert len(index_pairs) == 2
    assert all(len(set(pair)) == 2 for pair in index_pairs)
    assert set(index_pairs[0]) != set(index_pairs[1])
    A = np.zeros((6, 6), dtype=int)
    b = np.zeros(6, dtype=int)
    p = np.stack([stone.pos for stone in stones])
    v = np.stack([stone.vel for stone in stones])
    for i, (m, n) in enumerate(index_pairs):
        for j in range(3):
            for k in range(3):
                for l in range(3):  # noqa: E741
                    b[i * 3 + j] += eps[j, k, l] * (
                        p[m, k] * v[m, l] - p[n, k] * v[n, l]
                    )
                    A[i * 3 + j, k] += eps[j, k, l] * (v[m, l] - v[n, l])
                    A[i * 3 + j, l + 3] += eps[j, k, l] * (p[m, k] - p[n, k])
    return A, b


def part_2_linalg(stones: list[Hailstone]) -> int:
    index_pairs = ((0, 1), (2, 3))
    A, b = make_linear_system(stones, index_pairs)
    # pm1, pm2, pm3 = stones[index_pairs[0][0]].pos
    # vm1, vm2, vm3 = stones[index_pairs[0][0]].vel
    # pn1, pn2, pn3 = stones[index_pairs[0][1]].pos
    # vn1, vn2, vn3 = stones[index_pairs[0][1]].vel
    if DEBUG:
        print(f"A:\n{A}")
        print(f"b: {b}")
    # assert -b[0] == pm2 * vm3 - pm3 * vm2 - pn2 * vn3 + pn3 * vn2
    # assert np.array_equal(A[1], [vm3 - vn3, 0, vn1 - vm1, pn3 - pm3, 0, pm1 - pn1])
    # assert -b[1] == pm3 * vm1 - pm1 * vm3 - pn3 * vn1 + pn1 * vn3
    # assert b[1] == pm1 * vm3 - pm3 * vm1 - pn1 * vn3 + pn3 * vn1

    # assert np.array_equal(A[0], [0, -vm3 + vn3, vm2 - vn2, 0, pm3 - pn3, -pm2 + pn2])
    # assert np.array_equal(A[1], [vm3 - vn3, 0, -vm1 + vn1, -pm3 + pn3, 0, pm1 - pn1])
    # assert np.array_equal(A[2], [-vm2 + vn2, vm1 - vn1, 0, pm2 - pn2, -pm1 + pn1, 0])
    # assert b[0] == pm3 * vm2 - pm2 * vm3 - pn3 * vn2 + pn2 * vn3
    # assert b[1] == -(pm3 * vm1) + pm1 * vm3 + pn3 * vn1 - pn1 * vn3
    # assert b[2] == pm2 * vm1 - pm1 * vm2 - pn2 * vn1 + pn1 * vn2
    A, b = gauss_elim(A.astype(object), b.astype(object))
    if DEBUG:
        print("log2(A):")
        print(
            np.array(
                [[math.log2(abs(x)) if x != 0 else math.nan for x in row] for row in A]
            )
        )
        print(
            "log2(b):", np.array([math.log2(abs(x)) if x != 0 else math.nan for x in b])
        )
        print("after Gaussian elimination:")
        print(f"A:\n{A}")
        print(f"b: {b}")
    try:
        x = np.linalg.solve(A, b).round().astype(np.int64)
    except (TypeError, OverflowError):
        x = solve_upper_triangular(A, b)
    if DEBUG:
        print(f"x: {x}")
    return cast(int, x[:3].sum())


def solve_upper_triangular(A: IArray, b: IArray) -> IArray:
    b = b.copy()
    x = np.zeros_like(b)
    n = x.shape[0]
    for i in range(n - 1, -1, -1):
        for j in range(i + 1, n):
            b[i] -= A[i, j] * x[j]
        if b.dtype.kind in "iu" or (b.dtype.kind == "O" and isinstance(b[i], int)):
            # b is an integer
            print(f"checking {b[i]} % {A[i, i]}...")
            assert b[i] % A[i, i] == 0
            x[i] = b[i] // A[i, i]
        else:
            # b is not an integer
            x[i] = b[i] / A[i, i]
    return x


def partial_pivot_max(A: IArray | FArray, b: IArray | FArray, i: int) -> bool:
    """Partial pivot at row i, swapping with the row with maximum absolute
    value in that column.

    Return False if no non-zero pivot was found.
    """
    pivot = cast(int, i + np.argmax(np.abs(A[i:, i])))
    if A[pivot, i] == 0:
        # entire column is zero
        print(f"warning: column {i} is all zeros")
        return False
    # swap rows
    if pivot != i:
        if DEBUG:
            print(f"swapping rows {i} and {pivot}")
        A[[pivot, i]] = A[[i, pivot]]
        b[[pivot, i]] = b[[i, pivot]]
    return True


def partial_pivot_first(A: IArray | FArray, b: IArray | FArray, i: int) -> bool:
    """Partial pivot at row i, swapping with the first row with a non-zero value.

    Return False if no non-zero pivot was found.
    """
    pivot = next((j for j in range(i, A.shape[0]) if A[j, i] != 0), None)
    if pivot is None:
        # entire column is zero
        print(f"warning: column {i} is all zeros")
        return False
    # swap rows
    if pivot != i:
        if DEBUG:
            print(f"swapping rows {i} and {pivot}")
        A[[pivot, i]] = A[[i, pivot]]
        b[[pivot, i]] = b[[i, pivot]]
    return True


# the following functions are based on Turner 1995, "A simplified fraction-free
# integer Gauss elimination algorithm" (https://apps.dtic.mil/sti/citations/ADA313755)


def gauss_elim(ai: IArray | FArray, bi: IArray | FArray) -> tuple[FArray, FArray]:
    """Algorithm 1 from Turner 1995, plus partial pivoting."""
    a = ai.astype(np.float64)
    b = bi.astype(np.float64)
    n = a.shape[0]
    for i in range(n - 1):
        if not partial_pivot_max(a, b, i):
            continue
        for j in range(i + 1, n):
            m = a[j, i] / a[i, i]
            a[j, i] = 0
            b[j] -= m * b[i]
            a[j, i + 1 :] -= m * a[i, i + 1 :]
    return a, b


def gauss_elim_gcd(a: IArray, b: IArray) -> tuple[IArray, IArray]:
    """Algorithm 5 from Turner 1995, plus partial pivoting."""
    n = a.shape[0]
    for i in range(n - 1):
        if not partial_pivot_first(a, b, i):
            continue
        for j in range(i + 1, n):
            p = math.gcd(a[i, i], a[j, i])
            alpha = a[i, i] // p
            gamma = a[j, i] // p
            b[j] = alpha * b[j] - gamma * b[i]
            for k in range(i + 1, n):
                a[j, k] = alpha * a[j, k] - gamma * a[i, k]
            a[j, i] = 0
    return a, b


def gauss_elim_ff(a: IArray, b: IArray) -> tuple[IArray, IArray]:
    """Algorithm 6 from Turner 1995, plus partial pivoting."""
    n = a.shape[0]
    for i in range(n - 1):
        if not partial_pivot_first(a, b, i):
            continue
        for j in range(i + 1, n):
            b[j] = a[i, i] * b[j] - a[j, i] * b[i]
            for k in range(i + 1, n):
                a[j, k] = a[i, i] * a[j, k] - a[j, i] * a[i, k]
            a[j, i] = 0
        if i >= 1:  # removal of common factors
            for j in range(i + 1, n):
                b[j] //= a[i - 1, i - 1]
                for k in range(i + 1, n):
                    a[j, k] //= a[i - 1, i - 1]
    return a, b


def gauss_elim_ff_gcd(a: IArray, b: IArray) -> tuple[IArray, IArray]:
    """Algorithm 6 from Turner 1995, plus partial pivoting and full-row GCD reduction."""
    n = a.shape[0]
    for i in range(n):
        if not partial_pivot_first(a, b, i):
            continue
        for j in range(i + 1, n):
            b[j] = a[i, i] * b[j] - a[j, i] * b[i]
            for k in range(i + 1, n):
                a[j, k] = a[i, i] * a[j, k] - a[j, i] * a[i, k]
            a[j, i] = 0
        if i >= 1:  # removal of common factors
            for j in range(i + 1, n):
                b[j] //= a[i - 1, i - 1]
                for k in range(i + 1, n):
                    a[j, k] //= a[i - 1, i - 1]
        p = math.gcd(abs(b[i]), *(abs(x) for x in a[i, i:] if x != 0))
        if p > 1:
            a[i] //= p
            b[i] //= p
            if DEBUG:
                print(f"reduced row {i} by a common factor of 2^{math.log2(p)} ({p})")

    return a, b


def bareiss_algorithm(A: IArray, b: IArray) -> tuple[IArray, IArray]:
    M = np.hstack([A, b[:, None]])
    n = M.shape[0]

    for k in range(n - 1):
        # if k > 0 and M[k - 1, k - 1] == 0:
        #     for i in range(k, n):
        #         if M[i, k - 1] == 0:
        #             continue
        #         # swap rows k-1 and i
        #         M[[k - 1, i]] = M[[i, k - 1]]
        if not partial_pivot_first(M[:, :n], M[:, n], k - 1):
            continue
        for i in range(k + 1, n):
            for j in range(k + 1, n):
                numer = M[i, j] * M[k, k] - M[i, k] * M[k, j]
                if k > 0:
                    denom = M[k - 1, k - 1]
                else:
                    denom = 1
                if numer != 0:
                    assert denom != 0
                    assert numer % denom == 0
                    M[i, j] = numer // denom
                else:
                    M[i, j] = 0

    # if len(swaps) % 2 == 1:
    #     M[:] *= -1
    print(M)
    A[:] = M[:, :n]
    b[:] = M[:, n]
    return A, b


def make_equations_z3(stones: list[Hailstone]) -> tuple[list[Any], list[Any]]:
    equations: list[Any] = []
    rock_pos = [z3.Int(f"pr{coord}") for coord in "xyz"]
    rock_vel = [z3.Int(f"vr{coord}") for coord in "xyz"]
    variables = rock_pos + rock_vel
    stone_num = 0
    while len(variables) > len(equations):
        t = z3.Int(f"t{stone_num}")
        variables.append(t)
        stone = stones[stone_num]
        for i in range(3):
            equations.append(
                t * (stone.vel[i] - rock_vel[i]) + (stone.pos[i] - rock_pos[i]) == 0
            )
            if len(variables) == len(equations):
                break
        stone_num += 1
    return equations, variables


def part_2_z3(stones: list[Hailstone]) -> int:
    equations, variables = make_equations_z3(stones)

    s = z3.Solver()
    s.add(*equations)
    for t in variables[6:]:
        s.add(t >= 0)
    s.check()
    m = s.model()
    return sum(m[var].as_long() for var in variables[:3])


@dataclass(frozen=True)
class Dual:
    """Representation of a dual number.

    The dual numbers are an extension of the reals, with an additional element
    ε, with the property ε^2 = 0.
    """

    val: float
    eps: FArray = field(default_factory=lambda: np.asarray(0, dtype=np.float64))

    def __str__(self) -> str:
        return f"{self.val} + {self.eps}ε"

    def __repr__(self) -> str:
        return f"Dual({self.val!r}, {self.eps!r})"

    def __add__(self, other: Any) -> Dual:
        if isinstance(other, Dual):
            # dual + dual
            return Dual(self.val + other.val, self.eps + other.eps)
        if isinstance(other, Real):
            # dual + real
            return self.__add__(Dual(float(other)))
        return NotImplemented

    def __radd__(self, other: Any) -> Dual:
        # addition is commutative
        return self.__add__(other)

    def __sub__(self, other: Any) -> Dual:
        if isinstance(other, Dual):
            # dual - dual
            return Dual(self.val - other.val, self.eps - other.eps)
        if isinstance(other, Real):
            # dual - real
            return self.__sub__(Dual(float(other)))
        return NotImplemented

    def __rsub__(self, other: Any) -> Dual:
        if isinstance(other, Real):
            # real - dual
            return Dual(float(other)).__sub__(self)
        return NotImplemented

    def __mul__(self, other: Any) -> Dual:
        if isinstance(other, Dual):
            # dual * dual
            return Dual(
                self.val * other.val, self.eps * other.val + self.val * other.eps
            )
        if isinstance(other, Real):
            # dual * real
            return self.__mul__(Dual(float(other)))
        return NotImplemented

    def __rmul__(self, other: Any) -> Dual:
        # multiplication is commutative
        return self.__mul__(other)

    def __truediv__(self, other: Any) -> Dual:
        if isinstance(other, Dual):
            # dual / dual
            return Dual(
                self.val / other.val,
                (self.eps * other.val - self.val * other.eps) / other.val**2,
            )
        if isinstance(other, Real):
            # dual / real
            return self.__truediv__(Dual(float(other)))
        return NotImplemented

    def __rtruediv__(self, other: Any) -> Dual:
        if isinstance(other, Dual):
            assert False, "should not be here"
            return Dual.__truediv__(other, self)
        if isinstance(other, Real):
            # real / dual
            return Dual(float(other)).__truediv__(self)
        return NotImplemented

    def __pow__(self, other: Any) -> Dual:
        if isinstance(other, Dual):
            # dual ** dual
            return Dual(
                self.val**other.val,
                self.val ** (other.val - 1)
                * (other.val * self.eps + self.val * math.log(self.val) * other.eps),
            )
        if isinstance(other, Real):
            # dual ** real
            return Dual(
                self.val ** float(other),
                self.val ** (float(other) - 1) * float(other) * self.eps,
            )
        return NotImplemented

    def __rpow__(self, other: Any) -> Dual:
        if isinstance(other, Dual):
            assert False, "should not be here"
            return Dual.__pow__(other, self)
        if isinstance(other, Real):
            # real ** dual
            return Dual(float(other)).__pow__(self)
        return NotImplemented

    def __neg__(self) -> Dual:
        return Dual(-self.val, -self.eps)

    def __pos__(self) -> Dual:
        return self


def closest_approach(pv1: FArray, p2: FArray, v2: FArray) -> tuple[float, FArray]:
    derivs = np.eye(6)
    x = Dual(p2[0] - pv1[0], -derivs[0])
    y = Dual(p2[1] - pv1[1], -derivs[1])
    z = Dual(p2[2] - pv1[2], -derivs[2])

    vx = Dual(v2[0] - pv1[3], -derivs[3])
    vy = Dual(v2[1] - pv1[4], -derivs[4])
    vz = Dual(v2[2] - pv1[5], -derivs[5])

    numer = x * vx + y * vy + z * vz
    denom = vx * vx + vy * vy + vz * vz

    t = -numer / denom
    if t.val < 0:
        t = 0.0  # type: ignore[assignment]

    sq_dist = (x + vx * t) ** 2 + (y + vy * t) ** 2 + (z + vz * t) ** 2
    # dist = sq_dist
    return sq_dist.val, sq_dist.eps


def func(x, stones):
    value = 0.0
    jac = np.zeros(6)
    for stone in stones:
        v, j = closest_approach(x, stone.pos, stone.vel)
        value += v
        jac += j
    return value, jac


def part_2_scipy(stones: list[Hailstone]) -> int:
    # try minimizing the closest approach
    stone_count = 3
    while stone_count < 50:
        sol = minimize(func, np.zeros(6), (stones[:stone_count],), jac=True)
        if np.sum(np.abs(sol.x - sol.x.round())) < 0.01:
            break
        stone_count += 1

    x = sol.x.round().astype(np.int64)
    print(x)
    return cast(int, x[:3].sum())


def check_jac(x: FArray) -> None:
    pos = np.array([19, 13, 30], dtype=np.int64)
    vel = np.array([-2, 1, -2], dtype=np.int64)
    err = check_grad(
        lambda *args: closest_approach(*args)[0],
        lambda *args: closest_approach(*args)[1],
        x,
        pos,
        vel,
        seed=12345,
    )
    assert err <= 1e-5, f"error too large: {err}"


check_jac(-10 * np.ones(6))
check_jac(np.array([10, 10, 10, -4, 5, 1]))
