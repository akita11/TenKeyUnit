# TenKey UNIT

<img src="https://github.com/akita11/TenKeyUnit/blob/main/TenKeyUNIT.jpg" width="240px">

<img src="https://github.com/akita11/TenKeyUnit/blob/main/TenKeyUNIT_conn.jpg" width="240px">

<img src="https://github.com/akita11/TenKeyUnit/blob/main/TenKeyUNIT_board.jpg" width="240px">

<img src="https://github.com/akita11/TenKeyUnit/blob/main/TenKeyUNIT_board_back.jpg" width="240px">

GroveコネクタでM5Stack等の接続できる、数字の入力に便利なテンキーです。M5Stackの[CardKB Mini](https://www.switch-science.com/products/8445)と制御方法は互換ですので、CardKB Mini用のライブラリやUIFlowブロックを使用することができます。


## 使い方

M5Stack等のマイコンとGroveコネクタとをGroveケーブルで接続します。起動時にLEDが光ります（LEDつき版のみ）。キーを押すとそのキーのLEDがフラッシュします（LEDつき版のみ）。押したキーの情報(ASCIIコード)はGrove経由(I2C通信)で取得できます。


## 技術的詳細

### Grove経由の制御方法

通信方式はI2Cで、スレーブデバイス（I2Cアドレス=0x5f(7bit表記)）として動作します。マスタ側からI2C Readを行うと、直近で押したキーのASCIIコードを返します。

### 制御マイコン

制御マイコンはSTCmicro社のSTC8G08K-SOP16です。STCmicro社の書き込みツールをつかってファームウエアの書き換えを行うことができます。なおファームウエアのコンパイル方法や書き込み方法は、[STC8G_ProMini](https://www.switch-science.com/products/8216)のサポートページ等の情報を参照してください。

## 組立時の注意事項

プレート板（キースイッチ固定用）は表と裏があります。キースイッチがはまる穴が、基板上のキースイッチの位置にくる向きにしてください。またスペーサは2個重ねて（高さ3mmになります）ネジで固定します。

## Author

Junichi Akita (@akita11) / akita@ifdl.jp
