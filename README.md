# Curve Editor for AviUtl

![GitHub](https://img.shields.io/github/license/mimaraka/aviutl-plugin-curve_editor)
![GitHub issues](https://img.shields.io/github/issues/mimaraka/aviutl-plugin-curve_editor)
![GitHub all releases](https://img.shields.io/github/downloads/mimaraka/aviutl-plugin-curve_editor/total)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/mimaraka/aviutl-plugin-curve_editor)

![graph2 7](https://user-images.githubusercontent.com/106879397/215339739-e6b130bf-41a5-421a-8f8a-7dc2b12e647d.png)  

様々なイージングをウィンドウ上で編集できるAviUtlプラグイン&スクリプトです。  

## ダウンロード
下のリンクから最新バージョンのzipファイル(`curve_editor-v*.zip`)をダウンロードしてください。  
[**ダウンロード**](https://github.com/mimaraka/aviutl-plugin-curve_editor/releases/latest)

## インストール
ダウンロードした`curve_editor-v*.zip`を解凍し、フォルダ内にある`curve_editor.auf`をAviUtlのディレクトリ直下もしくは`plugins`フォルダ内に入れ、
`@Curve Editor.tra`を`script`フォルダまたはその下のフォルダに入れてください。  
[Visual C++ 2015/2017/2019/2022 再頒布可能パッケージ (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)をインストールしていない場合は、こちらもインストールしてください。  

![image](https://user-images.githubusercontent.com/106879397/212220660-598f0b11-a317-42c7-99cb-0795f56af998.png)

## 使用方法
AviUtlを起動後、メインウィンドウの「表示」メニューから「Curve Editorの表示」をクリックすると、Curve Editorのウィンドウが表示されます。

本プラグインでは、以下で紹介する「**ベジェ**」「**ベジェ(複数)**」「**振動**」「**バウンス**」の4つの編集モードでカーブを編集することができます。  
ウィンドウ左上のモード選択ボタンまたは右クリックメニューの「編集モード」から4つのモードを切り替えることができます。

- #### ベジェ
    ベジェ曲線のカーブを1~10桁の整数で識別します。値とカーブがそれぞれ対応しています。

- #### ベジェ(複数)
    1~1024までのグラフのIDを使って、最大1024個のベジェ曲線のカーブをプロジェクトファイルに保存できます。  
    
    このモードでは、制御点を追加することで93氏のマルチベジェ軌道のように複雑なカーブを作成することもできます。  
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

#### ドラッグ&ドロップができない場合は…  
ベジェ・ベジェ(複数)モードについては`Type1@Curve Editor`を、振動・バウンスモードについては`Type2@Curve Editor`をトラックバーのメニューから選択します。  
ベジェ・振動・バウンスモードの場合は右上のコピーボタンを押してコピーできるカーブの数値を、ベジェ(振動)モードの場合は表示されているIDをパラメータ設定ダイアログに入力することで使用できます。


### その他
エディタ上で右クリックするとコンテクストメニューを開くことができるので、色々触りながら試してみてください。  
また、詳細な使用方法については、[このリポジトリのWiki](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki)をご覧ください。  

## プロジェクトファイルへのカーブの保存機能について
一部のモードのカーブはプロジェクトファイル(`.aup`)への埋め込みが可能となっており、プロジェクト保存時に同時に保存されるようになっています。  
しかし、仕様上、**拡張編集標準のバックアップ機能では、これらのカーブを保存することはできません。**

そのため、カーブのデータを保持した状態でバックアップを行いたい場合は、代わりに[autosaver](https://github.com/epi5131/autosaver)(epi氏)を使用するようにしてください。

## スクリプトから使用する(スクリプト開発者向け)
スクリプト制御や、`.anm`や`.obj`、`.tra`などのスクリプトファイル上で、Curve Editorのカーブを呼び出して使用することができます。  
詳しくはWikiの[スクリプトからの使用](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%82%B9%E3%82%AF%E3%83%AA%E3%83%97%E3%83%88%E3%81%8B%E3%82%89%E3%81%AE%E4%BD%BF%E7%94%A8)を参照してください。

## 動作環境
| OS | AviUtl | 拡張編集 | 
| :---: | :---: | :---: |
| Windows 7~11 | 1.00, 1.10 | 0.92~ (0.92推奨) |  

※[Visual C++ 2015/2017/2019/2022 再頒布可能パッケージ (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)が必要です。

【動作確認済み】  
Windows 10, 11  
AviUtl 1.00, 1.10  
拡張編集 0.92  

※[patch.aul](https://github.com/ePi5131/patch.aul)、
[イージング設定時短プラグイン](https://github.com/kumrnm/aviutl-easing-quick-setup)、
[イージング簡単選択](https://github.com/hebiiro/AviUtl-Plugin-SelectEasing)
との共存を確認。  

## Q&A
Wikiの[トラブルシューティング](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0)を参照してください。

## バグ報告
バグ報告は[このリポジトリのIssues](https://github.com/mimaraka/aviutl-plugin-curve_editor/issues)または[Google Form](https://forms.gle/mhv96DSYVhhKPkYQ8)(匿名回答可)からお願いします。  

## その他
当プラグイン・スクリプトは以下のプラグイン・スクリプトを参考にして作成されました：  
- [Flow](https://aescripts.com/flow/)
- [ベジェ軌道T](https://www.nicovideo.jp/watch/sm20632293)
- [Aulsグラフエディタ](https://auls.client.jp/)
- [イージング設定時短プラグイン](https://github.com/kumrnm/aviutl-easing-quick-setup)
- [イージング簡単選択](https://github.com/hebiiro/AviUtl-Plugin-SelectEasing)

## 免責事項
当プラグイン・スクリプトを使用したことによっていかなる損害が生じても、製作者は一切の責任を負わないものとします。  
