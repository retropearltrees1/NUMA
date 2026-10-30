export interface KnowledgeEntry {
  keywords: string[];
  title: string;
  content: string;
  category: "math" | "physique" | "chimie" | "info";
}

export const knowledgeBase: KnowledgeEntry[] = [
  {
    category: "math",
    title: "Dérivées usuelles",
    keywords: ["derivee", "derivé", "derivées", "derivees", "dérivé", "dérivées"],
    content:
      "Dérivées usuelles:\n• (xⁿ)' = n·xⁿ⁻¹\n• (eˣ)' = eˣ\n• (ln x)' = 1/x\n• (sin x)' = cos x\n• (cos x)' = −sin x\n• (tan x)' = 1/cos²x\n• (u·v)' = u'·v + u·v'\n• (u/v)' = (u'·v − u·v') / v²\n• (uⁿ)' = n·uⁿ⁻¹·u'",
  },
  {
    category: "math",
    title: "Primitives & Intégrales",
    keywords: ["primitive", "integrale", "intégrale", "intégrales", "intégral"],
    content:
      "Primitives usuelles:\n• xⁿ → xⁿ⁺¹ / (n+1)  (n ≠ −1)\n• 1/x → ln|x|\n• eˣ → eˣ\n• e^(ax) → (1/a)·e^(ax)\n• cos x → sin x\n• sin x → −cos x\n• 1/(1+x²) → arctan x\n• ∫ₐᵇ f(x)dx = F(b) − F(a)",
  },
  {
    category: "math",
    title: "Théorème de Pythagore",
    keywords: ["pythagore", "hypotenuse", "hypoténuse", "rectangle"],
    content:
      "Théorème de Pythagore:\nDans un triangle ABC rectangle en A:\n• BC² = AB² + AC²\n• L'hypoténuse BC = √(AB² + AC²)\n• Réciproque: si BC² = AB² + AC², alors le triangle est rectangle en A.",
  },
  {
    category: "math",
    title: "Théorème de Thalès",
    keywords: ["thales", "thalès", "parallele", "parallèle"],
    content:
      "Théorème de Thalès:\nSi deux droites (BC) et (DE) sont parallèles et coupent les côtés d'un triangle:\n• AB/AD = AC/AE = BC/DE\n• Si (BC) ∥ (DE), alors les rapports de longueurs sont égaux.",
  },
  {
    category: "math",
    title: "Équations du second degré",
    keywords: ["second degre", "second degré", "delta", "discriminant", "trinome", "trinôme", "polynome"],
    content:
      "Équation du second degré: ax² + bx + c = 0\n• Δ = b² − 4ac (discriminant)\n• Δ > 0: x₁,₂ = (−b ± √Δ) / (2a)\n• Δ = 0: x₀ = −b / (2a) (racine double)\n• Δ < 0: pas de racine réelle (racines complexes)\n• Somme: x₁ + x₂ = −b/a | Produit: x₁·x₂ = c/a",
  },
  {
    category: "math",
    title: "Trigonométrie",
    keywords: ["trigo", "cos", "sin", "tan", "trigonometrie", "trigonométrie"],
    content:
      "Trigonométrie:\n• cos²(x) + sin²(x) = 1\n• tan(x) = sin(x) / cos(x)\n• cos(a+b) = cos a·cos b − sin a·sin b\n• sin(a+b) = sin a·cos b + cos a·sin b\n• sin(2x) = 2·sin x·cos x\n• cos(2x) = cos²x − sin²x\n• tan(a+b) = (tan a + tan b) / (1 − tan a·tan b)",
  },
  {
    category: "math",
    title: "Suites",
    keywords: ["suite", "suites", "arithmetique", "arithmétique", "geometrique", "géométrique", "recurrence"],
    content:
      "Suites:\n• Arithmétique: Uₙ = U₀ + n·r\n  Somme = (n+1)·(U₀ + Uₙ) / 2\n• Géométrique: Uₙ = U₀ · qⁿ\n  Somme = U₀·(1 − qⁿ⁺¹) / (1 − q)  (q ≠ 1)\n• Somme infinie (|q| < 1): S = U₀ / (1 − q)",
  },
  {
    category: "math",
    title: "Logarithme & Exponentielle",
    keywords: ["logarithme", "ln", "log", "exponentielle", "expo"],
    content:
      "Logarithme & Exponentielle:\n• ln(a·b) = ln a + ln b\n• ln(a/b) = ln a − ln b\n• ln(aⁿ) = n·ln a\n• ln(eˣ) = x | e^(ln x) = x\n• log₁₀(x) = ln x / ln 10\n• e^(a+b) = eᵃ · eᵇ\n• lim x→0 ln(1+x)/x = 1",
  },
  {
    category: "physique",
    title: "Loi d'Ohm",
    keywords: ["ohm", "electricite", "électricité", "tension", "courant", "resistance", "résistance", "circuit"],
    content:
      "Loi d'Ohm:\n• U = R · I  (Tension V, Résistance Ω, Intensité A)\n• Puissance: P = U · I = R · I² (en Watts)\n• Énergie: E = P · t (en Joules)\n• Loi des nœuds: Σ I = 0\n• Loi des mailles: Σ U = 0",
  },
  {
    category: "physique",
    title: "Gravitation (Newton)",
    keywords: ["gravitation", "newton", "poids", "pesanteur", "gravite", "gravité"],
    content:
      "Gravitation universelle (Newton):\n• F = G · m₁ · m₂ / d²\n  G = 6,674 × 10⁻¹¹ N·m²/kg²\n• Poids terrestre: P = m · g\n  g = 9,81 N/kg\n• Lois de Kepler sur le mouvement des planètes.",
  },
  {
    category: "physique",
    title: "Énergie",
    keywords: ["energie", "énergie", "cinetique", "cinétique", "potentielle", "mecanique", "mécanique"],
    content:
      "Énergie:\n• Cinétique: E_c = ½·m·v² (J)\n• Potentielle: E_p = m·g·h (J)\n• Mécanique: E_m = E_c + E_p (conservée sans frottement)\n• Puissance: P = E / t (W)\n• Travail: W = F · d·cos θ (J)",
  },
  {
    category: "physique",
    title: "Loi des gaz parfaits",
    keywords: ["gaz", "pv", "nrt", "pression", "mole", "mol"],
    content:
      "Gaz parfaits:\n• P · V = n · R · T\n  P en Pa, V en m³, n en mol\n  R = 8,314 J/(mol·K)\n  T en Kelvin: T(K) = T(°C) + 273,15\n• À T constant (Boyle-Mariotte): P₁·V₁ = P₂·V₂",
  },
  {
    category: "physique",
    title: "Constantes universelles",
    keywords: ["constantes", "constante", "vitesse lumiere", "lumière", "planck", "avogadro", "g"],
    content:
      "Constantes universelles:\n• c = 3,0 × 10⁸ m/s  (vitesse de la lumière)\n• h = 6,626 × 10⁻³⁴ J·s  (constante de Planck)\n• N_A = 6,022 × 10²³ mol⁻¹  (Avogadro)\n• g = 9,81 m/s²  (pesanteur terrestre)\n• R = 8,314 J/(mol·K)  (gaz parfaits)\n• G = 6,674 × 10⁻¹¹ N·m²/kg²  (gravitation)\n• k_B = 1,38 × 10⁻²³ J/K  (Boltzmann)",
  },
  {
    category: "physique",
    title: "Mouvements & Cinématique",
    keywords: ["mouvement", "cinematique", "cinématique", "vitesse", "acceleration", "accélération", "mrua"],
    content:
      "Cinématique:\n• v = d / t  (mouvement rectiligne uniforme)\n• MRUA: v = v₀ + a·t\n  d = v₀·t + ½·a·t²\n  v² = v₀² + 2·a·d\n• Chute libre: v = g·t, h = ½·g·t²",
  },
  {
    category: "chimie",
    title: "pH & Acido-basique",
    keywords: ["ph", "acide", "base", "acido", "neutre"],
    content:
      "pH et solutions acido-basiques:\n• pH = −log₁₀[H₃O⁺]\n• pH < 7: acide | pH = 7: neutre | pH > 7: basique\n• [H₃O⁺] = 10^(−pH)\n• pH + pOH = 14 (à 25°C)\n• Produit ionique de l'eau: Ke = [H₃O⁺]·[OH⁻] = 10⁻¹⁴",
  },
  {
    category: "chimie",
    title: "Stœchiométrie",
    keywords: ["stoechiometrie", "stœchiométrie", "mole", "molaire", "equation", "équation", "reaction"],
    content:
      "Stœchiométrie:\n• n = m / M  (quantité en mol, masse en g, masse molaire en g/mol)\n• C = n / V  (concentration molaire en mol/L)\n• Équilibrer une réaction: conserver les atomes de chaque côté.\n• Volume molaire gaz: V_m = 22,4 L/mol (CNTP, 0°C, 1 atm)",
  },
];

export const suggestions = [
  "aide",
  "derivees",
  "pythagore",
  "second degre",
  "loi ohm",
  "energie",
  "constantes",
  "qui es tu",
  "25 * 4 + 18",
  "thales",
  "trigo",
  "suites",
  "gaz parfaits",
  "ph",
];

export function findKnowledge(input: string): KnowledgeEntry | null {
  const lower = input.toLowerCase().trim();
  let best: KnowledgeEntry | null = null;
  let bestScore = 0;

  for (const entry of knowledgeBase) {
    let score = 0;
    for (const kw of entry.keywords) {
      if (lower.includes(kw)) {
        score += kw.length;
      }
    }
    if (score > bestScore) {
      bestScore = score;
      best = entry;
    }
  }

  return bestScore > 0 ? best : null;
}
