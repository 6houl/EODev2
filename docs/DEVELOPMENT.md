# EODev Development Guide

**Status:** Authoritative
**Last updated:** 2026-09-23

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

The current Release baseline builds with 0 errors and 678 existing warnings. The fixture suite builds with 0 errors and 16 warnings inherited from the legacy packet implementation.

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

## Current Boundary

Packet payloads, receive framing, sequence boundaries, encryption round trips, and the file-transfer build path are verified. High-risk login, character-list, paperdoll, chat, and online-player paths are hardened. The settings panel now controls the three systems EODev can currently honor. Live ArenaServ checks are still required for account/login, multi-file synchronization, and whisper preference changes. Remaining gameplay systems are the next code milestone.

## Working Order

1. Run account creation, login, and multi-file synchronization end to end against ArenaServ.
2. Implement gameplay systems through EODev's existing UI controls.

## Handoff Checklist

Before committing protocol or connection work:

1. Compare the EOProtocol type, EOLib behavior, and ArenaServ reader.
2. Add or update a fixture that exercises the production path.
3. Run `run-tests.cmd`.
4. Run `build-release.cmd`.
5. Update this file with confirmed behavior and the next unresolved boundary.
