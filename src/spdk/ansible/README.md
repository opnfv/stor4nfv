# spdk-ansible: deploy spdk
## Install ssh server
    sudo apt-get install openssh-server

    if you use root to run spdk-ansible, you should open the file of
    /etc/ssh/sshd_config and modify:
    PermitRootLogin yes
    sudo /etc/init.d/ssh restart

    generate ssh-token:
    ssh-keygen -t rsa
    ssh-copy-id -i ~/.ssh/id_rsa.pub <romte_ip(eg: username@hostName or username@hostIp)>

##  Install the ansible tool:
    sudo add-apt-repository ppa:ansible/ansible
    sudo apt-get update
    sudo apt-get install ansible

##  Configure Inventory, default in /etc/ansible/hosts:
    [spdk_server]
    your_host_name or your_host_ip

##  Check if the hosts could be reached:
    ansible all -m ping

##  Download spdk-ansible
    git clone https://github.com/hellowaywewe/spdk-ansible.git

##  configure spdk-ansible
    Configure common.yml according to required vars.
    Configure site.yml according to required tasks.

## Run ansible playbook: (under spdk-ansible root directory)
    ansible-playbook site.yml  --extra-vars "ansible_sudo_pass=your_user_password"
    if you use root to run,you can execute directly:
    ansible-playbook site.yml
