# nbp-ansible
This is an installation tool for opensds northbound plugins using ansible.

## Install work

### Pre-config (Ubuntu 16.04)
First download some system packages:
```
sudo apt-get install -y openssh-server git
```
Then config ```/etc/ssh/sshd_config``` file and change one line:
```conf
PermitRootLogin yes
```
Next generate ssh-token:
```bash
ssh-keygen -t rsa
ssh-copy-id -i ~/.ssh/id_rsa.pub <ip_address> # IP address of the target machine of the installation
```

### Install docker
If use a standalone cinder as backend, you also need to install docker to run cinder service. Please see the [docker installation document](https://docs.docker.com/engine/installation/linux/docker-ce/ubuntu/) for details.

### Install ansible tool
```bash
sudo add-apt-repository ppa:ansible/ansible # This step is needed to upgrade ansible to version 2.4.2 which is required for the ceph backend.
sudo apt-get update
sudo apt-get install ansible
ansible --version # Ansible version 2.4.2 or higher is required for ceph; 2.0.0.2 or higher is needed for other backends.
```

### Check if the hosts can be reached
```bash
sudo ansible all -m ping -i nbp.hosts
```

### Run opensds-ansible playbook to start deploy
```bash
sudo ansible-playbook site.yml -i nbp.hosts
```

## Uninstall work

### Run nbp-ansible playbook to clean the environment
```bash
sudo ansible-playbook clean.yml -i nbp.hosts
```