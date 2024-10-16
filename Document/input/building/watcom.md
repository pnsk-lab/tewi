\noindex
\language ^en$

# Using Open Watcom 2.0 {Build}

This page explains how to build using Open Watcom 2.0.

## Steps

### 1. Requirements
 - Open Watcom 2.0
 - Linux or NetBSD environment

### 2. Get the source code

You can get the source codes using several VCS. ([Official Repository List](repos.html))

### 3. Create config.h

Copy config.h.tmpl to config.h. Edit it if needed.

### 4. Build

Run `./make-installer.sh watcom`, and you should get these files:
 - install.exe (Setup program)
 - tewi.7z (Archive)

\language ^jp$

# Open Watcom 2.0を使う {ビルド}

ここではOpen Watcom 2.0を使用してコンパイルする方法を紹介します。

## ステップ

### 1. 必要なもの
 - Open Watcom 2.0
 - Linux環境かNetBSD環境

### 2. ソースコードを持って来る

ソースコードは色々なVCSを使って取得することができます。 ([公認リポジトリリスト](repos.html))

### 3. config.hを作成する

config.h.tmplをconfig.hにコピーしてください。必要があれば編集してください。

### 4. ビルドする

`./make-installer.sh watcom`を実行してください。そしたら以下のものが作成されるはずです:
 - install.exe (セットアッププログラム)
 - tewi.7z (アーカイブ)
