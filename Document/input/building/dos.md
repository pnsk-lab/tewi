\language ^en$
# Build for DOS {Build}

Tewi HTTPd has the DOS support.

## Steps

### 1. Install packages

It should be able to compile on Debian if below packages are installed:
 - `build-essential`
 - Open Watcom 2.0

### 2. Get the source code

You can get the source codes using several VCS. ([Official Repository List](repos.html))

### 3. Create config.h

Copy config.h.tmpl to config.h. Edit it if needed.

### 4. Run make

Run `make PLATFORM=dos PREFIX=prefix`. PREFIX is set to `C:/Tewi` by default.

\language ^jp$
# DOS向けにビルドする {ビルド}

Tewi HTTPdにはDOSサポートがあります。

## ステップ

### 1. パッケージを入れる

Debianの場合、以下のものがあればコンパイルできるはずです:
 - `build-essential`
 - Open Watcom 2.0

### 2. ソースコードを持って来る

ソースコードは色々なVCSを使って取得することができます。 ([公認リポジトリリスト](repos.html))

### 3. config.hを作成する

config.h.tmplをconfig.hにコピーしてください。必要があれば編集してください。

### 4. makeを実行する

`make PLATFORM=dos PREFIX=おすきなプレフィックス`を実行してください。PREFIXの指定が無い場合は`C:/Tewi`になります。
