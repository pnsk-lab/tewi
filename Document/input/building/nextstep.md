\language ^en$
# Build for NeXTSTEP {Build}

Tewi HTTPd has the NeXTSTEP support, which is checked working on NeXTSTEP 3.3.

## Steps

### 1. Install packages

It should work on 3.3 if below stuffs are installed:
 - Developer kit
 - GNU make

### 2. Get the source code

You can get the source codes using several VCS. ([Official Repository List](repos.html))

### 3. Create config.h

Copy config.h.tmpl to config.h. Edit it if needed.

### 4. Run make

Run `make PLATFORM=nextstep PREFIX=prefix`. PREFIX is set to `/usr/local` by default.

### 5. Install

Run `make PLATFORM=nextstep PREFIX=prefix install` as **root user**.

\language ^jp$
# NeXTSTEP向けにビルドする {ビルド}

Tewi HTTPdにはNeXTSTEP 3.3で動作確認がされているNeXTSTEPサポートがあります。

## ステップ

### 1. パッケージを入れる

3.3の場合、以下のものがあれば動くはずです:
 - Developer kit
 - GNU make

### 2. ソースコードを持って来る

ソースコードは色々なVCSを使って取得することができます。 ([公認リポジトリリスト](repos.html))

### 3. config.hを作成する

config.h.tmplをconfig.hにコピーしてください。必要があれば編集してください。

### 4. makeを実行する

`make PLATFORM=nextstep PREFIX=おすきなプレフィックス`を実行してください。PREFIXの指定が無い場合は`/usr/local`になります。

### 5. インストールする

`make PLATFORM=nextstep PREFIX=おすきなプレフィックス install`を**root**ユーザーで実行してください。
