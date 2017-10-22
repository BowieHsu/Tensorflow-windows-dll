
## tensorflow-r1.2-windows-dll
Since tensorflow project only support CPU version dll,

I compiled tensorflow-r1.2 GPU version using CMake,VisualStudio 2015, CUDA8.0, cudnn5.1.

(tutorial would be updated in a few days)

-tensorflow.dll link:https://pan.baidu.com/s/1jIBriPw pwd:b4ay

-tensorflow.lib link: https://pan.baidu.com/s/1eS2BSQ2 pwd: t2a9

-libprotobuf.lib link: https://pan.baidu.com/s/1kVMK3Uv pwd: kbfv

##中文版tensorflow.dll使用教程
##准备工作
- 安装VisualStudio Community 2015 安装NVIDIA CUDA 8.0
- git clone https://github.com/BowieHsu/Tensorflow-windows-dll
- 下载tensorflow.dll tensorflow.lib libprotobuf.lib,放置在Tensorflow-window-dll/extra/tensorflow/目录下
- 打开visual studio Solution 'tensorflow_classification', 编译选项为'Release' 'x64'
