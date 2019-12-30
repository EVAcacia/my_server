# docker的使用

1. docker images   列出本地镜像。

2. docker search ubuntu 查找ubuntu镜像。

3. docker pull ubuntu        //将名为ubuntu的镜像下载到本地。

4. docker tag ubuntu qq624758472/ubuntu                                           

5. docker push qq624758472/ubuntu

   这里要注意一个问题,给自己镜像命名的时候格式应该是: docker注册用户名/镜像名,比如我的docker用户名为 test123,那么我的镜像tag就为 test123/whalesay,不然是push不上去的

6. docker run -it ubuntu /bin/bash     //启动镜像。