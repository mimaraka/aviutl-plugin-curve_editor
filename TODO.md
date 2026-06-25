# TODO（今後の修正候補）

コードベース調査（2026-06-25）で洗い出した、修正を見送った問題点。
修正済みのバグは git 履歴を参照。各項目は確認時点のファイル・行番号を記載しているため、着手前に現状を確認すること。

---

## `ValueCurve` のコピーコンストラクタが基底 `GraphCurve` を初期化しない

- **場所:** `curve_editor/curve_value.cpp:13`
- **内容:** `ValueCurve(const ValueCurve&)` がメンバ初期化子で基底 `GraphCurve` を渡しておらず、アンカー座標やモディファイア等がコピー元から引き継がれず既定値になる。コピー代入演算子 (`operator=`) は `GraphCurve::operator=` を呼んでいるため、コピーコンストラクタとの挙動が非対称。
- **優先度:** 低（ValueCurve は未実装の WIP）

- [ ] コピーコンストラクタで `GraphCurve{ curve }` を初期化子に追加する
- [ ] ValueCurve 本体の実装に合わせてカーブ内容のコピーを実装する（ソース内 TODO コメントと対）
- [ ] コピー → 比較で内容が一致することを確認する

## `ScriptCurve::curve_function` がサンプル毎に Lua を再生成・再コンパイルする

- **場所:** `curve_editor/curve_script.cpp:19, 86`
- **内容:** カーブ値取得のたびに `sol::state` を新規生成し、`lua.script(...)` でスクリプト全体を再コンパイルしている。描画では1フレームあたり多数のサンプルで `get_value` が呼ばれるため、重大な性能劣化になる。
- **優先度:** 中（機能は動くが体感性能に影響）

- [ ] スクリプト文字列が変わったときだけ再コンパイルする仕組み（ハッシュ/フラグ）を設計する
- [ ] コンパイル済みチャンク（`sol::load` の結果）または `sol::state` をキャッシュする
- [ ] レンダリングのマルチスレッド化を考慮し、`thread_local` 化または同期を入れる（A-4 と同種のスレッド安全性に注意）
- [ ] 大量サンプル描画で性能が改善することを計測・確認する

## `UpdateChecker::check_for_update` の二重呼び出しで `std::terminate`

- **場所:** `curve_editor/update_checker.cpp:30`、`curve_editor/update_checker.hpp:20`
- **内容:** `th_ = std::thread{...}` を実行する際、既に `th_` が joinable（前回のスレッドが未 join）だと代入時に `std::terminate` する。現状は `filter_init` で1回しか呼ばれないため顕在化しないが、再入防止がない。
- **優先度:** 低（現状は単回呼び出し）

- [ ] `check_for_update` 冒頭に `if (th_.joinable()) return;`（または前スレッドの join）ガードを追加する
- [ ] 複数回呼んでも `std::terminate` しないことを確認する

## コレクション import/export がファイルパスを SJIS に変換している

- **場所:** `curve_editor/message_handler.cpp:960`（import）、`:1077`（export）
- **内容:** `IShellItem::GetDisplayName` で得た Unicode パス (`PWSTR`) を `wide_to_sjis` で変換してから `import_collection` / `export_collection`（`std::filesystem::path` 引数）へ渡している。SJIS で表現できない文字を含むパスでは文字化け・ファイルアクセス失敗が起きうる。`PresetManager` 側は既に `std::filesystem::path` を受けるため、呼び出し側の変換を外すだけで対応可能。
- **優先度:** 中（環境依存で再現する実害あり）

- [ ] import 呼び出し（`message_handler.cpp:960`）の `wide_to_sjis` を外しワイドパスを渡す
- [ ] export 呼び出し（`message_handler.cpp:1077`）の `wide_to_sjis` を外しワイドパスを渡す
- [ ] 非 SJIS 文字を含むフォルダでの import/export が成功することを確認する

## `BounceCurve::curve_function` の基準値が `anchor_end().y`（Elastic は `anchor_start().y`）

- **場所:** `curve_editor/curve_bounce.cpp:88`
- **内容:** `return start + (end - start) * (anchor_end().y + (anchor_end().y - anchor_start().y) * ret);` と、基準項が `anchor_end().y` になっており、Elastic の `anchor_start().y` と非対称。`ret` のセマンティクスが両者で異なる（Bounce は終端で `ret=0`）ため意図的なカーブ定義の可能性が高いが、未検証。
- **優先度:** 低（要視覚確認、誤りと断定できていない）

- [ ] 反転あり/なし・始終点を入れ替えたケースを AviUtl 上で目視確認する
- [ ] 意図どおりか判断し、誤りであれば基準項を見直す

## `Config::load_json` の `operator[]` による副作用

- **場所:** `curve_editor/config.cpp:164`
- **内容:** nlohmann json の `operator[]`（`data[GET_KEY(...)]`）を使っており、存在しないキーで null 要素を挿入する副作用がある。クラッシュには至らないが改善余地あり。
- **優先度:** 低

- [ ] `contains()` / `find()` による存在確認に置き換える
- [ ] キーが無い設定ファイルでも従来どおり読み込めることを確認する
