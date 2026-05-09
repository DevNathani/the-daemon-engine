FROM fedora:latest

# Enable docs when download packages for man -- docker disallows them by default
RUN sed -i '/tsflags=nodocs/d' /etc/dnf/dnf.conf

# Installing Packages
RUN dnf -y update && \
    dnf -y install \
	coreutils \
	grep \
	sed \
	binutils \
    openssl \
	bash \
	util-linux \
	sudo \
	clear \
	man-db \
	man-pages \
    openssh-server \
    openssh-clients \
    passwd \
    iproute \
    procps-ng \
    which \
    vim \
	rsync \
	nano \
    sqlite-libs \ 
        less && \
    dnf clean all

# Generate SSH host keys
RUN ssh-keygen -A

# Create a game group for players
RUN groupadd game

# Create gameadmin group for admins
RUN groupadd gameadmin

# Create admin Username = admin | Password = admin@123
RUN useradd -m admin && echo "admin:admin@123" | chpasswd

# Add admin to gameadmin group 
RUN usermod -aG gameadmin admin


#SSH CONFIGURATION
RUN sed -i 's/#Port 22/Port 22/' /etc/ssh/sshd_config && \
sed -i 's/#ListenAddress 0.0.0.0/ListenAddress 0.0.0.0/' /etc/ssh/sshd_config && \
sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config && \
sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin no/' /etc/ssh/sshd_config && \
sed -i 's/UsePAM no/UsePAM yes/' /etc/ssh/sshd_config && \
echo "LoginGraceTime 30" >> /etc/ssh/sshd_config && \
echo "AllowTcpForwarding no" >> /etc/ssh/sshd_config && \
echo "X11Forwarding no" >> /etc/ssh/sshd_config && \
echo "PermitTunnel no" >> /etc/ssh/sshd_config && \
sed -i 's/^#\?PrintLastLog.*/PrintLastLog no/' /etc/ssh/sshd_config && \
echo "Match User admin" >> /etc/ssh/sshd_config && \
echo "    MaxSessions 10" >> /etc/ssh/sshd_config && \
echo "    AllowTcpForwarding yes" >> /etc/ssh/sshd_config

# SECURITY AVOID PLAYERS TO CREATE ANY SERVERS
RUN dnf remove -y python3
RUN chmod 000 /usr/bin/nc || true && \
    chmod 000 /usr/bin/ncat || true && \
    chmod 000 /usr/bin/socat || true


# Game Files
COPY game /opt/game
RUN chown -R root:root /opt/game/bin
RUN chown root:root /opt/game/assets/*
RUN chmod 444 /opt/game/assets/*
RUN chmod 711 /opt/game/bin
RUN chown root:root /opt/game/lib/*
RUN chmod 600 /opt/game/lib/*

# Permission for Players scripts
RUN chmod 111 /opt/game/bin/player/unlock
RUN chmod 111 /opt/game/bin/player/reload

RUN chown root:root /opt/game/bin/player/check_and_advance
RUN chmod 4711 /opt/game/bin/player/check_and_advance

RUN chown root:root /opt/game/bin/player/giveup
RUN chmod 4711 /opt/game/bin/player/giveup

RUN chown root:root /opt/game/bin/player/load_level
RUN chmod 4711 /opt/game/bin/player/load_level

RUN chmod 111 /opt/game/bin/player/game_complete

RUN chown root:root /opt/game/bin/utils/get_level
RUN chmod 4711 /opt/game/bin/utils/get_level

RUN chown root:root /opt/game/bin/utils/get_level_name
RUN chmod 4711 /opt/game/bin/utils/get_level_name

# Player Sudoers
COPY docker/game_sudoers /etc/sudoers.d/game
RUN chmod 440 /etc/sudoers.d/game

# Make submit command available
RUN ln -s /opt/game/bin/player/unlock /usr/local/bin/unlock
RUN ln -s /opt/game/bin/player/reload /usr/local/bin/reload
RUN ln -s /opt/game/bin/player/giveup /usr/local/bin/giveup
RUN ln -s /opt/game/bin/player/instructions /usr/local/bin/instructions

# Player Login Behaviour
RUN mkdir -p /etc/game/
COPY private/templates/bashrc /etc/game/.bashrc
RUN chown root:root /etc/game/.bashrc  && \
    chmod 444 /etc/game/.bashrc 

RUN echo '' >> /etc/bashrc && \
    echo '# Player Shell' >> /etc/bashrc && \
    echo 'if [[ $- == *i* ]] && id -nG "$USER" 2>/dev/null | grep -qw game; then' >> /etc/bashrc && \
    echo '  [ -f /etc/game/.bashrc ] && source /etc/game/.bashrc' >> /etc/bashrc && \
    echo 'fi' >> /etc/bashrc

# Restricting Commands
RUN chown root:wheel /usr/bin/su
RUN chmod 4750 /usr/bin/su
RUN chmod 750 /usr/bin/newgrp
RUN chmod 700 /bin/login
RUN chown root:root /etc/profile
RUN chmod 644 /etc/profile

# Securing Levels Scripts
RUN chown -R root:root /opt/game/levels

# Securing Admin Scripts
RUN chown -R admin:admin /opt/game/bin/admin
RUN chmod 700 /opt/game/bin/admin

# Admin Sudoer
COPY docker/admin_sudoers /etc/sudoers.d/admin
RUN chmod 440 /etc/sudoers.d/admin

# Admin bash_profile 
COPY private/templates/admin_bash_profile /home/admin/.bash_profile
RUN chown admin:admin /home/admin/.bash_profile && chmod 644 /home/admin/.bash_profile

# Admin bashrc
COPY private/templates/admin_bashrc /home/admin/.bashrc
RUN chown admin:admin /home/admin/.bashrc && chmod 644 /home/admin/.bashrc

# Player Login Behaviour
RUN mkdir -p /etc/game/
COPY private/templates/bashrc /etc/game/.bashrc
RUN chown root:root /etc/game/.bashrc  && \
    chmod 444 /etc/game/.bashrc 

RUN echo '' >> /etc/bashrc && \
    echo '# Player Shell' >> /etc/bashrc && \
    echo 'if [[ $- == *i* ]] && id -nG "$USER" 2>/dev/null | grep -qw game; then' >> /etc/bashrc && \
    echo '  [ -f /etc/game/.bashrc ] && source /etc/game/.bashrc' >> /etc/bashrc && \
    echo 'fi' >> /etc/bashrc


# INIT state script
COPY docker/init_state.sh /usr/local/bin/init_state.sh
RUN chown root:root /usr/local/bin/init_state.sh
RUN chmod 700 /usr/local/bin/init_state.sh

# Expose SSH port
EXPOSE 22

# Start sshd
ENTRYPOINT ["/usr/local/bin/init_state.sh"]
CMD ["/usr/sbin/sshd", "-D"]

