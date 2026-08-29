# rbs – Rule Based System als Zustandsautomat

`rbs` ist eine kleine C-Regel-Engine, die mit der State-Machine-Bibliothek
`sm` zusammenarbeitet und zusammen einen **deterministischen, vollständig
überprüfbaren Zustandsautomaten** bildet.

Das Wissen (Regeln, Effekte, Zustands-Slots) liegt deklarativ in Daten; die
Engine ist ein kleiner Kern mit genau einer Ausführungs-Semantik.
Ein Programm in diesem Muster ist damit selbst das Modell seiner Zustände und
Übergänge.

## Schritt-Semantik

- Ein Schritt (`rbs_step`) wertet **alle** Regeln gegen dieselbe Faktenbasis
  aus – neue Fakten wirken erst ab dem **nächsten** Schritt.
- Effekte werden gegen denselben Memory-Stand (Snapshot) berechnet, der
  Commit passiert atomar am Schritt-Ende.
- Fortschritt und Beendigung erfolgen über **Konsumieren** (`N_X`-Signum)
  und **Terminieren** des Handlers – zustandsgetriggert, nicht flankengetriggert.
- Die Faktenbasis ist vor externen Zugriffen geschützt: `rbs_step` liest nur
  `facts`/`memory`, die Außenwelt speist Fakten ausschließlich zwischen den
  Schritten ein.

## Zusammenarbeit

| Schicht | Bibliothek | Aufgabe |
|---|---|---|
| `rbs` | diese Engine | Faktenbasis, Memory, Regeln (Guards → Fakten), Effekte (Memory-Ops + Trigger-Konsum) |
| `rbs_sm` | hier enthalten | RBS-Schritte + Routing des ersten aktiven Slots auf Handler |
| `sm` | `libraries/sm` | minimal-threaded State-Machine (Handler-Schleife) |
| `threading` | `libraries/threading` | Thread-Erzeugung/-Joining für `sm` |

## Nutzung

```c
#include <sm/sm.h>   /* Bibliotheken immer mit spitzen Klammern */
#include "rbs.h"

struct rbs_term if_adult[] = { {AGE, GT, 18}, {ZERO} };
enum token then_adult[] = { ADULT, PAY, ZERO };
struct rbs_effect effects[] = { { PAY, MONEY, SUB, 10 } };

struct rbs rbs = {
    .facts = fact_buffer,
    .memory = mel,
};

rbs_step(&rbs, rules, rule_count, effects, effect_count);
```

## Build & Teste

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Kern + Tests kommen ohne externe Abhängigkeiten aus; `main.c` (Demo-App)
zeigt als Beispiel Wetter- und Erwachsenen-Token mit Ausgabe je Schritt.