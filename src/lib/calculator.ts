export type CalcResult =
  | { ok: true; value: number; expression: string }
  | { ok: false; error: string };

type Token =
  | { type: "num"; value: number }
  | { type: "op"; value: string };

function tokenize(input: string): Token[] | null {
  const tokens: Token[] = [];
  let i = 0;
  while (i < input.length) {
    const ch = input[i];
    if (ch === " ") { i++; continue; }

    if (ch >= "0" && ch <= "9" || ch === ".") {
      let numStr = "";
      while (i < input.length && ((input[i] >= "0" && input[i] <= "9") || input[i] === ".")) {
        numStr += input[i];
        i++;
      }
      const value = parseFloat(numStr);
      if (isNaN(value)) return null;
      tokens.push({ type: "num", value });
      continue;
    }

    if ("+-*/^()".includes(ch)) {
      tokens.push({ type: "op", value: ch });
      i++;
      continue;
    }

    return null;
  }
  return tokens;
}

function precedence(op: string): number {
  if (op === "+" || op === "-") return 1;
  if (op === "*" || op === "/") return 2;
  if (op === "^") return 3;
  return 0;
}

function applyOp(op: string, a: number, b: number): number {
  switch (op) {
    case "+": return a + b;
    case "-": return a - b;
    case "*": return a * b;
    case "/": return a / b;
    case "^": return Math.pow(a, b);
    default: return NaN;
  }
}

function shuntingYard(tokens: Token[]): Token[] | null {
  const output: Token[] = [];
  const ops: string[] = [];

  for (const t of tokens) {
    if (t.type === "num") {
      output.push(t);
    } else {
      const op = t.value;
      if (op === "(") {
        ops.push(op);
      } else if (op === ")") {
        while (ops.length > 0 && ops[ops.length - 1] !== "(") {
          output.push({ type: "op", value: ops.pop()! });
        }
        if (ops.length === 0) return null;
        ops.pop();
      } else {
        while (
          ops.length > 0 &&
          ops[ops.length - 1] !== "(" &&
          precedence(ops[ops.length - 1]) >= precedence(op)
        ) {
          output.push({ type: "op", value: ops.pop()! });
        }
        ops.push(op);
      }
    }
  }

  while (ops.length > 0) {
    const op = ops.pop()!;
    if (op === "(" || op === ")") return null;
    output.push({ type: "op", value: op });
  }

  return output;
}

function evalRpn(rpn: Token[]): number | null {
  const stack: number[] = [];

  for (const t of rpn) {
    if (t.type === "num") {
      stack.push(t.value);
    } else {
      if (stack.length < 2) return null;
      const b = stack.pop()!;
      const a = stack.pop()!;
      const result = applyOp(t.value, a, b);
      if (isNaN(result) || !isFinite(result)) return null;
      stack.push(result);
    }
  }

  if (stack.length !== 1) return null;
  return stack[0];
}

function formatNumber(n: number): string {
  if (Number.isInteger(n)) return n.toString();
  const rounded = Math.round(n * 1e10) / 1e10;
  return rounded.toString();
}

export function tryEvaluateArithmetic(input: string): CalcResult | null {
  const trimmed = input.trim();
  if (!trimmed) return null;

  if (!/^[0-9.+\-*/^() ]+$/.test(trimmed)) {
    return null;
  }

  const tokens = tokenize(trimmed);
  if (!tokens || tokens.length === 0) return null;

  const rpn = shuntingYard(tokens);
  if (!rpn) return null;

  const result = evalRpn(rpn);
  if (result === null) return null;

  if (!isFinite(result)) {
    return { ok: false, error: "Division par zero impossible !" };
  }

  return { ok: true, value: result, expression: trimmed };
}

export function formatCalcResult(result: CalcResult): string {
  if (!result.ok) return `Erreur: ${result.error}`;
  return `= ${formatNumber(result.value)}`;
}
