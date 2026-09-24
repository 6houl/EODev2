# EODev UI Completion Matrix

**Status:** Active
**Last updated:** 2026-09-24

This file tracks the remaining UI work and the evidence needed to finish it. Update the row, implementation notes, tests, and GitHub issue together whenever a feature changes.

## Boundaries

- Keep EODev's fixed 640x480 layout and original artwork.
- Use EOProtocol for packet layouts, EOLib for state and validation, ArenaServ for server behavior, and EndlessClient for classic behavior and coordinates.
- Do not copy EndlessClient's floating panels, zoom, or expanded screen layout.
- Do not mark a row complete until the Release build passes and the feature has a reproducible live check.

## Status Keys

- `Missing`: no usable implementation.
- `Shell`: artwork or button exists without working content.
- `Partial`: useful behavior exists but the listed work remains.
- `Implemented`: code and automated checks pass; live validation may remain.
- `Complete`: implementation and live validation are finished.
- `Blocked`: a required protocol, gameplay state, or asset is not available yet.

## Shared UI Foundation

| Area | Status | Implementation | Remaining verification |
| --- | --- | --- | --- |
| Fixed logical canvas | Implemented | 640x480 view uses aspect-correct integer scaling and centered letterboxing | Resize to 1x, 2x, ultrawide, and tall window sizes |
| Pixel-art filtering | Implemented | Resource and generated menu textures use nearest-neighbor filtering without mipmaps | Compare login buttons, textboxes, HUD, and map tiles at 1x and 2x |
| Resized mouse input | Implemented | Mouse coordinates pass through the letterboxed SFML view and reject the outside bars | Check every screen edge and HUD button at 2x |
| Draw-order conventions | Partial | Status bars use an explicit foreground depth | Replace remaining unexplained depth literals with named UI layers |
| Visual regression coverage | Missing | Tracked by issue #150 | Add screenshot and interaction checks for supported window sizes |

## Fixed HUD Panels

| Issue | Element | Status | Current boundary / next work |
| --- | --- | --- | --- |
| #27 | Minimap | Implemented | Classic fixed-canvas tile, warp, player, and NPC overlay is centered on the player; live map coverage check remains |
| #28 | Party | Shell | Panel background only; add member rows, HP, leader state, and actions from EOLib party state |
| #29 | Active skills | Partial | Welcome now retains learned spells and the panel renders scrollable spell icons, levels, skill points, and hover names; slot persistence, casting, cooldown, targeting, and training depend on #62/#63 |
| #30 | Character stats/training | Partial | Values render; add server-authoritative stat and skill training controls |
| #31 | Chat modes | Partial | Basic public/global/whisper/guild views exist; complete supported modes and behavior |
| #32 | Whisper tabs | Partial | Replace placeholder player label with session target data |
| #33 | Chat presentation | Partial | Implement lock state and active-tab visuals |
| #34 | Player context menu | Partial | Complete actions and permission/range checks |
| #35 | Session EXP | Missing | Add session baseline, gains, reset behavior, and display |
| #36 | Quest status/history | Missing | Requires quest state and protocol work before panel content |
| #37 | Friends/ignore | Missing | Add list windows and file-backed state using EOLib behavior |
| #38 | Status clock | Implemented | Renders local `HH:MM:SS` at the classic fixed-client position; live visual check remains |
| #39 | SP bar | Implemented | Welcome initializes current SP from MaxSP; attacks spend one SP and standing/sitting recovery follows EOLib behavior; live combat check remains |
| #40 | Status bars | Implemented | HP, TP, SP, and TNL fills clamp their values and guard zero ranges; live edge-case check remains |
| #41 | Help/status message | Implemented | Uses the EndlessClient three-second lifetime and clears both stored title and message; live visual check remains |
| #43 | Loading state | Missing | Add map/pub progress, failure details, retry, and cancellation |
| #44 | Panel state | Missing | Persist active panel and movable dialog positions where EODev supports movement |
| #45 | Macros | Shell | Panel artwork only; define supported macro storage and activation or remove the button |
| #46 | Passive skills/help | Shell | Both panels only draw artwork; define content supported by current data |
| none | News | Shell | Panel and scrollbar exist but no news content source is connected |
| #26 | Online players | Implemented | Correct raw reply byte, classic columns, icons, sorting, count, and scrolling; live retest required |
| #25 | Settings | Partial | Whisper, balloons, and shadows work; remaining rows depend on configuration/audio/localization work |

## Menu and Account Screens

| Issue | Element | Status | Current boundary / next work |
| --- | --- | --- | --- |
| #42 | Credits | Implemented | Existing screen now credits the original game, protocol/client references, server compatibility, and project contributors; live visual check remains |
| #60 | Password change | Partial | Complete validation, response handling, and feedback |
| #124 | Empty login feedback | Implemented | Empty account/password input shows local feedback, focuses the missing field, and sends no packet |
| #125 | Pending authentication state | Partial | Request gates exist; disable and visually mark affected controls |
| #128 | Account/login responses | Partial | Map every EOProtocol/Etheos response to clear user feedback |
| #131 | Account validation | Partial | Restore all original input and password rules |
| #132 | Character creation | Partial | Add complete local validation and server response feedback |

## Dialogs and Service Interfaces

| Issues | Elements | Status | Dependency |
| --- | --- | --- | --- |
| #47-#48 | Bank, vault, locker | Missing | Bank/locker packets and server sessions |
| #49 | Barber | Missing | Barber packets and appearance validation |
| #50 | Bard/jukebox | Missing | Music and jukebox packets plus audio system |
| #51 | Message boards | Missing | Board list/read/post packets |
| #52 | Books/player information | Missing | Book and paperdoll information packets |
| #53 | Friend/ignore management | Missing | Social list persistence and online state |
| #54 | Guild management | Missing | Complete guild protocol |
| #55 | Inn/citizen/law/priest/marriage | Missing | Corresponding service protocols |
| #56 | Quest dialogs | Missing | Quest protocol and persistent quest state |
| #57 | Skillmaster | Missing | Skillmaster listings and stat/skill training packets |
| #58 | Trade | Missing | Complete trade session protocol |
| #59 | Player item/gold transfer | Missing | Item and gold transfer packets and validation |
| #61 | Messages/notifications | Partial | Cover every supported server message variant |

## Existing Complex UI

| Issues | Area | Status | Remaining work |
| --- | --- | --- | --- |
| #88 | Inventory stored quantities | Implemented | `SetAmount` updates the stored item; live mutation check remains |
| #89 | Inventory recalculation | Implemented | Rebuilds grid placement from an unchanged item snapshot without doubling quantities; live check remains |
| #90 | Inventory grid bounds | Implemented | Shared width/height placement checks cover all eight EIF shapes without out-of-range access; drag checks use the same bounds |
| #91 | Inventory valid cells | Implemented | New item state is accepted only after a complete shape fits and otherwise reports that no room is available |
| #92 | Authoritative amounts | Implemented | Drop and junk replies apply ArenaServ/EOProtocol remaining amounts, including zero gold |
| #93 | Inventory weight | Implemented | Welcome and item replies update the player stats used directly by inventory and stats panels |
| #94-#99 | Paperdoll | Partial | Safe strings, home/class data, stats, remote equipment, paired slots, and full graphic IDs |
| #101-#102 | Chest | Partial | Session isolation, close/update/range/error behavior |
| #103-#105 | Shop/craft | Partial | Welcome text, variable recipes, and all failure states |
| #106-#107 | Quantity dialogs | Partial | Safe numeric parsing and pending server response state |

## Configuration and Quality Dependencies

| Issue | Status | Work |
| --- | --- | --- |
| #137 | Missing | Correct fullscreen parsing |
| #138 | Implemented | Fullscreen, resizable, and stay-on-top startup settings now apply without changing the 640x480 logical canvas; live mode checks remain |
| #139 | Partial | Load, save, and use all declared settings |
| #140 | Missing | Implement audio before enabling its settings rows |
| #141 | Missing | Replace hardcoded UI text with localization |
| #142 | Missing | Persist chat, social, spell-slot, and panel state |
| #150 | Missing | Add panel, dialog, context-menu, and quantity interaction coverage |

## Implementation Order

1. Finish and live-test the shared scaling, filtering, mouse, and layering foundation.
2. Complete fixed HUD panels in issue order, starting with the minimap, party, and active skills.
3. Finish chat, context menu, session displays, real SP, status messages, loading, and panel state.
4. Finish login/account validation and feedback.
5. Repair inventory, paperdoll, chest, shop, and quantity flows.
6. Add service dialogs only after their EOProtocol/EOLib/ArenaServ dependencies exist.
7. Complete settings, localization, persistence, and UI interaction coverage.

## Handoff Checklist

For each completed row, record:

1. EOProtocol message names and field types, if packets are involved.
2. EOLib handler or domain types used as behavior references.
3. ArenaServ handler used for compatibility verification.
4. EndlessClient panel/control files used for behavior or coordinates.
5. EODev files changed and any new ownership rules.
6. Automated checks and Release build result.
7. Exact live test steps and result.
8. Commit hash and GitHub issue update.

## Current Batch Verification

- Release rebuild: passed with 0 errors and 660 warnings on 2026-09-24.
- Packet and connection fixtures: all passed with 0 errors and 16 inherited warnings.
- Scaling references: SFML logical views plus EODev's fixed 640x480 artwork and input coordinates.
- Minimap references: `MiniMapRenderer.cs`, EODev `Full_EMF`, `ENF`, and live map actor collections. The overlay keeps the classic 28x14 grid and uses resource 2/45.
- SP references: EOProtocol Welcome `MaxSp`, EOLib `CharacterStats`, `CharacterActions`, and `PeriodicStatUpdaterComponent`. ArenaServ does not send current SP in Welcome, so it starts full and is maintained locally as in EndlessClient.
- Window configuration references: EODev `config/setup.ini` keys and the fixed logical view. Fullscreen uses the desktop mode only as a presentation surface.
- Inventory references: EOProtocol/EOLib `ItemDropHandler` and `ItemJunkHandler`, ArenaServ `Item_Drop` and `Item_Junk`, EIF item shapes, and the Welcome inventory/spell payload.
- Required live checks: resize across 1x/2x and unusual aspect ratios; click every screen edge; open the minimap on small/large maps and around map edges; verify all marker colors; attack to zero SP while standing and sitting; start in windowed, fixed-size, fullscreen, and stay-on-top modes; load all eight inventory shapes, rearrange edge cells, pick up into a full grid, drop/junk partial stacks and gold, and inspect/scroll a character with more than 16 spells.
