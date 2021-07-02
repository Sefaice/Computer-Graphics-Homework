# 计算机图形学作业六

## 实现

### Phong Shading

Phong模型把光照分为三个部分：ambient，diffuse，specular。ambient环境光直接用系数ambientStrength改变光源的颜色得到结果：

```cpp
// ambient 环境光
vec3 ambient = ambientStrength * lightColor;
```


diffuse漫反射是根据光线和法向量的夹角大小计算的，这里需要注意光照的计算用到的法向量等需要在统一的坐标系下，这里我用的是世界坐标系，也可以用相机坐标系，但是法向量的坐标转换有一些特殊，不能直接乘model矩阵。diffuseStrength 控制漫反射的强度：

```cpp
// 顶点着色器中转换法向量到世界坐标系
Normal = mat3(transpose(inverse(model))) * normal;

// 片段着色器计算diffuse
vec3 norm = normalize(Normal);
vec3 lightDir = normalize(lightPos - FragPos);  
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diffuseStrength * diff * lightColor;
```

specluar是镜面反射，有两个参数控制反射的效果，specularPower控制镜面反射部分的大小，specularStrength控制强度（亮度）：

```cpp
// specular 反射
vec3 viewDir = normalize(viewPos - FragPos);
vec3 reflectDir = reflect(-lightDir, norm);  
float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);
vec3 specular = specularStrength * spec * lightColor;
```

### Gouraud Shading

Gouraud模型的计算和Phong类似，但是是在顶点着色器中进行的：

```cpp
// gouraud vertex shader
...
gl_Position =  projection * view * model * vec4(pos, 1.0);

// 在世界坐标系中计算光照
vec3 Normal = mat3(transpose(inverse(model))) * normal;
vec3 FragPos = vec3(model * vec4(pos, 1.0));
// ambient 环境光
vec3 ambient = ambientStrength * lightColor;
// diffuse 散射/漫射
vec3 norm = normalize(Normal);
vec3 lightDir = normalize(lightPos - FragPos);  
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diffuseStrength * diff * lightColor;
// specular 反射
vec3 viewDir = normalize(viewPos - FragPos);
vec3 reflectDir = reflect(-lightDir, norm);  
float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);
vec3 specular = specularStrength * spec * lightColor;

vec3 result = (ambient + diffuse + specular) * objectColor;
aColor = result;
```

这种设计这能大大降低运算次数，但是由于在顶点着色器中计计算光照颜色，最后得到的颜色会被Opengl插值，这样得到的效果会有明显的分界；且另一个非常明显的缺陷是，在绘制正方体时，由于一个面只有四个顶点，由于插值的关系，根本看不到specular的聚集效果；在绘制球体时，也能看到很明显的分层。

### 结果

所有颜色和关照参数都可以改变，并且可以选择绘制cube还是sphere，选用哪种光照模型：

![result](./imgs/result.gif)

由于录制原因gif中有很明显的分层。