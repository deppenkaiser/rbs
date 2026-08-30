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
- **Fakten persistieren**: Sie existieren, bis sie **explizit** zurückgesetzt
  werden – durch eine Regel (`else_facts` mit `N_X`-Form) oder eine externe
  Faktenquelle (App-Handler, z. B. Systemzeit). Es gibt kein automatisches
  Konsumieren; Effekte feuern zustandsgetriggert, solange ihr Trigger aktiv ist.
- Jede Regel kann zusätzlich zu `then_facts` einen **`else_facts`-Zweig**
  haben, der gesetzt wird, wenn der `if`-Zweig nicht zutrifft (typisch der
  deklarative Reset, z. B. `N_WET`).
- Fortschritt und Beendigung sind **Programmlogik** der App (Zähler, externer
  Input, Handler liefert `false`) – die Engine selbst läuft, bis die Welt
  keine aktiven Zustands-Marker mehr kennt.
- Die Faktenbasis ist vor externen Zugriffen geschützt: `rbs_step` liest nur
  `facts`/`memory`, die Außenwelt speist Fakten ausschließlich zwischen den
  Schritten ein.

## Zusammenarbeit

| Schicht | Bibliothek | Aufgabe |
|---|---|---|
| `rbs` | diese Engine | Faktenbasis, Memory, Regeln (Guards → then/else-Fakten), Effekte (Memory-Ops) |
| `rbs_sm` | hier enthalten | RBS-Schritte + Routing des ersten aktiven Slots auf Handler; FSM-Lebenszyklus (start/stop) |
| `sm` | `libraries/sm` | minimal-threaded State-Machine (Handler-Schleife) |
| `threading` | `libraries/threading` | Thread-Erzeugung/-Joining für `sm` |

## Lebenszyklus der App-FSM (`rbs_sm`)

Die App-State-Machine kennt drei Phasen, die durch optionale Handler im
`struct rbs_sm` gesetzt werden:

| Feld | Phase | Aufgabe |
|---|---|---|
| `start_handler` | **Konstruktor** | läuft genau einmal vor der Zustandsschleife und baut die **initiale Welt** auf (externe Ausgangslage, z. B. Regen/Bewölkung setzen) |
| `on_step` | Schritt-Callback | wird je Schritt mit der Schrittnummer aufgerufen (z. B. Step-Grenze in der Konsole) |
| `stop_handler` | **Destruktor** | läuft genau einmal nach Terminierung der Schleife, z. B. zur Bilanz der Endfakten |

```c
struct rbs_sm fsm = {
    .slots = slots, .slot_count = ...,
    .on_step = _app_on_step,
    .start_handler = _app_start,   /* Konstruktor: Welt aufbauen */
    .stop_handler  = _app_stop,    /* Destruktor:  Endfakten bilanzieren */
};
rbs_sm_run(&fsm);
```

Damit wird die Faktenbasis vollständig aus den Lebenszyklus-Handlern versorgt:
der Konstruktor stellt die Ausgangslage bereit, die Slot-Handler liefern
laufenden externen Input (z. B. Wettersensor), der Destruktor bilanziert das
Ergebnis. `main` selbst speist dann keine Fakten mehr ein, sondern nur
Welt-Konstanten (z. B. `AGE`/`MONEY`).

## Nutzung

```c
#include <sm/sm.h>   /* Bibliotheken immer mit spitzen Klammern */
#include "rbs.h"

struct rbs_term if_adult[] = { { .comparison = true, .value_enum = AGE, .op = GT, .operand = 18 }, { .comparison = false, .fact_enum = ZERO } };
enum token then_adult[] = { ADULT, PAY, ZERO };
struct rbs_effect effects[] = { { .trigger_fact_enum = PAY, .value_enum = MONEY, .op = SUB, .operand = 10 } };

struct rbs rbs = {
    .facts = fact_buffer,
    .token_count = TOKEN_COUNT,
    .memory = mel,
    .value_count = VALUE_COUNT,
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

## Wiki

Dokumentation: [rbs – Rule Based System als Zustandsautomat](https://czybor.i234.me/wiki/sw-module/rbs/) (Quartz-Wiki)