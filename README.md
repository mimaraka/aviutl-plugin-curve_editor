# Curve Editor for AviUtl

![GitHub](https://img.shields.io/github/license/mimaraka/aviutl-plugin-curve_editor)
![GitHub language count](https://img.shields.io/github/languages/count/mimaraka/aviutl-plugin-curve_editor)
![GitHub top language](https://img.shields.io/github/languages/top/mimaraka/aviutl-plugin-curve_editor)
![GitHub issues](https://img.shields.io/github/issues/mimaraka/aviutl-plugin-curve_editor)
![GitHub all releases](https://img.shields.io/github/downloads/mimaraka/aviutl-plugin-curve_editor/total)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/mimaraka/aviutl-plugin-curve_editor)

![graph3 1](https://github.com/user-attachments/assets/74930f51-99d3-4ce9-bafb-778a3476e864)

様々なイージングを作成・管理することができる AviUtl 用のプラグイン・スクリプトです。

## ダウンロード

下のリンクから最新バージョンの zip ファイル(`curve_editor-v*.zip`)をダウンロードしてください。  
[**ダウンロード**](https://github.com/mimaraka/aviutl-plugin-curve_editor/releases/latest)

## インストール

ダウンロードした`curve_editor-v*.zip`を解凍し、以下の内容物をそれぞれ適切な場所に配置します。

|                     内容物                     |                                                                 移動先                                                                  |
| :--------------------------------------------: | :-------------------------------------------------------------------------------------------------------------------------------------: |
| `curve_editor.auf`<br>`curve_editor`(フォルダ) | AviUtl のディレクトリ直下 / `plugins`フォルダ / `plugins`フォルダの 1 階層下のフォルダ<br>のいずれか (両者は同じ場所に配置してください) |
|              `@Curve Editor.tra`               |                                              `script`フォルダまたはその 1 階層下のフォルダ                                              |
|               `curve_editor.lua`               |                                                       `exedit.auf`があるフォルダ                                                        |

![installation](https://github.com/user-attachments/assets/8edff440-b22f-45fc-a930-cac3285cd805)

> [!NOTE]
> 更新の際は、すべてのファイル・フォルダを上書きして配置してください。

> [!NOTE]
> 正常に動作しない場合は、以下のプログラムがコンピュータにインストールされているか確認してください。
>
> - [Visual C++ 2015/2017/2019/2022 再頒布可能パッケージ (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)
> - [Microsoft Edge WebView2 ランタイム (x86)](https://developer.microsoft.com/ja-jp/microsoft-edge/webview2/?form=MA13LH)

## 使用方法

AviUtl を起動後、メインウィンドウの「表示」メニューから「Curve Editor の表示」をクリックすると、Curve Editor のウィンドウが表示されます。 ([→ ウィンドウが表示されない場合](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0))

このプラグインでは、以下の 5 つの編集モードでカーブを編集することができます。

- **標準** (旧: ベジェ(複数))
- **ベジェ**
- **振動**
- **バウンス**
- **スクリプト**

ウィンドウ左上のモード選択ドロップダウンから、これらのモードを切り替えることができます。

- #### 標準 (旧: ベジェ(複数))

  `1`~`524288` までの ID を使って、最大 524288 個のカーブをプロジェクトファイルに保存できます。

  このモードでは、直線・ベジェ・振動・バウンスの 4 種類のカーブを自由に組み合わせてカーブを作成することができます。  
  グラフ上の何もないところをダブルクリックするとアンカーが追加されます。
  カーブを削除するときは、アンカーをダブルクリックします。

- #### ベジェ

  単一の 3 次ベジェ曲線を作成することができるモードです。  
  カーブを数桁の整数値で識別しており、値とカーブが 1 対 1 で対応しています。

- #### 振動

  ゴムの振動のような動きを作成できるモードです。  
  ベジェモードと同様に、カーブを整数によって識別します。負の値にするとカーブが反転します。

- #### バウンス

  物体がバウンドする動きを作成できるモードです。  
  ベジェモードと同様に、カーブを整数によって識別します。負の値にするとカーブが反転します。

- #### スクリプト

  Lua スクリプトにより、イージングの関数を直接記述することができるモードです。  
  標準モードと同様に、最大 524288 個のカーブを追加できます。

### グラフビュー

マウスホイールを回転させて、でビューを拡大縮小することができます。

マウスホイールをドラッグする、または、 `Alt` キーを押しながら左ボタンでドラッグすることで、ビューの位置を変更することができます。

### ドラッグ&ドロップ

「適用」ボタンを押すと、ボタンが「トラックバーにドラッグ&ドロップして適用」という表示に変化します。  
そのままマウスの左ボタンを離さずに、トラックバーの移動方法変更ボタンまでドラッグします。

カーソルが移動方法変更ボタンにホバーすると、ボタンがハイライト表示されます。  
カーブを適用したいトラックバーの上でドロップすると、カーブが適用されます。

[→ ドラッグ&ドロップができない場合](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0)

> [!TIP]
> Shift キーを押しながらドラッグ&ドロップすることで、XYZ 座標など、イージングが一度に設定されるトラックバーについても、各座標ごとに個別にイージングを設定することができます。

### その他

詳細な使用方法については [Wiki](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki) を参照してください。。

## プロジェクトファイルへのカーブの保存機能について

一部のモードのカーブは、プロジェクトファイル(`.aup`)への埋め込みが可能となっており、プロジェクト保存時に同時に保存されるようになっています。  
しかし、仕様上、**拡張編集標準のバックアップ機能では、これらのカーブを保存することはできません。**

そのため、カーブのデータを保持した状態でバックアップを行いたい場合は、以下のいずれかのプラグインを導入してください。

- [autosaver (オリジナル)](https://github.com/epi5131/autosaver)
- [autosaver (Garech フォーク版)](https://github.com/Garech-mas/autosaver)

## スクリプトから使用する(スクリプト開発者向け)

スクリプト制御や、`.anm`、`.obj`、`.tra`などのスクリプトファイル上で、Curve Editor のカーブを呼び出して使用することができます。  
詳しくは、[API リファレンス (Lua スクリプト)](<https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/API%E3%83%AA%E3%83%95%E3%82%A1%E3%83%AC%E3%83%B3%E3%82%B9-(Lua%E3%82%B9%E3%82%AF%E3%83%AA%E3%83%97%E3%83%88)>)を参照してください。

## 動作環境

|      OS      | AviUtl | 拡張編集 |
| :----------: | :----: | :------: |
| Windows 7~11 |  1.10  |   0.92   |

> [!IMPORTANT]
> 以下のプログラムのインストールが必要です。
>
> - [Visual C++ 2015/2017/2019/2022 再頒布可能パッケージ (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)
> - [Microsoft Edge WebView2 ランタイム (x86)](https://developer.microsoft.com/ja-jp/microsoft-edge/webview2/?form=MA13LH)

## Q&A

Wiki の[トラブルシューティング](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0)を参照してください。

## バグ報告

バグ報告は [Issues](https://github.com/mimaraka/aviutl-plugin-curve_editor/issues) または [Google Form](https://forms.gle/mhv96DSYVhhKPkYQ8) (匿名回答可)からお願いします。

## 免責事項

当プラグイン・スクリプトを使用したことによっていかなる損害が生じても、製作者は一切の責任を負わないものとします。
