# EODev Development Guide

**Status:** Authoritative
**Last updated:** 2026-09-24

## Purpose

This file is the durable development reference for EODev. Update it whenever protocol behavior, build requirements, verified fixes, or the working order changes.

EODev keeps its existing artwork, fixed panel layout, and interaction style. EndlessClient is a behavior and protocol reference. Its floating panels, zoom, scaling, and screen layout are not EODev targets.

## Source Priority

When packet or gameplay sources disagree, use this order:

1. EOProtocol packet definitions from `Moffat.EndlessOnline.SDK`
2. EOLib handlers, tests, and domain behavior in local `EndlessClient-master/`
3. ArenaServ and Etheos handlers in the parent repository
4. Protocol and client notes in the parent `docs/` folder
5. Existing EODev behavior

Do not invent packet layouts. Record unresolved differences before implementation and verify them against ArenaServ.

## Repository Rules

- Keep changes inside EODev unless the task explicitly includes ArenaServ documentation.
- Keep UI layout and graphics native to EODev.
- Use EndlessClient for packet order, field types, state transitions, validation, and error handling.
- Add only behavior supported by current assets, EOProtocol, EOLib, and ArenaServ.
- Prefer one complete, tested flow over several partial features.
- Keep comments short and limited to non-obvious reasons.
- Do not commit `EndlessClient-master/`, `third_party/`, build output, IDE state, maps, pubs, private configuration, or generated research notes.

## Build and Tests

The authoritative application files are `EoDEV++.sln` and `EODev++ 2010.vcxproj`. The old DirectX-era duplicate project was removed.

Build the Release client:

```bat
build-release.cmd
```

Run protocol and connection fixtures:

```bat
run-tests.cmd
```

The current Release baseline builds with 0 errors and 662 existing warnings. The fixture suite builds with 0 errors and 16 warnings inherited from the legacy packet implementation.

## Verified Progress

### Issue #153: Build consolidation

- Commit: `a75c6a9`
- Removed the unused DirectX-era project and tracked IDE/build artifacts.
- Added one reproducible `Release|Win32` entry point.
- Added local-only exclusions for EndlessClient, SFML, build output, and IDE state.

### Issue #147: Packet fixtures

- Commit: `ddf16a2`
- Centralized production packet builders for init, account request/create, login, welcome, character selection, map requests, and pub requests.
- Added byte-exact fixtures based on EOProtocol/EOLib field order and ArenaServ readers.

### Issue #148: Connection state

- Commit: `78db8bc`
- Replaced the partial-read logic with a buffered EO frame reader.
- Handles split headers, split bodies, and multiple frames in one socket read.
- Resets framing and request state on reconnect.
- Blocks duplicate login/account requests and expires unanswered requests after five seconds.
- Keeps endpoint and packet-handler state on each connection instead of process globals.

### Issue #149: Resource integrity

- Commit: `ea65e0e`
- Writes EMF and pub payloads using their exact byte length.
- Preserves embedded zero bytes instead of truncating data with null-terminated string handling.
- Includes a binary round-trip regression fixture.

### Issues #4 and #5: Protocol framing and sequence boundaries

- Commit: `cf0f1f8`
- Encodes sequence 252 in one byte and sequence 253 in two bytes, matching ArenaServ.
- Verifies encrypted packet round trips and keeps init packets raw.
- Rejects invalid, oversized, and truncated packet input without buffer underflow.

### Account and login protocol hardening

- Builds Connection/Accept from the EOLib field names and verifies its exact bytes against ArenaServ's reader.
- Uses the server multiplier for receive and the client multiplier for send without relying on ambiguous local names.
- Does not enter the connected UI state when Connection/Accept fails to write.
- Handles the server's banned-account login reply.
- Parses every returned character while limiting EODev's character selector to its three available UI slots.
- Keeps the two-second account creation wait. EndlessClient uses the same default wait after the account-name reply before sending Account/Create.

### Map and pub transfer safety

- Queues stale EMF, EIF, ENF, ESF, and ECF downloads and requests only one file at a time.
- This ordering is required because ArenaServ rejects a second request while its current upload is active.
- Validates each init file reply against the queued file type and pub file ID before writing it.
- Uses the queued map ID instead of whichever map happens to be current when a download completes.
- Reloads pub objects from the downloaded path and releases the replaced object.
- Uses valid relative pub paths instead of invalid C++ escape sequences.

### Login and UI crash safety

- Prevents empty name, class, guild, rank, partner, home, title, and job strings from being indexed at position zero.
- Resolves paperdoll class names through the loaded ECF data instead of assigning a numeric byte to a string.
- Preserves actual guild and guild-rank values from Welcome/Reply.
- Bounds both login and character-update lists to EODev's three available selector models while still consuming every server entry.
- Rejects chat packets for unknown players instead of inserting and dereferencing a null map entry.
- Gives unknown character stances a stable standing frame instead of returning an indeterminate value.
- Removes temporary formatting buffers that were leaked or freed with the wrong delete form.

### Online player panel

- Sends the empty Players/Request packet defined by EOProtocol and used by EOLib.
- Parses ArenaServ's full online-player reply and displays the character name, title, guild tag, and resolved ECF class name in EODev's existing fixed panel.
- Protects the online-player list shared by the network and render threads.
- Refreshes the panel on elapsed wall time instead of process CPU time.

### Settings panel

- Uses EODev's original fixed GFX 47 panel and the classic control positions documented by EndlessClient.
- Loads `on` and `off` values from the original `setup.ini` format instead of treating them as false.
- Provides working controls for chat balloons, map shadows, and incoming whispers.
- Saves the visual settings immediately and restores them on the next launch.
- Sends the empty Global/Remove and Global/Player packets used by EOProtocol, EOLib, and ArenaServ when incoming whispers are enabled or disabled.
- Applies a saved disabled-whispers preference after Welcome/Reply places the character in game.
- Leaves sound, music, language, chat logging, filtering, and interaction rows untouched until those client systems exist.

### Issue #1: Character deletion safety

- Passes the real `Game` instance through character-delete send paths instead of casting a `Menu` pointer to `Game`.
- Waits for Character/Player before showing the final confirmation, matching EOLib's request-session-confirm-remove order.
- Validates the server-returned character ID against the selected character before accepting the delete session.
- Builds Character/Take and Character/Remove through tested production packet builders.

### Issue #2: Confirmation ownership

- Replaces the shared `MsgID` and `CancelTrue` flags with owner-scoped confirmation state.
- Separates return-to-menu, character-delete, and shop/craft results so one feature cannot consume another feature's answer.
- Clears pending shop and craft state when a confirmation is cancelled.
- Keeps informational messages on the single-button path without creating a confirmation result.

### Issue #3: Pending authentication and downloads

- Login and account lookup use independent request gates that block duplicate sends and expire after five seconds.
- Account creation retains the protocol-required delay without blocking the network worker.
- Map and pub requests use one serialized queue, so ArenaServ never receives overlapping uploads.
- All request state is reset on disconnect and reconnect.

### Issue #6: Connection cancellation and cleanup

- Gives each connection its own atomic stop request instead of a shared static drop flag.
- Closes the socket, signals the worker, waits for it to finish, and deletes the connection before reconnecting.
- Releases the owned socket stream when the connection is destroyed.
- Suppresses the connection-lost dialog when the disconnect was requested by the client.

### Issue #7: Idle network waiting

- Waits on the socket for up to 50 milliseconds instead of polling it every millisecond.
- Retains frequent login, account, and delayed account-creation timeout checks without consuming a CPU core while idle.

### Issues #17-#20, #22, and #24: Runtime safety

- Ignores avatar updates for players that are no longer present instead of dereferencing a missing map entry.
- Initializes the Win32 message state and repairs the unreachable arrow-key range check.
- Shuts down the connection and releases game resources when the window closes instead of entering an infinite loop or terminating inside the window callback.
- Uses the monotonic 10 millisecond tick value expected by EOProtocol and EOLib for walk and attack packets.
- Prints debug messages as data instead of treating server-controlled text as a format string.

### Issues #8 and #12: Error reporting and resource replacement

- Reports parser, protocol, and file failures as connection or resource errors after init instead of claiming the server was not found.
- Keeps the server-not-found message limited to failures before the init handshake completes.
- Writes downloads to a sibling temporary file, flushes and checks the byte count, then atomically replaces the live resource.
- Removes the temporary download when validation or replacement fails, preserving the previous live file.
- Covers binary replacement, embedded zero bytes, and temporary-file cleanup in the packet fixtures.

### Issues #11 and #13: Pub file ownership and split files

- Keeps one canonical EIF, ENF, ESF, and ECF dataset on `World`; the stale copies on `Game` were removed.
- Makes Welcome checksum checks and all current gameplay lookups use the same canonical data.
- Tracks records physically present in each pub response instead of padding file 1 to the declared total.
- Requests increasing pub file IDs until the first file's declared record total is satisfied, matching EOLib.
- Merges later file records into the canonical dataset with continuous record IDs while retaining each downloaded file on disk.
- ArenaServ's current upload code always serves its configured file and labels it as file 1, so its stock configuration remains a one-file source; the client now supports split-aware servers without changing that server boundary.

### Issues #14-#16 and #23: Bounds and shared state

- Serializes packet-driven state changes against game update and rendering through the game state lock.
- Releases that lock before waiting for connection shutdown so the network worker cannot deadlock behind the main thread.
- Ignores face, attack, damage, refresh, warp, inventory, chest, and avatar updates when their referenced player or NPC no longer exists.
- Uses bounds-safe pub lookups for NPC stats and graphics and ignores unknown NPC definitions.
- Prevents invalid paperdoll removals from clearing equipment slot zero.
- Handles zero-length pub names without indexing an empty string.
- Guards HUD and stat rendering when the main player is absent and avoids division by zero for empty HP, TP, or experience ranges.

### Issues #100, #109-#110, #113-#115, #117-#123, and #156-#158: Rendering and movement stability

- Uses monotonic elapsed time for frame pacing, player and NPC movement, map animation, combat frames, damage display, death effects, and UI refresh timers.
- Interpolates player movement over 450 milliseconds and NPC movement over 400 milliseconds using EndlessClient's 32-by-16 isometric tile offsets and animation timing as the reference.
- Queues overlapping remote-player and NPC walk destinations instead of resetting an active step when the next server packet arrives.
- Carries unused frame time into the queued step so brief frame stalls do not create movement lag.
- Keeps EODev's fixed camera and panel layout. Smooth actor offsets now move the world beneath the centered local player without adding EndlessClient zoom or floating UI behavior.
- Applies the tile-spec walkability rules supported by EOLib and blocks occupied actor positions and in-progress destinations before sending a local walk.
- Treats Refresh/Reply coordinates as authoritative. A matching active or queued destination confirms movement without a snap; a conflicting coordinate cancels interpolation and logs a correction.
- Parses ArenaServ's Walk/Reply sentinels and dropped-item entries instead of leaving unread reply data.
- Handles both the five-byte immediate NPC removal packet and full NPC death packets without requiring an item drop, and no longer reports a successfully handled death packet as unhandled.
- Initializes actor state, equipment, stance, direction, animation, and destination fields before the first render.
- Releases removed and replaced player/NPC objects, cancels stale deferred removals when an entity is re-added, and frees remaining actors when the map is destroyed.
- Rejects invalid actor and item coordinates before indexing the map render lookup table.
- Removes the duplicate local-player render and replaces per-draw multimap allocation with a reserved, stable-sorted render queue.
- Removes recurring heap allocation from core map, HUD, inventory, paperdoll, chat-bubble, character-select, and scrollbar render paths.
- Applies NPC death fading to the rendered sprite and guards HP, TP, and experience bars against zero ranges.

Live checks should cover continuous movement in every direction, fast direction changes, crowded tiles, server-rejected walks, remote players, NPC movement, combat animations, and camera tracking under a busy map. Test two clients walking continuously at the same time and introduce latency or a short frame stall. The local player should remain centered, remote actors should not restart a step when another packet arrives, and all actors should finish on server-provided coordinates.

## Current Boundary

Packet payloads, receive framing, sequence boundaries, encryption round trips, and the file-transfer build path are verified. High-risk login, character-list, paperdoll, chat, online-player, rendering, timing, and movement paths are hardened. The settings panel controls the three systems EODev can currently honor. Live ArenaServ checks are still required for account/login, multi-file synchronization, whisper preference changes, and movement correction under latency.

## Working Order

1. Run account creation, login, movement, combat animation, and multi-file synchronization end to end against ArenaServ.
2. Verify frame pacing and camera tracking on a populated map and record any remaining packet-driven snaps.
3. Continue gameplay systems through EODev's existing UI controls.

## Handoff Checklist

Before committing protocol or connection work:

1. Compare the EOProtocol type, EOLib behavior, and ArenaServ reader.
2. Add or update a fixture that exercises the production path.
3. Run `run-tests.cmd`.
4. Run `build-release.cmd`.
5. Update this file with confirmed behavior and the next unresolved boundary.
