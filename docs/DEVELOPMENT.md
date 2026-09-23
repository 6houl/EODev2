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

The current Release baseline builds with 0 errors and 689 existing warnings. The fixture suite builds with 0 errors and 19 warnings inherited from the legacy packet implementation.

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

## Current Boundary

The packet payload layouts and receive framing are covered by automated tests. Sequencing, encryption, and a live ArenaServ login/account integration run still need dedicated verification. Do not treat a successful compile as proof that the full login flow is complete.

## Working Order

1. Verify and correct sequence insertion, encryption, and packet-length validation.
2. Run account creation and login end to end against ArenaServ.
3. Complete map/pub synchronization and validation.
4. Remove confirmed crash and corruption paths.
5. Implement gameplay systems through EODev's existing UI controls.

## Handoff Checklist

Before committing protocol or connection work:

1. Compare the EOProtocol type, EOLib behavior, and ArenaServ reader.
2. Add or update a fixture that exercises the production path.
3. Run `run-tests.cmd`.
4. Run `build-release.cmd`.
5. Update this file with confirmed behavior and the next unresolved boundary.
