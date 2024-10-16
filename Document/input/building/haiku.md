\language ^en$
# Build for Haiku {Build}

Tewi HTTPd has the Haiku support.

## Steps

### 1. Install packages

There are no required packages, they should be installed by default.

### 2. Get the source code

You can get the source codes using several VCS. ([Official Repository List](repos.html))

### 3. Create config.h

Copy config.h.tmpl to config.h. Edit it if needed.

### 4. Run make

Run `make PLATFORM=haiku PREFIX=prefix`. PREFIX is set to `/usr/local` by default.

### 5. Install

Run `make PLATFORM=haiku PREFIX=prefix install` as **root user**.

\language ^jp$
# Haiku向けにビルドする {ビルド}

Tewi HTTPdにはHaikuサポートがあります。

## ステップ

### 1. パッケージを入れる

追加で必要なパッケージはありません。デフォルトでインストールされているはずです。

### 2. ソースコードを持って来る

ソースコードは色々なVCSを使って取得することができます。 ([公認リポジトリリスト](repos.html))

### 3. config.hを作成する

config.h.tmplをconfig.hにコピーしてください。必要があれば編集してください。

### 4. makeを実行する

`make PLATFORM=haiku PREFIX=おすきなプレフィックス`を実行してください。PREFIXの指定が無い場合は`/usr/local`になります。

### 5. インストールする

`make PLATFORM=haiku PREFIX=おすきなプレフィックス install`を**root**ユーザーで実行してください。
