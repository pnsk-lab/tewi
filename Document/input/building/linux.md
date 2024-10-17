\language ^en$
# Build for Linux {Build}

Tewi HTTPd has the Linux support, which is checked working on Debian.

## Steps

### 1. Install packages

It should work on Debian if below packages are installed:
 - `build-essential`
 - `libssl-dev` (If you want TLS/SSL)

### 2. Get the source code

You can get the source codes using several VCS. ([Official Repository List](repos.html))

### 3. Create config.h

Copy config.h.tmpl to config.h. Edit it if needed.

### 4. Run make

Run `make PLATFORM=linux PREFIX=prefix`. PREFIX is set to `/usr/local` by default.

### 5. Install

Run `make PLATFORM=linux PREFIX=prefix install` as **root user**.

\language ^jp$
# Linux向けにビルドする {ビルド}

Tewi HTTPdにはDebianで動作確認がされているLinuxサポートがあります。

## ステップ

### 1. パッケージを入れる

Debianの場合、以下のものがあれば動くはずです:
 - `build-essential`
 - `libssl-dev` (TLS/SSL機能が必要なら)

### 2. ソースコードを持って来る

ソースコードは色々なVCSを使って取得することができます。 ([公認リポジトリリスト](repos.html))

### 3. config.hを作成する

config.h.tmplをconfig.hにコピーしてください。必要があれば編集してください。

### 4. makeを実行する

`make PLATFORM=linux PREFIX=おすきなプレフィックス`を実行してください。PREFIXの指定が無い場合は`/usr/local`になります。

### 5. インストールする

`make PLATFORM=linux PREFIX=おすきなプレフィックス install`を**root**ユーザーで実行してください。