## 1. How to install an opensds local cluster
### Pre-config (Ubuntu 16.04)
All the installation work is tested on `Ubuntu 16.04`, please make sure you have installed the right one. Also `root` user is suggested before the installation work starts.

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
If you want to integrate stor4nfv with k8s csi, please modify `nbp_plugin_type` to `csi` and also change `opensds_endpoint` field in `group_vars/common.yml`:
```yaml
# 'hotpot_only' is the default integration way, but you can change it to 'csi'
# or 'flexvolume'
nbp_plugin_type: hotpot_only
# The IP (127.0.0.1) should be replaced with the opensds actual endpoint IP
opensds_endpoint: http://127.0.0.1:50040
```

##### LVM
If `lvm` is chosen as storage backend, modify `group_vars/osdsdock.yml`:
```yaml
enabled_backend: lvm 
```

Modify ```group_vars/lvm/lvm.yaml```, change `tgtBindIp` to your real host ip if needed:
```yaml
tgtBindIp: 127.0.0.1 
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

##### Cinder
If `cinder` is chosen as storage backend, modify `group_vars/osdsdock.yml`:
```yaml
enabled_backend: cinder # Change it according to the chosen backend. Supported backends include 'lvm', 'ceph', and 'cinder'

# Use block-box install cinder_standalone if true, see details in:
use_cinder_standalone: true
```

Configure the auth and pool options to access cinder in `group_vars/cinder/cinder.yaml`. Do not need to make additional configure changes if using cinder standalone.

### Check if the hosts can be reached
```bash
ansible all -m ping -i local.hosts
```

### Run opensds-ansible playbook to start deploy
```bash
ansible-playbook site.yml -i local.hosts
```

## 2. How to test opensds cluster
### OpenSDS CLI
Firstly configure opensds CLI tool:
```bash
sudo cp /opt/opensds-linux-amd64/bin/osdsctl /usr/local/bin/
export OPENSDS_ENDPOINT=http://{your_real_host_ip}:50040
export OPENSDS_AUTH_STRATEGY=keystone
source /opt/stack/devstack/openrc admin admin

osdsctl pool list # Check if the pool resource is available
```

Then create a default profile:
```
osdsctl profile create '{"name": "default", "description": "default policy"}'
```

Create a volume:
```
osdsctl volume create 1 --name=test-001
```

List all volumes:
```
osdsctl volume list
```

Delete the volume:
```
osdsctl volume delete <your_volume_id>
```

### OpenSDS UI
OpenSDS UI dashboard is available at `http://{your_host_ip}:8088`, please login the dashboard using the default admin credentials: `admin/opensds@123`. Create tenant, user, and profiles as admin.

Logout of the dashboard as admin and login the dashboard again as a non-admin user to create volume, snapshot, expand volume, create volume from snapshot, create volume group.

## 3. How to purge and clean opensds cluster

### Run opensds-ansible playbook to clean the environment
```bash
ansible-playbook clean.yml -i local.hosts
```

### Run ceph-ansible playbook to clean ceph cluster if ceph is deployed
```bash
cd /opt/ceph-ansible
sudo ansible-playbook infrastructure-playbooks/purge-cluster.yml -i ceph.hosts
```

In addition, clean up the logical partition on the physical block device used by ceph, using the ```fdisk``` tool.

### Remove ceph-ansible source code (optional)
```bash
sudo rm -rf /opt/ceph-ansible
```
