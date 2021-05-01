# ServiceSolution

Windowsのサービスプログラムのテンプレートソリューション

この Microsoft Visual Studio Community 2019 版で作られたソリューションは、Windowsのサービスプログラムのひな型ソリューションとなるべく、Microsoftのサービスプログラムのサンプルから構築しています。

Doxygenの結果を以下の場所に保管しています。併せてご確認ください。

https://kumakuma0421.github.io/ServiceSolution/

## ServiceApplication1,2 プロジェクト

現在進行中のサービスプログラム用プロジェクトです。とりあえずですが、開始、終了、中断、再開が可能です。
ServiceApplication1とServiceApplication2の違いは、ServiceApplication2には依存先としてServiceApplication1を指定しているところです。テスト用なので、本当に依存している訳ではありません。

## ServiceLogics プロジェクト

ServiceApplication1,2プロジェクトの共有ロジックを格納しているのが、このServiceLogicsプロジェクトです。
DLLプロジェクトとして構成しています。

## ServiceMessage プロジェクト

上記サービスプログラムでイベントビューアー用に出力する文言DLLを出力するようにしたプロジェクトです。


# Microsoft謹製サービスプログラム

## Svc プロジェクト

これは、Microsoftのサンプルをできるだけ加工せず、サービスプログラムのリファレンスとなるように作ったものです。
サンプルソースのありかは[こちら](https://docs.microsoft.com/en-us/windows/win32/services/svc-cpp)から。

https://docs.microsoft.com/en-us/windows/win32/services/svc-cpp

サービスプログラムが出力する、イベントビューアー用のメッセージは、バッチファイルにて作成します。

## SvcConfig プロジェクト

これもMicrosoftのサンプルからリファレンス用に作りました。
サンプルソースのありかは[こちら](https://docs.microsoft.com/en-us/windows/win32/services/svcconfig-cpp)から。

https://docs.microsoft.com/en-us/windows/win32/services/svcconfig-cpp

## SvcControl プロジェクト

これも上記同様、Microsoftのサンプルからリファレンス用に作りました。
サンプルソースのありかは[こちら](https://docs.microsoft.com/en-us/windows/win32/services/svccontrol-cpp)から。

https://docs.microsoft.com/en-us/windows/win32/services/svccontrol-cpp