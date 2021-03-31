# ServiceSolution

Windowsのサービスプログラムのテンプレートソリューション

この Microsoft Visual Studio Community 2019 版で作られたソリューションは、Windowsのサービスプログラムのひな型ソリューションとなるべく、Microsoftのサービスプログラムのサンプルから構築しています。

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

## ServiceApplication1 プロジェクト

現在進行中のサービスプログラム用プロジェクトです。とりあえずですが、開始、終了、中断、再開が可能です。
イベントビューアー用のメッセージは、下記のプロジェクトで作成するようにしました。

## ServiceMessage プロジェクト

上記サービスプログラムでイベントビューアー用に出力する文言DLLを出力するようにしたプロジェクトです。

## RegistryAccess プロジェクト

サービスプログラムのレジストリ登録は、サービスプログラムのパラメータで設定、解除ができますが、メッセージDLLの登録、解除は別に必要になります。
現時点ではまだ、組み込んでいませんが単体でのレジストリ操作が一通りできるように組みました。もう少ししたらこの機能を搭載する予定です。

## MessageResource1、MessageDriver1 プロジェクト

イベントビューアーではMCファイルを使用して文言を出力しますが、そもそもWindowsはリソースとしての”メッセージテーブル”を持っています。
このメッセージテーブルからの文言取得は果たして遅いのだろうか、という疑問からこのプロジェクトを作りました。結論としては、「十分早い」です。
結局のところ、RCファイルのメッセージテーブルは、このMCファイルのメッセージと機能が被るので、どっちつかずな感じです。

## EventViewerTest プロジェクト

イベントビューアーもWindows10でかなり進化したみたいですが、逆に「カテゴリー」に関しては出力できるアプリと出力できないアプリがあります。
私が作ったこのサービスもやはり、出力できません。できるものと、できないものがある以上、なぜなのかを突き詰めるべく、プロジェクトを作成しました。
つまり、テスト用です。CategoryMessageFileに関して相当Googleってみましたが、誰も出力できないことに疑問を持たないようで、情報はほとんどありません。
知っている方は御一報ください！

## ConsoleApplication1 プロジェクト

完全に、WindowsAPIの動作確認用プロジェクトです。

そして、このソリューションはまだ完全なテストができていませんし、所々冗長なコードが散見されます。APIの使用方法について参照するぐらいは耐えられると思いますが、
テンプレートソリューションにはまだほど遠いのが現状です。
