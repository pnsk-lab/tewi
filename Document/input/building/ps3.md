\language ^en$
# Build for PlayStation 3 {Build}

Tewi HTTPd has the PlayStation 3 support.

## Steps

### 1. Install packages

It should be able to compile on Debian if below packages are installed:
 - `build-essential`
 - PS3Dev

### 2. Get the source code

You can get the source codes using several VCS. ([Official Repository List](repos.html))

### 3. Create config.h

Copy config.h.tmpl to config.h. Edit it if needed.

### 4. Run make

Run `make PLATFORM=ps3 PREFIX=prefix`. PREFIX is set to `/dev_hdd0/game/TEWI_00-0/USRDIR` by default.

\language ^jp$
# PlayStation 3向けにビルドする {ビルド}

Tewi HTTPdにはPlayStation 3サポートがあります。

## ステップ

### 1. パッケージを入れる

Debianの場合、以下のものがあればコンパイルできるはずです:
 - `build-essential`
 - PS3Dev

### 2. ソースコードを持って来る

ソースコードは色々なVCSを使って取得することができます。 ([公認リポジトリリスト](repos.html))

### 3. config.hを作成する

config.h.tmplをconfig.hにコピーしてください。必要があれば編集してください。

### 4. makeを実行する

`make PLATFORM=ps3 PREFIX=おすきなプレフィックス`を実行してください。PREFIXの指定が無い場合は`/dev_hdd0/game/TEWI_00-0/USRDIR`になります。
