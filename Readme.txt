作品：DirectX11の自作ゲームエンジン
制作期間：2年


＊＊＊実行時の注意＊＊＊
・FBXSDKのlib、SkyBoxテクスチャは圧縮ファイルとして入っているため、ビルド前に解凍する必要があります。
(FBXSDK：external\FBXSDK\lib\vs2017\x86\)
(SkyBox：Asset\Textures\Skymaps)

・ビルド時にソリューションプラットフォームをx64からx86に変更する必要があります。

・DirectXMathで右手系に対応した関数を使用しています。
バージョンが対応していない場合は(external\DirectXMath\DoAdd.txt)指定先のDirectXMathを最新のものに更新する必要があります。