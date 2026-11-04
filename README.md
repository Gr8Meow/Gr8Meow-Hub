# Gr8Meow Hub

One small Windows app that installs and updates every Gr8Meow app from its GitHub release.
Developed by Gr8Meow. Free and open source (MIT). Native C++, no runtime needed.

## What it does

- Shows all apps listed in `apps.json` as cards: **Install**, **Update**, **Reinstall**, **Open**, **Remove**
- "Updates available" banner with **Update all**
- **NEW** badge when you add a new app to `apps.json` (no Hub rebuild needed)
- Updates itself: **Update vX** appears in the header, Yes = update now, No = later
- Downloads only from GitHub over HTTPS, and checks the release `.sha256` file when there is one
- Same look and themes as Meow Player (Mint, Pink, Purple, Ocean, Sunset, Mono)

## Repos

| Repo | What it holds |
| --- | --- |
| `Gr8Meow/Gr8Meow-Hub` | source code + the build (this repo) |
| `Gr8Meow/Gr8Meow-Hub-Updates` | public: `apps.json` and the Hub releases (the folder `updates-repo/` here) |
| `Gr8Meow/Meow-player-Updates` | public: Meow Player releases |
| `Gr8Meow/live-wallpaper-Updates` | public: LiveWall releases |

The Hub only talks to the public `-Updates` repos, so the source repos can stay private if you want.

## Set up (once)

1. Push this repo's files to `Gr8Meow/Gr8Meow-Hub`.
2. Put `updates-repo/apps.json` and `updates-repo/README.md` into `Gr8Meow/Gr8Meow-Hub-Updates` on the **main** branch
   (the repo needs one commit before a release can be created).
3. Let the build publish to the Updates repo: create a fine-grained token (GitHub > Settings > Developer settings >
   Personal access tokens) for **only** `Gr8Meow-Hub-Updates` with *Contents: Read and write*, and save it in
   `Gr8Meow-Hub` > Settings > Secrets and variables > Actions as `UPDATES_TOKEN`.
4. Release:
   ```
   git tag v1.0.0
   git push origin v1.0.0
   ```
   The build makes `Gr8MeowHub-windows-x64.zip` (+ `.sha256`) and publishes it in `Gr8Meow-Hub-Updates`.

Without the token you can also upload the zip from the Actions page to a release of `Gr8Meow-Hub-Updates` by hand.

## Add an app

Add an entry to `apps.json` in `Gr8Meow-Hub-Updates` and push (the Hub reads it from the `main` branch):

| Field | Meaning |
| --- | --- |
| `id` | short unique name, lowercase letters, digits, `-` `_` |
| `name`, `description` | shown on the card |
| `repo` | `owner/repo` of the app |
| `asset` | part of the release file name to download, e.g. `windows-x64.zip` (default `.zip`) |
| `exe` | the app's exe name, used by the **Open** button |

Release files can be a `.zip` (unpacked to `%LOCALAPPDATA%\Gr8MeowHub\apps\<id>`) or an installer `.exe`.
If the release also has `<file>.sha256`, the download is verified before it is used.

## Ship an update of an app

Publish a new release with a higher tag (`v1.1.0`) in that app's `-Updates` repo. Every Hub shows **Update** on that card.
Hub updates work the same way, from `Gr8Meow-Hub-Updates`.

## Connect an app to the Hub (optional)

Apps can tell the Hub they are installed (also when not installed through the Hub) by writing
`HKCU\Software\Gr8MeowStudios\Apps\<id>` with `Version` and `ExePath` (see `HubLink.h`), and open the Hub on their
card with `gr8meowhub.exe --update <id>`.

## Build from source

MSVC:

```
cl /O2 /EHsc /MT /GS /guard:cf /DUNICODE /D_UNICODE gr8meowhub.cpp /link /SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup /guard:cf /DYNAMICBASE /NXCOMPAT /HIGHENTROPYVA /DEPENDENTLOADFLAG:0x800 user32.lib gdi32.lib shell32.lib ole32.lib advapi32.lib shlwapi.lib dwmapi.lib gdiplus.lib winhttp.lib bcrypt.lib
```

MinGW:

```
g++ -O2 -municode -mwindows -static gr8meowhub.cpp -o gr8meowhub.exe -lgdiplus -lwinhttp -lbcrypt -lole32 -lshell32 -luser32 -lgdi32 -ladvapi32 -lshlwapi -ldwmapi -luuid
```

Local builds use the same repos. To test with another repo put `owner/repo` in `hub-repo.txt` next to the exe, or put an `apps.json` next to the exe.

## Custom font

Put an Ortoid font file (`ortoid*.ttf` or `*.otf`) next to `gr8meowhub.exe` or in a `fonts` folder. Check the font license first.

## Security

No admin rights. Only GitHub hosts are contacted, HTTPS only. Download links must belong to the app's own repo.
Folders are only deleted inside `%LOCALAPPDATA%\Gr8MeowHub`. Builds use stack protection, ASLR, DEP,
Control Flow Guard and system-only DLL loading.

## License

MIT, copyright Gr8Meow. Check the Craftwork (Edge Icons) license before publishing the icons.
