# libFDKK-linker

## 功能说明
1一个客户端稳定可用的socket连接库，代码中已经包含了一个下载的使用例子（src/FDEightLinkClient.h）

  ``` /**
      *@brief                   下载图片
      *@param desfilePath       图片存储的路径
      *@param filePath          下载图片的路径
      **/
     RESTULT_DATA_STRUCT  *take_file_downImage(char *desfilePath,char *filePath);
  ```
  2核心调用代码(src/FDEightLinkSocket.h)

##顺手写了一个CMakeLists，方便交叉编译
## 1.Requirements  

​    1.CMAKE request min VERSION 3.1 

​    <!--其他版本有可能也可以-->

​    2.Support MACOSX ,iOS,Android,Unix,Win32，Static library，dynamic library

## 2.Compilation configuration description

​      1.FDKK_LINKER_BUILD_TESTS    if you need a example please set it ON ,or not set OFF；

​      2.FDKK_LINKER_BUILD_DEPENDENT Dependency library required or not,Dependency libraries include     CJSON，s2j；

​      3.LINKER_BUILD_SHARED_LIBS  if you need a dynamic library for you specific target， set it ON

​      4.LINKER_APPLE_UNIVERSAL_BIN. When you set it ON ,for the Apple's  platform，it will product a universal library , it is worth to remind  you that it will product a library for arm64 ro arm64e when you set it OFF;

​      5.LINKER_MSVC_SHARED_RT:Build with shared runtime libs

​      6.LINKER_MSVC_STHREADED_RT: Build with single-threaded static runtime libs

# 3.product

​	If all goes well,you will get library named libFDKK-linker.a or libFDKK-linker.dll ,libFDKK-linker.so ,and so on.

​	Enjoy you life,Thanks!

