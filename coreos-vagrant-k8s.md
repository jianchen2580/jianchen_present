Vagrant+CoreOS部署多节点k8s测试集群

# Install Prerequisites
## git

## Vagrant

## kubectl 

下载链接：
    https://github.com/kubernetes/kubernetes/blob/master/CHANGELOG.md#client-binaries-15

macOS（amd64）：

    wget https://dl.k8s.io/v1.6.0/kubernetes-client-darwin-amd64.tar.gz

Linux （amd64）：

    wget https://dl.k8s.io/v1.6.0/kubernetes-client-linux-amd64.tar.gz

# Clone the repo

    $ git clone https://github.com/coreos/coreos-kubernetes.git
    $ cd coreos-kubernetes/multi-node/vagrant

# Start the Machines

编辑config.rb.sample（或者不做任何改动使用默认配置）

    #$update_channel="alpha"
    
    #$controller_count=1
    #$controller_vm_memory=1024
    
    #$worker_count=1
    #$worker_vm_memory=1024
    
    #$etcd_count=1
    #$etcd_vm_memory=512

更新CoreOS vagrant image

    vagrant box update
    vagrant up

# Configure kubectl 

配置KUBECONFIG路径

    $ export KUBECONFIG="${KUBECONFIG}:$(pwd)/kubeconfig"
    $ kubectl config use-context vagrant-multi

# 验证

    $ kubectl get nodes
    NAME          LABELS                               STATUS
    172.17.4.201   kubernetes.io/hostname=172.17.4.201   Ready
