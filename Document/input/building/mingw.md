\noindex
\language ^en$

# Using MinGW-w64 {Build}

This page explains how to build using MinGW-w64.

## Steps

### 1. Requirements
 - MinGW-w64
 - Linux or NetBSD environment

### 2. Get the source code

You can get the source codes using several VCS. ([Official Repository List](repos.html))

### 3. Create config.h

Copy config.h.tmpl to config.h. Edit it if needed.

### 4. Build

Run `./make-installer.sh win32` or `./make-installer.sh win64`, and you should get these files:
 - install.exe (Setup program)
 - tewi.7z (Archive)

\language ^jp$

# MinGW-w64を使う {ビルド}

ここではMinGW-w64を使用してコンパイルする方法を紹介します。

## ステップ

### 1. 必要なもの
 - MinGW-w64
 - Linux環境かNetBSD環境

### 2. ソースコードを持って来る

ソースコードは色々なVCSを使って取得することができます。 ([公認リポジトリリスト](repos.html))

### 3. config.hを作成する

config.h.tmplをconfig.hにコピーしてください。必要があれば編集してください。

### 4. ビルドする

`./make-installer.sh win32`か`./make-installer.sh win64`を実行してください。そしたら以下のものが作成されるはずです:
 - install.exe (セットアッププログラム)
 - tewi.7z (アーカイブ)
