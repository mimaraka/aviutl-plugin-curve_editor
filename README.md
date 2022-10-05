# Curve Editor v0.1.1 for AviUtl
![スクリーンショット 2022-10-01 170054](https://user-images.githubusercontent.com/106879397/193399542-45d1b0ae-7eac-4150-aeb5-5678cb52e802.png)  
ベジェ曲線のイージングをウィンドウ上で編集できるAviUtlプラグインです。  

## ダウンロード
**v0.1.1**(テスト版)：https://github.com/mimaraka/aviutl-plugin-curve_editor/releases/tag/v0.1.1  
※カーブを編集するUIと、カーブの適用・プロジェクトファイルへの保存機能が実装されています。

## インストール
同梱されている「curve_editor.auf」をaviutl.exeのあるフォルダもしくはpluginsフォルダ内に入れ、「@Curve Editor.tra」をscriptフォルダまたはその下のフォルダに入れてください。

## 使用方法
AviUtlを起動後、メインウィンドウの「表示」メニューから「Curve Editorの表示」をクリックすると、Curve EditorのGUIが表示されます。

本プラグインでは、以下で紹介する「Valueモード」と「IDモード」の2つの編集モードでカーブを編集することができます。

- #### Valueモード
    Valueモードでは、カーブを1~10桁の整数で識別します。値とカーブがそれぞれ対応しています。

    ウィンドウ下部の「Copy」ボタンをクリックすると、クリップボードにカーブの値がコピーされます。アニメーションカーブを設定したいパラメータの移動方法に「Value@Curve Editor」を選択し、「移動フレーム間隔」の数値入力欄でコピーした値を貼り付けるとカーブが適用されます。

- #### IDモード
    IDモードでは、0~255までのグラフのIDを使って、最大256個のカーブをプロジェクトファイルに保存できます。  
    ベジェ軌道スクリプトと同様に、パラメータにIDを入力することで作成したイージングが使用できます。  
    
    このモードでは、制御点を追加することでマルチベジェ軌道のように複雑なカーブを作成することもできます。  
    グラフ上の何もないところをダブルクリックすると制御点が追加され、制御点上で再度ダブルクリックすると削除されます。  

| | Valueモード | IDモード |
| :---: | :---: | :---: |
| マルチベジェ軌道 | × | 〇|
| カーブの一括変更 | × | 〇|
| exa, exo出力 | 〇 | ×|
| .flowファイル読み込み | 〇 | 〇|
| .flowファイル書き出し | 〇 | ×|
| .cepファイル読み込み | 〇 | 〇|
| .cepファイル書き出し | 〇 | 〇|
| ライブラリから直接D&D | 〇 | ×|
| その他 | 制御点のY座標に制限あり | カーブの個数に制限あり|

カーブエディタ内の"Value/ID"のスイッチをクリックすることで2つのモードを切り替えることができます。

### グラフ
[Shift]や[Ctrl]、[Alt]のキーを押しながらドラッグすると次のような操作をすることができます。(Valueモードのみ)  
| [Shift] | [Alt] | [Ctrl] | [Shift]+[Ctrl] |
| :---: | :---: | :---: | :---: |
| 制御点をグラフ上下の辺にスナップ | ハンドルの角度を固定 | ハンドルの長さを固定 | 2つの制御点を同時に動かす |

### 右クリックメニュー
カーブエディタ内で右クリックをすると次のようなオプションを選択できます：

#### 値をコピー(Valueモードのみ)
カーブの値をクリップボードにコピーします。Copyボタンと動作は同じです。

#### 保存
カーブをプリセットとして保存します。ボタンと動作は同じです。

#### 値をコピー(4次元)
カーブの値を`0.70,0.27,0.16,1.00`の形式でクリップボードにコピーします。

#### カーブを反転
カーブが、グラフの中心について点対称の形になります。つまり、EaseInのカーブはEaseOutに、EaseOutのカーブはEaseInになります。

### ショートカットキー
カーブ編集時には、以下のショートカットキーが使用できます：

#### [A]キー
ペアのハンドルとの角度を固定するかどうかを切り替えます。

#### [F]キー
カーブをグラフウィンドウにフィットさせます。

#### [R]キー
カーブを反転させます。

#### [S]キー
カーブをプリセットに保存します。(未実装)

#### [<] / [>]キー
前/次のIDのカーブに移動します。

### 値を読み取る
矢印マークのボタンをクリックし，変換後の値を入力するとグラフにカーブが反映されます。(Valueモードのみ)

### ライブラリへ保存
マークのボタンをクリックすると設定したカーブをプリセットとして保存でき，プリセットウィンドウから参照することができます。   
設定したプリセットは「curve_editor.bin」に保存されます。

## プリセット

## 設定

## その他
### Valueモードの注意点
- グラフでの制御点のY座標(グラフの幅を1としたとき)は-2.74~3.74まで有効です。これより外の値は範囲外となり設定できません。  

- Valueモードではグラフの一括変更ができないため，カーブを使い回す場合はあらかじめ調整をしてからの方が良いかもしれません。  

- Valueモードでは小数点以下2桁刻みでカーブを設定するため，.flowファイルを読み込む際，値が小数点以下3桁まで記述されている場合は四捨五入されます。  

### IDモードの注意点
- IDモードで設定したカーブは，エイリアス(.exa)やオブジェクトファイル(.exo)として出力しても保存されません。  

- IDモードでは，.flowファイルの読み込みはできますが書き出しができない仕様となっています。  

- Value@Curve Editorはプラグインと連動していないため，プラグインを入れていなくても動作します。   

## 動作環境
| OS | AviUtl | 拡張編集 | 
| :---: | :---: | :---: |
| Windows 7~11 | ~1.10 | ~0.93rc(0.92推奨) |

【動作確認済み】  
Windows 11  
AviUtl 1.00, 1.10  
拡張編集 0.92  

## 免責事項
当プラグイン・スクリプトを使用したことによっていかなる損害が生じても，製作者は一切の責任を負わないものとします。  


