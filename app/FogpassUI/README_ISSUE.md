# Readme — Issues and Resolution

Repository: FogpassUI

This file documents the QML/Build issues encountered while building and running the UI and the exact fixes applied.

## Summary of observed problems
- Build-time QML parse error: "Expected token ','" at `Main.qml:30` when using an object-literal for `StackView.initialItem`.
- Buttons and labels not visible (blank/black screen). Runtime errors about control customization and `control` being undefined.
- Invalid property `minimumWidth` used on Buttons (should use `Layout.minimumWidth` inside layouts).
- Unexpected token `;` from single-line semicolon-delimited property blocks (some toolchains reject these inline statements).
- Assigning `ApplicationWindow` (QObject*) into a QQuickItem* property led to "Cannot assign QObject* to QQuickItem*".
- Missing QML files at runtime when using incorrect resolved paths (e.g., `content/content/DemoPage.qml`).
- Linker error when attempting to call `QQuickStyle::setStyle(...)`: undefined reference because QuickControls2 wasn't linked.
- VirtualKeyboard import warning (optional) and StackView conflicting anchors warning.

## Files added/modified (high level)
- `Main.qml` — replaced object-literal `initialItem` with a `Component.onCompleted` push and `property alias stack` for navigation.
- `content/WelcomePage.qml` — created/updated welcome page, removed custom `background`/`contentItem` (final simplification), accept `property var parentWindow` and push `DemoPage.qml` with `parentWindow`.
- `content/DemoPage.qml` — created/updated demo page, removed inline semicolons and custom styling, accept `property var parentWindow`.
- `main.cpp` — previously added/then removed `QQuickStyle::setStyle(...)` to avoid linking QuickControls2; final state is the simple default style (no QuickControls2 link required).

## Exact problems & fixes (step-by-step)

1) QML parse error: "Expected token ','"
   - Cause: used an object-literal directly in `StackView.initialItem` like:
     `initialItem: { item: Qt.resolvedUrl("content/WelcomePage.qml") properties: { parentWindow: appWindow } }`
   - Fix: avoid the object literal. Push the initial page in `Component.onCompleted`:
     `Component.onCompleted: stackView.push(Qt.resolvedUrl("content/WelcomePage.qml"), { parentWindow: appWindow })`

2) Buttons/labels invisible + style warnings
   - Cause: custom `background` and `contentItem` are not supported by some native styles; also used `contentItem: Text { text: control.text }` but `control` was undefined in that context.
   - Fix (two approaches tried):
     - Initially used `QQuickStyle::setStyle("Basic")` in `main.cpp` (requires linking QuickControls2). That produced a linker error when QuickControls2 wasn't linked.
     - Final chosen fix: remove `QQuickStyle` usage and simplify QML to use default control visuals (remove `background` and `contentItem` customizations), and replace `control.text` with `buttonId.text` (give buttons explicit ids like `enterBtn`, `quitBtn`) so text is referenced safely.

3) Wrong property name `minimumWidth`
   - Cause: Using `minimumWidth` directly on a `Button` inside layouts (not a property on Button in QtQuick Controls).
   - Fix: use layout attached property: `Layout.minimumWidth: 80`.

4) Unexpected token `;` errors
   - Cause: single-line element definitions with semicolon-separated properties (e.g., `Text { text: "X"; color: ... }`) triggered parser errors in some environments.
   - Fix: expand these into multi-line property blocks, e.g.:
     Text {
       text: "X"
       color: "#000"
     }

5) Assigning the ApplicationWindow into an Item-typed property
   - Cause: some pages attempted `property Item parentWindow` and then had `parentWindow` set to the ApplicationWindow (QObject), causing the "Cannot assign QObject* to QQuickItem*" error.
   - Fix: use a generic property type `property var parentWindow` so the ApplicationWindow reference can be passed safely.

6) Wrong resolved URL paths
   - Cause: pushing `"content/DemoPage.qml"` from a page that was already in `content/` resulted in `content/content/DemoPage.qml` being resolved, producing "No such file or directory".
   - Fix: use relative pushes when the page is in the same folder (e.g. `Qt.resolvedUrl("DemoPage.qml")`) or use explicit `qrc:/` resource paths if files are bundled.

7) QQuickStyle link error
   - Cause: calling `QQuickStyle::setStyle()` requires linking `Qt6::QuickControls2`.
   - Fixes:
     - Quick fix: remove the `QQuickStyle::setStyle()` call and use the default system style (no extra linking required).
     - If a custom Controls style is required, update `CMakeLists.txt` to `find_package(Qt6 COMPONENTS Quick QuickControls2 REQUIRED)` and `target_link_libraries(<your-target> PRIVATE Qt6::QuickControls2)` then re-add `QQuickStyle::setStyle("Basic")` in `main.cpp`.

## Commands used during debugging and to rebuild
- Build from a PowerShell prompt (example):
```powershell
cd 'C:\NMSPL\qt_workspace\trial1\build\Desktop_Qt_6_10_0_MinGw_64_bit-Debug'
cmake --build . --config Debug
.\apptrial1.exe
```

## Files changed during debugging (concrete)
- `c:\NMSPL\qt_workspace\trial2\FogpassUI\Main.qml` — refactored to use `StackView` + `Component.onCompleted` push
- `c:\NMSPL\qt_workspace\trial2\FogpassUI\content\WelcomePage.qml` — added `property var parentWindow`, removed custom backgrounds/contentItems in final iteration, fixed push path to `DemoPage.qml`, added button ids
- `c:\NMSPL\qt_workspace\trial2\FogpassUI\content\DemoPage.qml` — added `property var parentWindow`, removed inline semicolons and custom style blocks, added button ids and safe references
- `c:\NMSPL\qt_workspace\trial2\FogpassUI\main.cpp` — briefly added `QQuickStyle::setStyle("Basic")` to test style customization, then removed it to avoid linker issues (optionally can be re-added if QuickControls2 is linked)

## Recommendations / next steps
- If you want control customizations (background/contentItem) to work reliably, link `Qt6::QuickControls2` in `CMakeLists.txt` and set the style in `main.cpp` (see section 7). I can prepare an exact CMake patch if you want.
- For packaging or to ensure QML files are always available, add a `resources.qrc` and include your `content/` QML files; update `CMakeLists.txt` with `qt_add_resources(...)`.
- Consider creating a small `Theme.qml` singleton to centralize colors and sizes (if you later want to reintroduce styling cleanly).

If you want, I will now:
- Add a `resources.qrc` + CMake patch to bundle the `content/` files (recommended), or
- Re-add QuickControls2 linking and set `QQuickStyle::setStyle("Basic")` in `main.cpp` and patch `CMakeLists.txt` accordingly.

— End
