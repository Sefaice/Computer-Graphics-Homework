优化：

初始20fps

1.倒影绘制不绘制光照，绘制更少的顶点。
   倒影不绘制模型，forward shading和deferred shading都是60fps（全部去掉房子，不去掉是30）

2.deferred shading.

可能没啥用，它优化的都是fragment shader中多余的工作，主要集中在lighting，而terrain的fs中只有纹理读取和shadow，没有lighting。最终帧率的确没有提升，都在20左右。

terrain由于效果并没有用到光照模型，而model需要用到，导致采用deferred shading时最后这两个物体不能绘制到一个texture中，或在一个texture中用变量开关光照模型。这里的主要问题是，采用不同光照模型的物体不能用一个gbuffer。  

3.从场景中去掉房子——30fps。

4.CSM最后30fps，没有房子，deferred shading。效果不错。问题：过渡太突兀，peter panning用面剔除也无法解决。





