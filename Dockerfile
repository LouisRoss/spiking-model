FROM    ubuntu:20.10
LABEL   maintainer="Louis Ross <louis.ross@gmail.com"

ARG     MYDIR=/home/spiking-model
WORKDIR ${MYDIR}

COPY    install-deps ${MYDIR}/
COPY    bmtk.zip ${MYDIR}/

RUN     echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections
RUN     bash ${MYDIR}/install-deps ${MYDIR} >>install-deps.log
CMD     ["bash"]