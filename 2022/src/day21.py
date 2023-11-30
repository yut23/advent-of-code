from __future__ import annotations

import operator
from collections import deque
from dataclasses import dataclass
from numbers import Real


@dataclass
class Expression:
    def __add__(self, other: Expression) -> Expression:
        return BinaryOp("+", self, other)

    def __radd__(self, other: Expression) -> Expression:
        return BinaryOp("+", other, self)

    def __sub__(self, other: Expression) -> Expression:
        return BinaryOp("-", self, other)

    def __rsub__(self, other: Expression) -> Expression:
        return BinaryOp("-", other, self)

    def __mul__(self, other: Expression) -> Expression:
        return BinaryOp("*", self, other)

    def __rmul__(self, other: Expression) -> Expression:
        return BinaryOp("*", other, self)

    def __truediv__(self, other: Expression) -> Expression:
        return BinaryOp("/", self, other)

    def __rtruediv__(self, other: Expression) -> Expression:
        return BinaryOp("/", other, self)

    def equate(self, value: Real) -> None:
        raise NotImplementedError


@dataclass
class Integer(Expression):
    value: Real

    def __add__(self, other: Expression) -> Expression:
        if not isinstance(other, Integer):
            return super().__add__(other)
        return Integer(self.value + other.value)

    def __sub__(self, other: Expression) -> Expression:
        if not isinstance(other, Integer):
            return super().__sub__(other)
        return Integer(self.value - other.value)

    def __mul__(self, other: Expression) -> Expression:
        if not isinstance(other, Integer):
            return super().__mul__(other)
        return Integer(self.value * other.value)

    def __truediv__(self, other: Expression) -> Expression:
        if not isinstance(other, Integer):
            return super().__truediv__(other)
        return Integer(self.value / other.value)

    def equate(self, value: Real) -> None:
        assert self.value == value


@dataclass
class Unknown(Expression):
    name: str
    value: Real | None = None

    def equate(self, value: Real) -> None:
        self.value = value


@dataclass
class BinaryOp(Expression):
    operator: str
    lhs: Real | Expression
    rhs: Real | Expression

    def equate(self, value: Real) -> None:
        # pylint: disable=no-else-return, too-many-return-statements
        lhs = self.lhs
        rhs = self.rhs
        assert (isinstance(lhs, Real) and isinstance(rhs, Expression)) or (
            isinstance(rhs, Real) and isinstance(lhs, Expression)
        )
        if isinstance(lhs, Expression):
            assert isinstance(rhs, Real)
            match self.operator:
                case "+":
                    # value = lhs + rhs
                    # lhs = value - rhs
                    result = value - rhs
                case "-":
                    # value = lhs - rhs
                    # lhs = value + rhs
                    result = value + rhs
                case "*":
                    # value = lhs * rhs
                    # lhs = value / rhs
                    result = value / rhs
                case "/":
                    # value = lhs / rhs
                    # lhs = value * rhs
                    result = value * rhs
            lhs.equate(result)
        elif isinstance(rhs, Expression):
            assert isinstance(lhs, Real)
            match self.operator:
                case "+":
                    # value = lhs + rhs
                    # rhs = value - lhs
                    result = value - lhs
                case "-":
                    # value = lhs - rhs
                    # rhs + value = lhs
                    # rhs = lhs - value
                    result = lhs - value
                case "*":
                    # value = lhs * rhs
                    # rhs = value / lhs
                    result = value / lhs
                case "/":
                    # value = lhs / rhs
                    # rhs * value = lhs
                    # rhs = lhs / value
                    result = lhs / value
            rhs.equate(result)


def equate_expression(lhs: Real | Expression, rhs: Real | Expression) -> bool:
    if isinstance(lhs, Expression):
        assert isinstance(rhs, Real)
        expr = lhs
        value = rhs
    elif isinstance(rhs, Expression):
        assert isinstance(lhs, Real)
        expr = rhs
        value = lhs
    else:
        return lhs == rhs

    # resolve the expression
    expr.equate(value)
    return True


@dataclass
class Monkey:
    name: str
    number: Real | Expression | None = None
    lhs: str = ""
    rhs: str = ""
    op: str = ""

    def evaluate(self, numbers: dict[str, Real | Expression]) -> None:
        if self.number is not None:
            return
        if self.lhs not in numbers or self.rhs not in numbers:
            return
        # both operands are present
        print(f"evaluating monkey {self.name}: {self.lhs} {self.op} {self.rhs}")
        operation = {
            "+": operator.add,
            "-": operator.sub,
            "*": operator.mul,
            "/": operator.truediv,
            "=": equate_expression,
        }[self.op]
        self.number = operation(numbers[self.lhs], numbers[self.rhs])


def parse(lines: list[str]) -> deque[Monkey]:
    pending_monkeys: deque[Monkey] = deque()
    for line in lines:
        name, _, rest = line.partition(": ")
        if rest.isdigit():
            pending_monkeys.append(Monkey(name, number=int(rest)))  # type: ignore[arg-type]
        else:
            lhs, op, rhs = rest.split()
            pending_monkeys.append(Monkey(name, lhs=lhs, op=op, rhs=rhs))
    return pending_monkeys


def part_1(lines: list[str]) -> Real:
    pending_monkeys = parse(lines)

    numbers: dict[str, Real | Expression] = {}
    while pending_monkeys:
        monkey = pending_monkeys.popleft()
        # if monkey.name == "root":
        #     monkey.op = "="
        # if monkey.name == "humn":
        #     monkey.number = 301  # type: ignore[assignment]
        if monkey.number is None:
            monkey.evaluate(numbers)
        if monkey.number is not None:
            print(f"got value for monkey {monkey.name}: {monkey.number}")
            numbers[monkey.name] = monkey.number
        else:
            pending_monkeys.append(monkey)

    assert isinstance(numbers["root"], (bool, Real))
    return numbers["root"]


def part_2(lines: list[str]) -> Real:
    pending_monkeys = parse(lines)

    numbers: dict[str, Real | Expression] = {}
    humn = Unknown("humn")
    while pending_monkeys:
        monkey = pending_monkeys.popleft()
        if monkey.name == "root":
            monkey.op = "="
        if monkey.name == "humn":
            monkey.number = humn
        if monkey.number is None:
            monkey.evaluate(numbers)
        if monkey.number is not None:
            numbers[monkey.name] = monkey.number
        else:
            pending_monkeys.append(monkey)

    assert humn.value is not None
    return humn.value
