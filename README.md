# 🤖 NUMA — Assistant IA Hors-Ligne pour NumWorks

<p align="center">
  <img src="src/icon.png" width="80" alt="NUMA Icon">
</p>

<p align="center">
  <b>Assistant conversationnel autonome et expert scientifique codé en C natif pour les calculatrices NumWorks (N0110, N0115, N0120).</b><br>
  100% hors-ligne, fluide, avec moteur NLP embarqué, clavier complet et base de connaissances scientifiques.
</p>

---

## ✨ Fonctionnalités

- **Interface de Chat Moderne :** Bulles de discussion avec défilement de l'historique et affichage fluide en 320x240.
- **Saisie Clavier Complète :**
  - **Mode NUM :** chiffres `0-9`, opérateurs `+`, `-`, `*`, `/`, parenthèses `(`, `)`, puissance `^`.
  - **Mode ALPHA :** saisie de toutes les lettres de l'alphabet (A-Z) et de l'espace directement avec le clavier de la calculatrice !
  - **Suggestions instantanées :** flèches Droite/Gauche quand le champ est vide pour tester instantanément des exemples.
- **Calculateur Arithmétique Direct :** Évalue automatiquement les expressions numériques (ex: `25 * 4 + 18`, `1000 / 8`).
- **Base de Connaissances & Formulaire :**
  - **Mathématiques :** Dérivées usuelles (`(x^n)'`, `(e^x)'`, `(u*v)'`, etc.), primitives, théorème de Pythagore, théorème de Thalès, équations du second degré (discriminant $\Delta$), trigonométrie, suites arithmétiques & géométriques.
  - **Physique-Chimie :** Loi d'Ohm ($U=R\cdot I$, $P=U\cdot I$), gravitation de Newton ($F=G\cdot m_1 m_2/d^2$), énergie cinétique & potentielle, loi des gaz parfaits ($PV=nRT$), constantes universelles ($c$, $h$, $N_A$, $g$).
- **Personnalité & Mémoire de Session :** Dialogue interactif, présentation, salutations et mémorisation du prénom de l'utilisateur.

---

## 🎮 Commandes de la Calculatrice

| Touche | Action |
| :--- | :--- |
| **ALPHA** | Basculer entre le mode lettres (**ALPHA**) et chiffres (**NUM**) |
| **OK / EXE** | Envoyer la question / le calcul à NUMA |
| **Backspace** | Effacer le dernier caractère |
| **Back** | Vider le champ de saisie |
| **Haut / Bas** | Faire défiler l'historique des discussions |
| **Gauche / Droite** | Faire défiler les suggestions rapides (quand le champ est vide) |
| **Home / On-Off** | Quitter l'application proprement |

---

## 🚀 Installation sur la NumWorks

1. Télécharge le fichier **`numa.nwa`** depuis les [Artifacts de la dernière compilation GitHub Actions](https://github.com/retropearltrees1/NUMA/actions).
2. Connecte ta calculatrice NumWorks avec un câble USB à ton ordinateur.
3. Ouvre **[my.numworks.com/apps](https://my.numworks.com/apps)** dans Chrome ou Edge.
4. Glisse-dépose le fichier **`numa.nwa`** et clique sur **Installer**.

---

## 🛠️ Compilation

Le projet utilise GitHub Actions pour compiler automatiquement le binaire ARM Cortex-M7 à chaque mise à jour.

```bash
make
```

Requiert `arm-none-eabi-gcc` et `nwlink@1.0.0`.
