# SwissSystemTournament
スイス式トーナメントを実装する

## 概要
スイス式トーナメントとは、`N`人のプレイヤーによる戦いを 各人 `log_2 N + α`回程の勝負でプレイヤー全員の順位を決定する。
総当たりでは試合が多すぎる、ただのトーナメントでは早い段階で負けてしまったプレイヤーがつまらない、それらを解決するのが、このスイス式トーナメントである。

## 本アプリの使い方
これ単体では動かす気はあまりないが動かせる。

1. プレイヤーの数、~~試合数~~（任意の試合数ができるようになった）、**プレイヤーの名前・レート（未実装）** を~~標準~~入力  
2. マッチング処理を行い、完了するとJSONファイル(`$matchingname/matching_x.json`)にマッチング結果を出力
3. そのJSONファイルに試合結果（勝ち数、負け数、引き分け数、不戦勝、不戦敗）を入力し、結果を報告**（JSONエディタは未実装）**
4. 試合結果から計算し、プレイヤーの順位をhtml(`$matchingname/result_x.html`)、プレイヤーのデータをjson(`$matchingname/data_x.json`)で出力、2.へ戻る。

GUIを実装予定。

## 使用ライブラリ
+ [picojson](https://github.com/kazuho/picojson)
+ [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser)
