
## tensorflow-r1.2-windows-dll
Since tensorflow project only support CPU version dll,

I compiled tensorflow-r1.2 GPU version using CMake,VisualStudio 2015, CUDA8.0, cudnn5.1.

(tutorial would be updated in a few days)

- tensorflow.dll link:https://pan.baidu.com/s/1jIBriPw pwd:b4ay

- tensorflow.lib link: https://pan.baidu.com/s/1eS2BSQ2 pwd: t2a9

- libprotobuf.lib link: https://pan.baidu.com/s/1kVMK3Uv pwd: kbfv

- inception_v3.pb link:https://pan.baidu.com/s/1mi5jSms pwd:es67

## 中文版tensorflow.dll使用教程

## 准备工作

- 安装VisualStudio Community 2015 安装NVIDIA CUDA 8.0

- git clone https://github.com/BowieHsu/Tensorflow-windows-dll

- 下载tensorflow.dll,放置在Tensorflow-windows-dll/bin目录下

- 下载tensorflow.lib、libprotobuf.lib,放置在Tensorflow-windows-dll/extra/tensorflow-r1.2/目录下

- 下载inception_v3.pb，放置在Tensorflow-windows-dll/data目录下

- 打开Tensorflow-windows-dll/build/vc14目录下的visual studio Solution 'tensorflow_classification', 将编译选项为 **'Release' 'x64'**

## How to use tensorflow.dll

- Install VisualStudio Community2015 Install NVIDIA CUDA 8.0

- git clone https://github.com/BowieHsu/Tensorflow-windows-dll

- Download tensorflow.dll, place it under Tensorflow-windows-dll/bin

- Download tensorflow.lib and libprotobuf.lib, place theme under Tensorflow-windows-dll/extra/tensorflow-r1.2/

- Download inception_v3.pb, place it under Tensorflow-windows-dll/data

- Open Visual Studio Solution "tensorflow_classification.sln" which should be under Tensorflw-windows-dll/build/vc14，**Solution configurations option choose "Release", Soluton Platform option choose "x64"**
