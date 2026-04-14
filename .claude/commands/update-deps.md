Update NuGet dependencies to latest stable versions.

## Instructions

1. Read all `packages.config` files under `CelestiaAppComponent/`, `CelestiaComponent/`, and `CelestiaWinUI/`.
2. For each package (except `Celestia.ANGLE`), query the NuGet V3 flat container API at `https://api.nuget.org/v3-flatcontainer/{package-id-lowercase}/index.json` to find the latest stable (non-prerelease, non-preview, non-experimental) version. For `Microsoft.Windows.SDK.BuildTools`, stay within the same major SDK version (e.g. if currently on `10.0.26100.x`, pick the latest `10.0.26100.x`, not `10.0.28000.x`).
3. For each package that has a newer version available, update:
   - All 3 `packages.config` files (version attribute)
   - All 3 `.vcxproj` files (versioned package paths in Import and Error elements)
   - Both patch files under `patches/nuget/` (context lines referencing versioned package paths)
   - `.github/workflows/build.yml` if it references any versioned package paths
4. After all edits, run a final grep to confirm no old version strings remain in the repo.
5. Report which packages were updated and which were already at the latest version.
