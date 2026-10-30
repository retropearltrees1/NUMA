import { tryEvaluateArithmetic, formatCalcResult, type CalcResult } from "./calculator";
import { findKnowledge } from "./knowledge";

export interface NumaReply {
  text: string;
  category: "calc" | "knowledge" | "conversation" | "unknown";
}

function isGreeting(s: string): boolean {
  const greetings = ["bonjour", "salut", "hello", "coucou", "hey", "bonsoir"];
  return greetings.some((g) => s.includes(g));
}

function respondConversation(input: string): string | null {
  const lower = input.toLowerCase().trim();

  if (lower.includes("je m'appelle") || lower.includes("je m appelle")) {
    return null;
  }
  if (lower.includes("qui es tu") || lower.includes("qui es-tu") || lower.includes("presentation") || lower.includes("numa")) {
    return "Je suis NUMA, ton assistant scientifique ! Je fonctionne entièrement hors-ligne. Pose-moi une question en maths, physique ou chimie, ou entre un calcul.";
  }
  if (isGreeting(lower)) {
    return "Bonjour ! Prêt pour les maths et les sciences ? Tape 'aide' ou un mot-clé pour commencer !";
  }
  if (lower.includes("ca va") || lower.includes("comment vas") || lower.includes("ça va")) {
    return "Je tourne à plein régime ! Que veux-tu réviser aujourd'hui ?";
  }
  if (lower.includes("merci")) {
    return "Avec plaisir ! N'hésite pas si tu as d'autres questions.";
  }
  if (lower.includes("au revoir") || lower.includes("bye") || lower.includes("a+") || lower.includes("à bientot")) {
    return "À bientôt ! Bon courage pour tes révisions.";
  }
  return null;
}

export function numaRespond(input: string, savedName?: string): { reply: NumaReply; newName?: string } {
  const lower = input.toLowerCase().trim();

  const calc = tryEvaluateArithmetic(input);
  if (calc) {
    if (calc.ok) {
      const expr = (calc as Extract<CalcResult, { ok: true }>).expression;
      return {
        reply: {
          text: `${expr} ${formatCalcResult(calc)}`,
          category: "calc",
        },
      };
    } else {
      return {
        reply: {
          text: `Erreur: ${(calc as Extract<CalcResult, { ok: false }>).error}`,
          category: "calc",
        },
      };
    }
  }

  if (lower.includes("je m'appelle") || lower.includes("je m appelle")) {
    const match = lower.match(/appelle\s+([a-zà-ÿ]+)/);
    const name = match ? match[1] : "";
    const capitalizedName = name ? name.charAt(0).toUpperCase() + name.slice(1) : "";
    return {
      reply: {
        text: `Enchanté${capitalizedName ? ", " + capitalizedName : ""} ! C'est noté, je retiens ton prénom pour notre session.`,
        category: "conversation",
      },
      newName: capitalizedName,
    };
  }

  if (lower.includes("qui suis je") || lower.includes("mon nom") || lower.includes("mon prenom") || lower.includes("mon prénom")) {
    if (savedName) {
      return {
        reply: {
          text: `Tu t'appelles ${savedName} ! Prêt à réviser ?`,
          category: "conversation",
        },
      };
    }
    return {
      reply: {
        text: "Tu ne me l'as pas encore dit ! Dis-moi par exemple : « je m'appelle ... »",
        category: "conversation",
      },
    };
  }

  const conv = respondConversation(input);
  if (conv) {
    return { reply: { text: conv, category: "conversation" } };
  }

  if (lower.includes("aide") || lower.includes("menu") || lower.includes("commandes") || lower.includes("help")) {
    return {
      reply: {
        text: "Mots-clés disponibles:\n• Maths: dérivées, primitives, pythagore, thalès, second degré, trigo, suites, logarithme\n• Physique: ohm, gravitation, énergie, gaz, constantes, cinématique\n• Chimie: pH, stœchiométrie\n• Ou entre un calcul : 25 * 4 + 18",
        category: "conversation",
      },
    };
  }

  const knowledge = findKnowledge(input);
  if (knowledge) {
    return {
      reply: {
        text: knowledge.content,
        category: "knowledge",
      },
    };
  }

  return {
    reply: {
      text: "Je n'ai pas reconnu ta demande. Essaie : 'aide', 'dérivées', 'pythagore', 'second degré', 'ohm', ou entre un calcul (ex : 25 * 4 + 18) !",
      category: "unknown",
    },
  };
}
