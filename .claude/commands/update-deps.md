Update NuGet dependencies to latest stable versions.

## Instructions

1. Read all `packages.config` files under `CelestiaAppComponent/`, `CelestiaComponent/`, and `CelestiaWinUI/`.
2. For each package (except `Celestia.ANGLE`), query the NuGet V3 flat container API at `https://api.nuget.org/v3-flatcontainer/{package-id-lowercase}/index.json` to find the latest stable (non-prerelease, non-preview, non-experimental) version. If that API is unavailable, verify versions using the official NuGet package pages or V2 API; follow pagination when listing versions. For `Microsoft.Windows.SDK.BuildTools`, stay within the same SDK line (e.g. if currently on `10.0.26100.x`, pick the latest `10.0.26100.x`, not `10.0.28000.x`) unless the user explicitly approves moving to another SDK line.
3. For each package that has a newer version available, update:
   - All 3 `packages.config` files (version attribute)
   - All 3 `.vcxproj` files (versioned package paths in Import and Error elements)
   - Both patch files under `patches/nuget/` (context lines referencing versioned package paths)
   - `.github/workflows/build.yml` if it references any versioned package paths
   - For SDK BuildTools changes, verify the package's actual `bin/<SDK version>/` paths and update workflow calls to `makeappx.exe` and `signtool.exe`; the SDK directory version differs from the NuGet package version.
   - Make package-specific edits rather than global version-string replacements. Package IDs can end in digits (for example, `Microsoft.Web.WebView2.1.0.x`), so replacing another package's version such as `2.1.0` globally can corrupt unrelated paths.
4. Verify every versioned package path matches the exact package ID and version in the corresponding `packages.config`.
5. Before committing, verify both architecture patches apply to the updated project files using the same options as CI:
   ```
   git apply --check patches/nuget/nuget-x64.patch --ignore-whitespace --whitespace=nowarn
   git apply --check patches/nuget/nuget-arm64.patch --ignore-whitespace --whitespace=nowarn
   ```
   Check each patch independently; they are alternatives, not patches to apply together. Updating only `packages.config` and `.vcxproj` files is incomplete even if their package paths agree.
6. Run a final grep across project files, patches, and workflows to confirm no old version strings or malformed package paths remain. Run `git -c core.whitespace=cr-at-eol diff --check` to accommodate the Windows files' CRLF line endings.
7. Report which packages were updated, which were already at the latest version, and whether both patch checks passed. Do not claim build validation when only non-build checks were run.
