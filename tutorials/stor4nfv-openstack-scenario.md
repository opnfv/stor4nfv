# OpenSDS Integration with OpenStack on Ubuntu

All the installation work is tested on `Ubuntu 16.04`, please make sure you have
installed the right one.

## Environment Prepare

* OpenStack (Supposed you have deployed)
```shell
openstack endpoint list # Check the endpoint of the killed cinder service
```

* packages

Install following packages:
```bash
apt-get install -y git curl wget
```
* docker

Install docker:
```bash
wget https://download.docker.com/linux/ubuntu/dists/xenial/pool/stable/amd64/docker-ce_18.03.1~ce-0~ubuntu_amd64.deb
dpkg -i docker-ce_18.03.1~ce-0~ubuntu_amd64.deb 
```
* golang

Check golang version information:
```bash
root@proxy:~# go version
go version go1.9.2 linux/amd64
```
You can install golang by executing commands below:
```bash
wget https://storage.googleapis.com/golang/go1.9.2.linux-amd64.tar.gz
tar -C /usr/local -xzf go1.9.2.linux-amd64.tar.gz
echo 'export PATH=$PATH:/usr/local/go/bin' >> /etc/profile
echo 'export GOPATH=$HOME/gopath' >> /etc/profile
source /etc/profile
```

## Start deployment
### Download opensds-installer code
```bash
git clone https://gerrit.opnfv.org/gerrit/stor4nfv
cd stor4nfv/ci/ansible
```

### Install ansible tool
To install ansible, run the commands below:
```bash
# This step is needed to upgrade ansible to version 2.4.2 which is required for the "include_tasks" ansible command.
chmod +x ./install_ansible.sh && ./install_ansible.sh
ansible --version # Ansible version 2.4.x is required.
```

### Configure opensds cluster variables:
##### System environment:
Change `opensds_endpoint` field in `group_vars/common.yml`:
```yaml
# The IP (127.0.0.1) should be replaced with the opensds actual endpoint IP
opensds_endpoint: http://127.0.0.1:50040
```

Change `opensds_auth_strategy` field to `noauth` in `group_vars/auth.yml`:
```yaml
# OpenSDS authentication strategy, support 'noauth' and 'keystone'.
opensds_auth_strategy: noauth
```

##### Ceph
If `ceph` is chosen as storage backend, modify `group_vars/osdsdock.yml`:
```yaml
enabled_backend: ceph # Change it according to the chosen backend. Supported backends include 'lvm', 'ceph', and 'cinder'.
```

Configure ```group_vars/ceph/all.yml``` with an example below:
```yml
ceph_origin: repository
ceph_repository: community
ceph_stable_release: luminous # Choose luminous as default version
public_network: "192.168.3.0/24" # Run 'ip -4 address' to check the ip address
cluster_network: "{{ public_network }}"
monitor_interface: eth1 # Change to the network interface on the target machine
devices: # For ceph devices, append ONE or MULTIPLE devices like the example below:
  - '/dev/sda' # Ensure this device exists and available if ceph is chosen
  #- '/dev/sdb'  # Ensure this device exists and available if ceph is chosen
osd_scenario: collocated
```

### Check if the hosts can be reached
```bash
ansible all -m ping -i local.hosts
```

### Run opensds-ansible playbook to start deploy
```bash
ansible-playbook site.yml -i local.hosts
```

And next build and run cindercompatibleapi module:
```shell
cd $GOPATH/src/github.com/opensds/opensds
go build -o ./build/out/bin/cindercompatibleapi github.com/opensds/opensds/contrib/cindercompatibleapi
```

## Test
```shell
export CINDER_ENDPOINT=http://10.10.3.173:8776/v3 # Use endpoint shown above
export OPENSDS_ENDPOINT=http://127.0.0.1:50040

./build/out/bin/cindercompatibleapi
```

Then you can execute some cinder cli commands to see if the result is correct,
for example if you execute the command `cinder type-list`, the result will show
the profile of opnesds.

For detailed test instruction, please refer to the 5.3 section in
[OpenSDS Aruba PoC Plan](https://github.com/opensds/opensds/blob/development/docs/test-plans/OpenSDS_Aruba_POC_Plan.pdf).
