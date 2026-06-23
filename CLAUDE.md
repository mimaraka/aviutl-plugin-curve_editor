# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 概要

AviUtl（拡張編集 0.92）向けのイージング作成・管理プラグイン（`.auf`）。C++ のネイティブプラグイン本体と、WebView2 上で動作する TypeScript/React 製の UI で構成される。配布物は `curve_editor.auf` 本体、`@Curve Editor.tra`、`curve_editor.lua`（スクリプトからカーブを呼び出すための連携ファイル）。

## ビルド

Visual Studio / MSBuild プロジェクト。ターゲットは **Win32 (x86)** が本番（AviUtl が 32bit のため）。`x64` 構成も存在するが配布対象は Win32。

- **言語/ツールセット**: C++20 (`stdcpp20`)、PlatformToolset `v145`、Windows SDK 10.0
- **本体ビルド**: `curve_editor.sln` を Visual Studio で開いてビルド、または:
  ```
  msbuild curve_editor.sln /p:Configuration=Release /p:Platform=Win32
  ```
- ビルドは MSBuild のイベントで以下を自動実行する（手動で個別に走らせる必要は基本ない）:
  - **PreBuild**: 依存ライブラリ `mkaul`（`external/aviutl-mkaul`）を先にビルド
  - **PostBuild**: `npx webpack --mode production` で UI をバンドル → `dist/js/bundle.js` を出力し、`index.html` と `curve_editor.lua` / `@Curve Editor.tra` を `$(OutDir)` にコピー

### UI 単体の開発

UI は `curve_editor/ui/`（webpack + ts-loader）。`package.json` に npm scripts は無いので webpack を直接叩く:

```
cd curve_editor/ui
npm install
npx webpack --mode development   # 開発用バンドル（本番は --mode production）
```

エントリは `src/index.tsx`、出力は `dist/js/bundle.js`。Lua スクリプトエディタに Monaco を使用（`monaco-editor-webpack-plugin` で言語を `lua` のみに限定）。

テストフレームワークは導入されていない。

## アーキテクチャ

### 全体構成: ネイティブ本体 ⇄ WebView2 UI

プラグインの GUI は HWND 上に貼り付けた **WebView2** で描画される。C++ 側がロジック・状態・永続化を担い、React UI は表示と入力を担当する。両者は2系統の経路で通信する:

1. **Host Object（同期 RPC、UI→C++ の主経路）**
   - C++ の `mkaul::ole::HostObject` 派生クラス（`host_object_*.hpp/.cpp`）が、`register_member()` でプロパティ／メソッドを JS に公開する。
   - `MyWebView2::add_host_object<T>()`（`my_webview2.hpp`）で WebView に登録。トップレベルの名前は `sync`。
   - TS 側は `interface.ts` で `window.chrome.webview.hostObjects.sync.{config,editor,preset}` として型付けされ、`config` / `editor` / `preset` をエクスポート。
   - Host Object の階層は C++ とほぼ鏡像: `EditorHostObject` → `graph`（`GraphEditorHostObject`）/ `script`、`graph` → `bezier` / `elastic` / `bounce` / `normal` / `numeric` など。**新しい C++ API を追加したら `interface.ts` の対応する interface も更新すること。**

2. **postMessage（非同期メッセージ、双方向）**
   - C++→UI: `MyWebView2::post_message(command, options)` / `send_command()`（`message_handler.*` 経由）。UI 初期化（`InitComponent`）や文字列テーブル送信（`LoadStringTable`）など。`src/index.tsx` がリスナのトップ。
   - 1つの HTML（`index.html` + `bundle.js`）を複数の用途で使い回し、`InitComponent` の `page` で `MainPanel` / `SelectDialog` / `CurveIdxSelector` を描き分ける。

### AviUtl プラグインのエントリポイント

- `dll_export.cpp` の `GetFilterTable()` が AviUtl フィルタ構造体を返す本体。`func_init` / `func_exit` / `func_WndProc` / `func_project_load` / `func_project_save` を登録。
- 各コールバックの実装は `filter_*.cpp/.hpp`（`filter_init`, `filter_exit`, `filter_wndproc`, `filter_project_load`, `filter_project_save`）に分離。
- `lua_export.cpp` がスクリプト（`.anm`/`.obj`/`.tra`/`curve_editor.lua`）からカーブ値を取得するための Lua 連携 API を提供。
- 拡張編集本体へのフック（トラックバー連携・D&D 適用など）は `exedit_hook.*`、`trackbar_button.*`、`drag_and_drop.*`。

### カーブのドメインモデル

中心は `global::editor`（`curve_editor.hpp` のシングルトン `CurveEditor`）。`config.get_edit_mode()` の `EditMode`（`enum.hpp`: Normal / Value / Bezier / Elastic / Bounce / Script）に応じて、内部の `GraphCurveEditor`（`editor_graph_`）か `ScriptCurveEditor`（`editor_script_`）へ委譲する。

- カーブ階層: 抽象基底 `Curve`（`curve.hpp`）→ `GraphCurve` / `NumericGraphCurve` → `BezierCurve` / `ElasticCurve` / `BounceCurve` / `LinearCurve` / `NormalCurve` / `ValueCurve` / `ScriptCurve`。各 `curve_*.cpp/.hpp` に対応。
- **Normal（標準）/ Script モード**: ID（`1`〜`CURVE_ID_MAX = 524288`）でカーブを管理し、プロジェクトファイル（`.aup`）に埋め込み保存される。
- **Bezier / Elastic / Bounce モード**: カーブを単一の整数値に `encode()` / `decode()` する（プロジェクトに保存せず、値そのものがカーブを表す）。
- **Modifier**（`modifier_*.cpp/.hpp`）: 区間カーブに重ねる変調（離散化・ノイズ・正弦波・矩形波）。
- カーブ名と `EditMode` の対応文字列は `constants.hpp`（`CURVE_NAME_*`）。

### 永続化

- **cereal** でシリアライズ。`CurveEditor::serialize()` がカーブ群を直列化し、`CEREAL_CLASS_VERSION` でバージョン管理。プロジェクト保存／読込は `filter_project_save` / `filter_project_load`。
- **設定**: `global::config`（`config.hpp` の `Config`、`config.json`）が編集モード・レイアウト・各モードの apply mode などのセッション状態を保持。永続的なユーザ設定（環境設定ダイアログ）は内部に持つ `Preferences`（`preferences.*`）。Host Object 経由で UI に公開（`ConfigHostObject`）。
- **プリセット**: `preset_manager.*` / `preset.*`、`PresetHostObject` で UI に公開。

### 主要な外部依存（`curve_editor/external/`、Git サブモジュール/ベンダリング）

- `aviutl-mkaul`: 自作の AviUtl プラグイン共通ライブラリ（`mkaul::` 名前空間。Host Object 基盤、描画、exedit 連携などを提供）。PreBuild で先にビルドされる。
- `aviutl_exedit_sdk`: 拡張編集 SDK。`cereal`: シリアライズ。`json` (nlohmann): postMessage の payload。`sol2`: Lua バインディング。`magic_enum`、`strconv`（SJIS⇄Unicode 変換）、`FastNoiseLite`。

## 補足

- 文字コード: AviUtl は SJIS。C++ 内の表示名はワイド文字列で持ち、AviUtl へ渡す箇所で `wide_to_sjis()`（`strconv2.h`）を使う。
- バージョンは `constants.hpp` の `PLUGIN_VERSION` で一元管理。
- スクリプト連携・トラブルシューティングの詳細は[リポジトリの Wiki](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki) を参照。
