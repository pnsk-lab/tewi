\language ^en$
# Build for PlayStation Portable {Build}

Tewi HTTPd has the PlayStation Portable support.

## Steps

### 1. Install packages

It should be able to compile on Debian if below packages are installed:
 - `build-essential`
 - PSPDev

### 2. Get the source code

You can get the source codes using several VCS. ([Official Repository List](repos.html))

### 3. Create config.h

Copy config.h.tmpl to config.h. Edit it if needed.

### 4. Run make

Run `make PLATFORM=psp PREFIX=prefix`. PREFIX is set to `ms0:/PSP/GAME/httpd` by default.

\language ^jp$
# PlayStation Portable向けにビルドする {ビルド}

Tewi HTTPdにはPlayStation Portableサポートがあります。

## ステップ

### 1. パッケージを入れる

Debianの場合、以下のものがあればコンパイルできるはずです:
 - `build-essential`
 - PSPDev

### 2. ソースコードを持って来る

ソースコードは色々なVCSを使って取得することができます。 ([公認リポジトリリスト](repos.html))

### 3. config.hを作成する

config.h.tmplをconfig.hにコピーしてください。必要があれば編集してください。

### 4. makeを実行する

`make PLATFORM=psp PREFIX=おすきなプレフィックス`を実行してください。PREFIXの指定が無い場合は`ms0:/PSP/GAME/httpd`になります。
