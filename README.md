# Curve Editor for AviUtl

![GitHub](https://img.shields.io/github/license/mimaraka/aviutl-plugin-curve_editor)
![GitHub language count](https://img.shields.io/github/languages/count/mimaraka/aviutl-plugin-curve_editor)
![GitHub top language](https://img.shields.io/github/languages/top/mimaraka/aviutl-plugin-curve_editor)
![GitHub issues](https://img.shields.io/github/issues/mimaraka/aviutl-plugin-curve_editor)
![GitHub all releases](https://img.shields.io/github/downloads/mimaraka/aviutl-plugin-curve_editor/total)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/mimaraka/aviutl-plugin-curve_editor)

![graph2 7](https://user-images.githubusercontent.com/106879397/215339739-e6b130bf-41a5-421a-8f8a-7dc2b12e647d.png)

様々なイージングをウィンドウ上で編集することができる AviUtl 用のプラグイン・スクリプトです。

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

また、以下のプログラムをインストールしていない場合は、こちらも併せてインストールしてください。

- [Visual C++ 2015/2017/2019/2022 再頒布可能パッケージ (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)
- [Microsoft Edge WebView2 ランタイム (x86)](https://developer.microsoft.com/ja-jp/microsoft-edge/webview2/?form=MA13LH)

## 使用方法

AviUtl を起動後、メインウィンドウの「表示」メニューから「Curve Editor の表示」をクリックすると、Curve Editor のウィンドウが表示されます。 ([→ ウィンドウが表示されない場合](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0))

このプラグインでは、以下の 6 つの編集モードでカーブを編集することができます。

- **標準** (旧: ベジェ(複数))
- **値指定**
- **ベジェ**
- **振動**
- **バウンス**
- **スクリプト**

ウィンドウ左上のモード選択ボタンから、これらのモードを切り替えることができます。

- #### 標準 (旧: ベジェ(複数))

  1~524288 までの ID を使って、最大 524288 個のカーブをプロジェクトファイルに保存できます。

  このモードでは、直線・ベジェ・振動・バウンスの 4 種類のカーブを自由に組み合わせてカーブを作成することができます。  
   グラフ上の何もないところをダブルクリックすると制御点が追加され、制御点上で再度ダブルクリックすると削除されます。

- #### 値指定

- #### ベジェ

  ベジェ曲線のカーブを 1~10 桁の整数で識別します。値とカーブがそれぞれ対応しています。

- #### 振動

  ゴムの振動のような動きになります。  
   ベジェモードと同様に、カーブを整数によって識別します。負の値にするとカーブが反転します。

- #### バウンス

  物体がバウンドする動きになります。  
   ベジェモードと同様に、カーブを整数によって識別します。負の値にするとカーブが反転します。

- #### スクリプト
  Lua でスクリプトを記述してイージングを作成することができます。

### グラフビュー

以下の画像のようにして、ビューを移動させたり拡大縮小させたりすることができます。

![curve_editor_view](https://user-images.githubusercontent.com/106879397/208283665-2d22b1f4-3672-4c0c-a8b2-7b1d718b67c6.gif)

### ドラッグ&ドロップ

以下の画像のようにして、編集したカーブをドラッグ&ドロップでトラックバーに適用することができます。

![curve_editor_dandd](https://user-images.githubusercontent.com/106879397/208283022-ed88a2d9-66e0-41bb-8244-92a8adebc1db.gif)

※Shift キーを押しながらドラッグ&ドロップすることで、XYZ 座標などイージングが一度に設定されるトラックバーについても、X、Y、Z ごとに個別にイージングを設定することができます。

#### ドラッグ&ドロップができない場合は…

標準・値指定・ベジェ・スクリプトモードについては`Type1@Curve Editor`を、振動・バウンスモードについては`Type2@Curve Editor`をトラックバーのメニューから選択します。  
ベジェ・振動・バウンスモードの場合は右上のコピーボタンを押してコピーできるカーブの数値を、標準・値指定・スクリプトモードの場合は表示されている ID をパラメータ設定ダイアログに入力することで使用できます。

### その他

詳細な使用方法については、[このリポジトリの Wiki](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki)をご覧ください。

## プロジェクトファイルへのカーブの保存機能について

一部のモードのカーブは、プロジェクトファイル(`.aup`)への埋め込みが可能となっており、プロジェクト保存時に同時に保存されるようになっています。  
しかし、仕様上、**拡張編集標準のバックアップ機能では、これらのカーブを保存することはできません。**

そのため、カーブのデータを保持した状態でバックアップを行いたい場合は、代わりに[autosaver](https://github.com/epi5131/autosaver)プラグインを使用するようにしてください。

## スクリプトから使用する(スクリプト開発者向け)

スクリプト制御や、`.anm`、`.obj`、`.tra`などのスクリプトファイル上で、Curve Editor のカーブを呼び出して使用することができます。  
詳しくは、Wiki の[スクリプトからの使用](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%82%B9%E3%82%AF%E3%83%AA%E3%83%97%E3%83%88%E3%81%8B%E3%82%89%E3%81%AE%E4%BD%BF%E7%94%A8)を参照してください。

## 動作環境

|      OS      | AviUtl | 拡張編集 |
| :----------: | :----: | :------: |
| Windows 7~11 |  1.10  |   0.92   |

※[Visual C++ 2015/2017/2019/2022 再頒布可能パッケージ (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)および[Microsoft Edge WebView2 ランタイム (x86)](https://developer.microsoft.com/ja-jp/microsoft-edge/webview2/?form=MA13LH)が必要です。

## Q&A

Wiki の[トラブルシューティング](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0)を参照してください。

## バグ報告

バグ報告は[このリポジトリの Issues](https://github.com/mimaraka/aviutl-plugin-curve_editor/issues)または[Google Form](https://forms.gle/mhv96DSYVhhKPkYQ8)(匿名回答可)からお願いします。

## 免責事項

当プラグイン・スクリプトを使用したことによっていかなる損害が生じても、製作者は一切の責任を負わないものとします。
