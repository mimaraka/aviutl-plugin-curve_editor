# Curve Editor v1.0.1 for AviUtl
![graph](https://user-images.githubusercontent.com/106879397/208284126-54f9caf2-8525-4a1b-8c89-6e3726afe3c1.png)  

様々なイージングをウィンドウ上で編集できるAviUtlプラグイン&スクリプトです。  

## ダウンロード
最新バージョン: **v1.0.1**

<a href="https://github.com/mimaraka/aviutl-plugin-curve_editor/releases/download/v1.0.1/curve_editor-v1.0.1.zip">![Download](https://user-images.githubusercontent.com/106879397/198503637-3f0ca637-08cf-47b0-afdc-10f9974c7b37.png)</a>

<br/>

**⚠注意⚠(Alpha版を使用していた方々へ)**  
v1.0では大きな仕様変更がされており、前のバージョンとの互換性がなくなっています。  
これまでのAlpha版を使用していた場合、作成したカーブ等はv1.0以降では読み込むことができません。申し訳ございません。  
ご理解の程よろしくお願いいたします。

## インストール
同梱されている`curve_editor.auf`を`exedit.auf`のあるフォルダもしくは`plugins`フォルダ内に入れ、`@Curve Editor.tra`を`script`フォルダ**直下**に入れてください。  
(※scriptフォルダ直下に入れないと、ドラッグ&ドロップ機能を使用することができません。)  
[Visual C++ 2015/2017/2019/2022 再頒布可能パッケージ (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)をインストールしていない場合は、こちらもインストールしてください。  

## 使用方法
AviUtlを起動後、メインウィンドウの「表示」メニューから「Curve Editorの表示」をクリックすると、Curve Editorのウィンドウが表示されます。

本プラグインでは、以下で紹介する「**ベジェ**」「**ベジェ(複数)**」「**振動**」「**バウンス**」の4つの編集モードでカーブを編集することができます。  
ウィンドウ左上のモード選択ボタンまたは右クリックメニューの「編集モード」から4つのモードを切り替えることができます。

- #### ベジェ
    このモードでは、カーブを1~10桁の整数で識別します。値とカーブがそれぞれ対応しています。

- #### ベジェ(複数)
    1~1024までのグラフのIDを使って、最大1024個のカーブをプロジェクトファイルに保存できます。  
    
    このモードでは、制御点を追加することでマルチベジェ軌道のように複雑なカーブを作成することもできます。  
    グラフ上の何もないところをダブルクリックすると制御点が追加され、制御点上で再度ダブルクリックすると削除されます。  

- #### 振動
    ゴムの振動のような動きになります。  
    ベジェモードと同様に、カーブを整数によって識別します。負の値にするとカーブが反転します。

- #### バウンス
    物体がバウンドする動きになります。  
    ベジェモードと同様に、カーブを整数によって識別します。負の値にするとカーブが反転します。
    
### グラフビュー
以下の画像のようにして、ビューを移動させたり拡大縮小させたりすることができます。

![curve_editor_view](https://user-images.githubusercontent.com/106879397/208283665-2d22b1f4-3672-4c0c-a8b2-7b1d718b67c6.gif)

### ドラッグ&ドロップ
以下の画像のようにして、編集したカーブをドラッグ&ドロップでトラックバーに適用することができます。

![curve_editor_dandd](https://user-images.githubusercontent.com/106879397/208283022-ed88a2d9-66e0-41bb-8244-92a8adebc1db.gif)

※Shiftキーを押しながらドラッグ&ドロップすることで、XYZ座標などイージングが一度に設定されるトラックバーについても、X、Y、Zごとに個別にイージングを設定することができます。

※また、Ctrlキーを押しながらドラッグ&ドロップすると、中間点無視のイージングが適用されます。

#### ドラッグ&ドロップができない場合は…  
ベジェ・ベジェ(複数)モードについては`Type1@Curve Editor`を、振動・バウンスモードについては`Type2@Curve Editor`をトラックバーのメニューから選択します。  
ベジェ・振動・バウンスモードの場合は右上のコピーボタンを押してコピーできるカーブの数値を、ベジェ(振動)モードの場合は表示されているIDをパラメータ設定ダイアログに入力することで使用できます。


### その他
その他の詳しい使用方法については、[Wiki](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki)をご覧ください。  

## スクリプトから使用する(スクリプト開発者向け)
以下のようにして`curve_editor`を読み込み、`getcurve()`関数を使用することでスクリプトからカーブの値を取得することができます。  

```lua
local modname="curve_editor"
if not package.loaded[modname] then
	package.preload[modname]=package.loadlib(modname .. ".auf","luaopen_" .. modname)
	require(modname)
	package.preload[modname]=nil
end
```
```
curve_editor.getcurve(モード番号, パラメータ値, 進捗(0~1), 開始値, 終了値)
```

モード番号については以下のようになっています：  
| 番号 | モード |
| :---: | :---: |
| 0 | ベジェ |
| 1 | ベジェ(複数) |
| 2 | (未実装) |
| 3 | 振動 |
| 4 | バウンス |

パラメータ値については、数値を指定するものであれば「カーブの数値をコピー」からコピーできる値を、IDを指定するものであればIDを入力します。

## 動作環境
| OS | AviUtl | 拡張編集 | 
| :---: | :---: | :---: |
| Windows 7~11 | 1.00, 1.10 | ~0.93rc(0.92推奨) |  

※[Visual C++ 2015/2017/2019/2022 再頒布可能パッケージ (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)が必要です。

【動作確認済み】  
Windows 10, 11  
AviUtl 1.00, 1.10  
拡張編集 0.92  

※[patch.aul](https://github.com/ePi5131/patch.aul)、
[イージング設定時短プラグイン](https://github.com/kumrnm/aviutl-easing-quick-setup)、
[イージング簡単選択](https://github.com/hebiiro/AviUtl-Plugin-SelectEasing)との共存を確認。  

## バグ報告
バグ報告はこのリポジトリのIssuesまたはTwitter([@mimaraka](https://twitter.com/mimaraka))のDMにてお願いします。  

## 免責事項
当プラグイン・スクリプトを使用したことによっていかなる損害が生じても、製作者は一切の責任を負わないものとします。  
