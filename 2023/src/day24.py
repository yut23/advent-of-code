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

MIN_X, MAX_X = 200000000000000, 400000000000000
MIN_Y, MAX_Y = MIN_X, MAX_X


@dataclass(frozen=True)
class Hailstone:
    px: int
    py: int
    pz: int
    vx: int
    vy: int
    vz: int

    @cached_property
    def pos(self) -> npt.NDArray[np.int64]:
        arr = np.array([self.px, self.py, self.pz], dtype=np.int64)
        arr.flags.writeable = False
        return arr

    @cached_property
    def vel(self) -> npt.NDArray[np.int64]:
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


def make_equations(stones: list[Hailstone]) -> tuple[list[Any], list[Any]]:
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


FArray = npt.NDArray[np.float64]


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


def part_2(lines: list[str]) -> int:
    stones = parse(lines)
    # return part_2_z3(stones)
    return part_2_scipy(stones)


def part_2_z3(stones: list[Hailstone]) -> int:
    equations, variables = make_equations(stones)

    s = z3.Solver()
    s.add(*equations)
    for t in variables[6:]:
        s.add(t >= 0)
    s.check()
    m = s.model()
    return sum(m[var].as_long() for var in variables[:3])


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
