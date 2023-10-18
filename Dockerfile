FROM ubuntu:jammy
LABEL authors="kyle"

ARG DEBIAN_FRONTEND=noninteractive
ARG AUTOMATED_MODE=yes
WORKDIR /tmp
ENV TZ=America/Los_Angeles
RUN apt-get update -y && apt-get install -y gpg sudo ca-certificates apt-transport-https ssh tar gzip

RUN mkdir scripts
WORKDIR scripts
ADD scripts .
WORKDIR /tmp
RUN bash ./scripts/install-depdendencies.sh && rm -r *
RUN     apt-get autoclean && \
        apt-get autoremove && \
        apt-get clean && \
        rm -rf /var/lib/apt/lists/*