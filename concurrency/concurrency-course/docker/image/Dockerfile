FROM ubuntu:20.04

COPY install_deps.sh /
RUN sh install_deps.sh

ENV TZ=Europe/Moscow

ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8

ENV CXX="/usr/bin/clang++-14"
ENV CC="/usr/bin/clang-14"

# SSH
EXPOSE 22
RUN echo "StrictHostKeyChecking=no" >> /etc/ssh/ssh_config
RUN mkdir /var/run/sshd
CMD ["/usr/sbin/sshd", "-D"]
