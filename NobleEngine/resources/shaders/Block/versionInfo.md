メインアップデート.マイナーアップデート.パッチ  
<使用シェーダー>

## 1.0.0
<1.0.0/BlockMap.hlsli> <1.0.0/BlockMap.PS.hlsl> <1.0.0/BlockMap.VS.hlsl>  
立方体モデルをインスタンシング描画

## 1.1.0
<1.1.0/Block.VS.hlsl> <1.1.0/Block.PS.hlsl>  
テクスチャを2枚送れるようにした1.0.0
破壊テクスチャを重ねられるようになった

## 2.0.0
<2.0.0/Block.MS.hlsl> <2.0.0/Block.PS.hlsl>  
VS削除・MS追加・PS更新  
CPU側で生成されたメッシュをMSで描画。テクスチャは廃止しStructuredBufferで頂点ごとの色情報を渡すよう変更。
チャンク数分のモデルを描画するだけになりDrawCallが大幅に削減された。

## 2.1.0
<2.1.0/Block.MS.hlsl> <2.0.0/Block.PS.hlsl>  
MS更新  
MSにチャンクのブロック配列を渡し、MS内でメッシュを生成するよう変更。  
CPU側でのメッシュ生成が不要になった。

## 2.2.0
<2.2.0/Block.MS.hlsl> <2.0.0/Block.PS.hlsl>  
MS更新  
MSにチャンクのブロック配列を渡し、MA内で露出面のみのメッシュを生成するよう変更。  
描画頂点数が大幅に削減された。

## 3.0.0
<3.0.0/Block.MS.hlsl> <3.0.0/Block.CreateMesh.CS.hlsl> <2.0.0/Block.PS.hlsl> <3.0.0/Block.hlsli>  
MS更新・CreateMesh.CS追加・hlsli追加  
3.0.0で毎フレーム行っていた露出面のみのメッシュ生成をComputeShaderで行うよう変更。  
MSは描画のみを行うようになった。メッシュの再生成がチャンクの更新時のみになったためGPU負荷が大幅に削減された。
GPUFPS:130

## 4.0.0
<4.0.0/Block.MS.hlsl> <4.0.0/Block.AS.hlsl> <3.0.0/Block.CreateMesh.CS.hlsl> <2.0.0/Block.PS.hlsl> <3.0.0/Block.hlsli>  
MS更新・AS追加
ASを導入しASを通してから描画するように変更。  
描画頂点0のグループが描画されなくなりGPU負荷が削減された。
GPUFPS:190

## 4.1.0
<4.0.0/Block.MS.hlsl> <4.1.0/Block.AS.hlsl> <3.0.0/Block.CreateMesh.CS.hlsl> <2.0.0/Block.PS.hlsl> <3.0.0/Block.hlsli>  
AS更新  
描画頂点0カリング + 視錐台カリング
GPUFPS:170 ~ 220(マップの全体を見た時 ~ 通常のゲームプレイの範疇でマップを視界に入れた時)

## 4.2.0
<4.2.0/Block.MS.hlsl> <4.1.0/Block.AS.hlsl> <3.0.0/Block.CreateMesh.CS.hlsl> <4.2.0/Block.PS.hlsl> <3.0.0/Block.hlsli>  
MS更新・PS更新
辺に近い部分を濃く塗るように変更。見た目がきもかったためなかったことになるかもしれない。
