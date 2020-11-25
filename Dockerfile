FROM    ubuntu:20.10
LABEL   maintainer="Louis Ross <louis.ross@gmail.com"

ARG     MYDIR=/home/spiking-model
WORKDIR ${MYDIR}

COPY    install-deps ${MYDIR}/

RUN     echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections
RUN     bash ${MYDIR}/install-deps >>install-deps.log
CMD     ["bash"]