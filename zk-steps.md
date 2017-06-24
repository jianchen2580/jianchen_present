
# zookeeper 

Headless Service - zk-headless

ConfigMap - zk-config

PodDisruptionBudget - zk-budget 

StatefulSet - zk

# StatefulSet controller create 

    kubectl get pods -w -l app=zk

# get hostname 

    for i in 0 1 2; do kubectl exec zk-$i -- hostname; done

StatefulSet controller 提供每个Pod 一个unique hostname（<statefulset name>-<ordinal index>）

# Examine the contents of the myid file for each server

    for i in 0 1 2; do echo "myid zk-$i";kubectl exec zk-$i -- cat /var/lib/zookeeper/data/myid; done

zk-headless 为所有pod创建，FQDN (Fully Qualified Domain Name) 
    
    for i in 0 1 2; do kubectl exec zk-$i -- hostname -f; done

    zk-0.zk-headless.default.svc.cluster.local
    zk-1.zk-headless.default.svc.cluster.local
    zk-2.zk-headless.default.svc.cluster.local

zk的配置文件zoo.cfg

    kubectl exec zk-0 -- cat /opt/zookeeper/conf/zoo.cfg

    clientPort=2181
    dataDir=/var/lib/zookeeper/data
    dataLogDir=/var/lib/zookeeper/log
    tickTime=2000
    initLimit=10
    syncLimit=2000
    maxClientCnxns=60
    minSessionTimeout= 4000
    maxSessionTimeout= 40000
    autopurge.snapRetainCount=3
    autopurge.purgeInteval=0
    server.1=zk-0.zk-headless.default.svc.cluster.local:2888:3888
    server.2=zk-1.zk-headless.default.svc.cluster.local:2888:3888
    server.3=zk-2.zk-headless.default.svc.cluster.local:2888:3888

headless顺序创建pod

测试zookeeper， zk-0写world到路径／hello

    kubectl exec zk-0 zkCli.sh create /hello world

在zk-1获取／hello

    kubectl exec zk-1 zkCli.sh get /hello

删除 stateful zk

    kubectl delete statefulset zk
    statefulset "zk" deleted

查看Pod删除过程

    kubectl get pods -w -l app=zk

重新apply zookeeper.yaml

    kubectl apply -f zookeeper.yaml

    kubectl get pods -w -l app=zk

查看／hello上面的数据是否还在(因为persist到硬盘,数据还在)

    kubectl exec zk-2 zkCli.sh get /hello 

查看pvc

    kubectl get pvc -l app=zk

## ConfigMap

    apiVersion: v1
    kind: ConfigMap
    metadata:
      name: zk-config
    data:
      ensemble: "zk-0;zk-1;zk-2"
      jvm.heap: "1G"
      tick: "2000"
      init: "10"
      sync: "5"
      client.cnxns: "60"
      snap.retain: "3"
      purge.interval: "1"

查看容器ENV

    for i in 0 1 2; do kubectl exec zk-$i env | grep "ZK_*";echo""; done

查看zk User
Pod template

    securityContext:
      runAsUser: 1000
      fsGroup: 1000

查看zk user

    kubectl exec zk-0 -- ps -elf

kill zk process
    
    kubectl exec zk-0 -- ps -ef
    kubectl get pod -w -l app=zk
    kubectl exec zk-0 -- pkill java

Liveness

Pod template 

    livenessProbe:
            exec:
              command:
              - "zkOk.sh"
            initialDelaySeconds: 15
            timeoutSeconds: 5

验证

    kubectl get pod -w -l app=zk
    kubectl exec zk-0 -- rm /opt/zookeeper/bin/zkOk.sh
    kubectl get pod -w -l app=zk

部署到不同node
affinity：requiredDuringSchedulingRequiredDuringExecution告诉schedule不要co-locate两个pod到同一个node

    affinity:
      podAntiAffinity:
        requiredDuringSchedulingIgnoredDuringExecution:
          - labelSelector:
              matchExpressions:
                - key: "app"
                  operator: In
                  values: 
                  - zk-headless
            topologyKey: "kubernetes.io/hostname"

验证：

    for i in 0 1 2; do kubectl get pod zk-$i --template {{.spec.nodeName}}; echo ""; done

# poddisruptionbudget
定义最少zk pod必须存活
    
    kubectl get poddisruptionbudget zk-budget
    
    NAME        MIN-AVAILABLE   ALLOWED-DISRUPTIONS   AGE
    zk-budget   2               1                     1h