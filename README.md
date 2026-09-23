# EODev

EODev is the custom Endless Online client used with ArenaServ and Etheos.

## Build

The authoritative solution is `EoDEV++.sln`. It uses `EODev++ 2010.vcxproj` and targets Win32.

Required local dependencies:

- Visual Studio with the C++ desktop workload and v143 toolset
- PTypes built as `lib/ptypes.lib`
- SFML 2.5.1 under `third_party/sfml/SFML-2.5.1/`

Build the release client with:

```bat
build-release.cmd
```

Use `build-release.cmd -BuildOnly` for an incremental build.

The dependency folders, reference client, IDE state, and build output are local-only and must not be committed.
