## Prerequisite ##

### ubuntu
* Version information

	```
	root@proxy:~# cat /etc/issue
	Ubuntu 16.04.2 LTS \n \l
	```

### docker
* Version information

	```
	root@proxy:~# docker version
	Client:
	 Version:      1.12.6
	 API version:  1.24
	 Go version:   go1.6.2
	 Git commit:   78d1802
	 Built:        Tue Jan 31 23:35:14 2017
	 OS/Arch:      linux/amd64
	
	Server:
	 Version:      1.12.6
	 API version:  1.24
	 Go version:   go1.6.2
	 Git commit:   78d1802
	 Built:        Tue Jan 31 23:35:14 2017
	 OS/Arch:      linux/amd64
	```

### [kubernetes](https://github.com/kubernetes/kubernetes) local cluster
* You can startup `v1.10.0` k8s local cluster by executing commands blow:

	```
	cd $HOME
	git clone https://github.com/kubernetes/kubernetes.git
	cd $HOME/kubernetes
	git checkout v1.10.0
	make
	echo alias kubectl='$HOME/kubernetes/cluster/kubectl.sh' >> /etc/profile
	ALLOW_PRIVILEGED=true FEATURE_GATES=CSIPersistentVolume=true,MountPropagation=true RUNTIME_CONFIG="storage.k8s.io/v1alpha1=true" LOG_LEVEL=5 hack/local-up-cluster.sh
	```

### [opensds](https://github.com/opensds/opensds) local cluster
* For testing purposes you can deploy OpenSDS refering to [OpenSDS Cluster Installation through Ansible](https://github.com/opensds/opensds/wiki/OpenSDS-Cluster-Installation-through-Ansible).

## Testing steps ##

* Change the workplace

	```
	cd /opt/opensds-k8s-linux-amd64
	```

* Create example nginx application

	```
	kubectl create -f csi/examples/kubernetes/nginx.yaml
	```

	This example will mount a opensds volume into ```/var/lib/www/html```.

	You can use the following command to inspect into nginx container to verify it.

	```
	docker exec -it <nginx container id> /bin/bash
	```

## Clean up steps ##

Clean up example nginx application and opensds CSI pods by the following commands.

```
kubectl delete -f csi/examples/kubernetes/nginx.yaml
kubectl delete -f csi/deploy/kubernetes
```
