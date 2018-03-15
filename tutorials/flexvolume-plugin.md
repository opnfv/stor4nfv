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
* Version information
	```
	root@proxy:~# kubectl version
	Client Version: version.Info{Major:"1", Minor:"9+", GitVersion:"v1.9.0-beta.0-dirty", GitCommit:"a0fb3baa71f1559fd42d1acd9cbdd8a55ab4dfff", GitTreeState:"dirty", BuildDate:"2017-12-13T09:22:09Z", GoVersion:"go1.9.2", Compiler:"gc", Platform:"linux/amd64"}
	Server Version: version.Info{Major:"1", Minor:"9+", GitVersion:"v1.9.0-beta.0-dirty", GitCommit:"a0fb3baa71f1559fd42d1acd9cbdd8a55ab4dfff", GitTreeState:"dirty", BuildDate:"2017-12-13T09:22:09Z", GoVersion:"go1.9.2", Compiler:"gc", Platform:"linux/amd64"}
	```
* You can startup the k8s local cluster by executing commands blow:

	```
	cd $HOME
	git clone https://github.com/kubernetes/kubernetes.git
	cd $HOME/kubernetes
	git checkout v1.9.0
	make
	echo alias kubectl='$HOME/kubernetes/cluster/kubectl.sh' >> /etc/profile
	RUNTIME_CONFIG=settings.k8s.io/v1alpha1=true AUTHORIZATION_MODE=Node,RBAC hack/local-up-cluster.sh -O
	```


### [opensds](https://github.com/opensds/opensds) local cluster
* For testing purposes you can deploy OpenSDS local cluster referring to ```ansible/README.md```.

## Testing steps ##

* Create service account, role and bind them.
	```
	cd /opt/opensds-k8s-{release version}-linux-amd64/provisioner
	kubectl create -f serviceaccount.yaml
	kubectl create -f clusterrole.yaml
	kubectl create -f clusterrolebinding.yaml
	```

* Change the opensds endpoint IP in pod-provisioner.yaml
The IP ```192.168.56.106``` should be replaced with the OpenSDS osdslet actual endpoint IP.
    ```yaml
    kind: Pod
    apiVersion: v1
    metadata:
      name: opensds-provisioner
    spec:
      serviceAccount: opensds-provisioner
      containers:
        - name: opensds-provisioner
          image: opensdsio/opensds-provisioner:latest
          securityContext:
          args:
            - "-endpoint=http://192.168.56.106:50040" # should be replaced
          imagePullPolicy: "IfNotPresent"
    ```

* Create provisioner pod.
	```
	kubectl create -f pod-provisioner.yaml
	```

* You can use the following cammands to test the OpenSDS FlexVolume and Proversioner functions.

	```
	kubectl create -f sc.yaml              # Create StorageClass
	kubectl create -f pvc.yaml             # Create PVC
	kubectl create -f pod-application.yaml # Create busybox pod and mount the block storage.
	```

	Execute the `findmnt|grep opensds` to confirm whether the volume has been provided.

## Clean up steps ##

```
kubectl delete -f pod-application.yaml
kubectl delete -f pvc.yaml
kubectl delete -f sc.yaml

kubectl delete -f pod-provisioner.yaml
kubectl delete -f clusterrolebinding.yaml
kubectl delete -f clusterrole.yaml
kubectl delete -f serviceaccount.yaml
```